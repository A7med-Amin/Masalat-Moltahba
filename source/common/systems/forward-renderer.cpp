#include "forward-renderer.hpp"
#include "../mesh/mesh-utils.hpp"
#include "../texture/texture-utils.hpp"
#define DIRECTIONAL 0
#define POINT 1
#define SPOT 2
#define SPOT_NUM 6
namespace our
{

    void ForwardRenderer::initialize(glm::ivec2 windowSize, const nlohmann::json &config)
    {
        // First, we store the window size for later use
        this->windowSize = windowSize;

        // Then we check if there is a sky texture in the configuration
        if (config.contains("sky"))
        {
            // First, we create a sphere which will be used to draw the sky
            this->skySphere = mesh_utils::sphere(glm::ivec2(16, 16));

            // We can draw the sky using the same shader used to draw textured objects
            ShaderProgram *skyShader = new ShaderProgram();
            skyShader->attach("assets/shaders/textured.vert", GL_VERTEX_SHADER);
            skyShader->attach("assets/shaders/textured.frag", GL_FRAGMENT_SHADER);
            skyShader->link();

            // TODO: (Req 10) Pick the correct pipeline state to draw the sky
            //  Hints: the sky will be draw after the opaque objects so we would need depth testing but which depth funtion should we pick?
            //  We will draw the sphere from the inside, so what options should we pick for the face culling.

            PipelineState skyPipelineState{};
            // Firstly, enable depth testing to make sure that pixels that have less depth compared to camera will be drawen
            skyPipelineState.faceCulling.enabled = true;
            skyPipelineState.faceCulling.culledFace = GL_FRONT;
            skyPipelineState.depthTesting.enabled = true;
            // Secondly, choose the depth function and it will be LEQUAL to make sure that objects that have less depth will be drawen oven objects that have higher depth
            skyPipelineState.depthTesting.function = GL_LEQUAL;

            // Load the sky texture (note that we don't need mipmaps since we want to avoid any unnecessary blurring while rendering the sky)
            std::string skyTextureFile = config.value<std::string>("sky", "");
            Texture2D *skyTexture = texture_utils::loadImage(skyTextureFile, false);

            // Setup a sampler for the sky
            Sampler *skySampler = new Sampler();
            skySampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            skySampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            skySampler->set(GL_TEXTURE_WRAP_S, GL_REPEAT);
            skySampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Combine all the aforementioned objects (except the mesh) into a material
            this->skyMaterial = new TexturedMaterial();
            this->skyMaterial->shader = skyShader;
            this->skyMaterial->texture = skyTexture;
            this->skyMaterial->sampler = skySampler;
            this->skyMaterial->pipelineState = skyPipelineState;
            this->skyMaterial->tint = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            this->skyMaterial->alphaThreshold = 1.0f;
            this->skyMaterial->transparent = false;
        }

        // Then we check if there is a postprocessing shader in the configuration
        if (config.contains("postprocess"))
        {
            // TODO: (Req 11) Create a framebuffer
            //  this function create a Framebuffer and takes two parameters:
            //  1 -> means that 1 framebuffer will be generated after that line
            //  second paramters is a pointer to the variable that name or(ID) of Framebuffer which will be generated
            //  by this function will be stored in that variable
            glGenFramebuffers(1, &postprocessFrameBuffer);
            // this binding function function tell that any operation will be done to this fragement buffer
            // GL_DRAW_FRAMEBUFFER means that any operation will be done for drawing target or purpose
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postprocessFrameBuffer);
            // TODO: (Req 11) Create a color and a depth texture and attach them to the framebuffer
            //  Hints: The color format can be (Red, Green, Blue and Alpha components with 8 bits for each channel).
            //  The depth format can be (Depth component with 24 bits).
            colorTarget = texture_utils::empty(GL_RGBA8, windowSize);
            depthTarget = texture_utils::empty(GL_DEPTH_COMPONENT24, windowSize);
            // this function takes 5 parameters:
            // GL_FRAMEBUFFER: it indicates that the operations will be done to the currently bounded framebuffer
            // GL_TEXTURE_2D: it specifies that the target texture will be 2D texture
            // colorTarget->getOpenGLName(): takes a name of texture object not a texture itself
            // 0: represents a mipmap level of texture that i attached to framebuffer (highest resolution of texture)
            // GL_COLOR_ATTACHMENT0: first color attachment to the framebuffer
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTarget->getOpenGLName(), 0);
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTarget->getOpenGLName(), 0);
            // TODO: (Req 11) Unbind the framebuffer just to be safe
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            // Create a vertex array to use for drawing the texture
            glGenVertexArrays(1, &postProcessVertexArray);

            // Create a sampler to use for sampling the scene texture in the post processing shader
            Sampler *postprocessSampler = new Sampler();
            postprocessSampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            postprocessSampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            postprocessSampler->set(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            postprocessSampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Create the post processing shader
            ShaderProgram *postprocessShader = new ShaderProgram();
            postprocessShader->attach("assets/shaders/fullscreen.vert", GL_VERTEX_SHADER);
            postprocessShader->attach(config.value<std::string>("postprocess", ""), GL_FRAGMENT_SHADER);
            postprocessShader->link();

            // Create a post processing material
            postprocessMaterial = new TexturedMaterial();
            postprocessMaterial->shader = postprocessShader;
            postprocessMaterial->texture = colorTarget;
            postprocessMaterial->sampler = postprocessSampler;
            // The default options are fine but we don't need to interact with the depth buffer
            // so it is more performant to disable the depth mask
            postprocessMaterial->pipelineState.depthMask = false;
        }
    }

    void ForwardRenderer::destroy()
    {
        // Delete all objects related to the sky
        if (skyMaterial)
        {
            delete skySphere;
            delete skyMaterial->shader;
            delete skyMaterial->texture;
            delete skyMaterial->sampler;
            delete skyMaterial;
        }
        // Delete all objects related to post processing
        if (postprocessMaterial)
        {
            glDeleteFramebuffers(1, &postprocessFrameBuffer);
            glDeleteVertexArrays(1, &postProcessVertexArray);
            delete colorTarget;
            delete depthTarget;
            delete postprocessMaterial->sampler;
            delete postprocessMaterial->shader;
            delete postprocessMaterial;
        }
    }

    void ForwardRenderer::render(World *world, const std::string &postProcessFilter)
    {
        // First of all, we search for a camera and for all the mesh renderers
        CameraComponent *camera = nullptr;
        opaqueCommands.clear();
        transparentCommands.clear();
        point_directional_lights.clear();
        spot_light.clear();
        for (auto entity : world->getEntities())
        {
            // If we hadn't found a camera yet, we look for a camera in this entity
            if (!camera)
                camera = entity->getComponent<CameraComponent>();
            // If this entity has a mesh renderer component
            if (auto meshRenderer = entity->getComponent<MeshRendererComponent>(); meshRenderer)
            {
                // We construct a command from it
                RenderCommand command;
                command.localToWorld = meshRenderer->getOwner()->getLocalToWorldMatrix();
                command.center = glm::vec3(command.localToWorld * glm::vec4(0, 0, 0, 1));
                command.mesh = meshRenderer->mesh;
                command.material = meshRenderer->material;
                // if it is transparent, we add it to the transparent commands list
                if (command.material->transparent)
                {
                    transparentCommands.push_back(command);
                }
                else
                {
                    // Otherwise, we add it to the opaque command list
                    opaqueCommands.push_back(command);
                }
            }
            // TODO: (Phase 2) add entity light component to the list of lights
            if (auto light = entity->getComponent<LightComponent>(); light)
            {
                if (light->light_type == SPOT)
                {
                    spot_light.push_back(light);
                }
                else
                {
                    point_directional_lights.push_back(light);
                }
            }
        }

        // If there is no camera, we return (we cannot render without a camera)
        if (camera == nullptr)
            return;

        // TODO: (Req 9) Modify the following line such that "cameraForward" contains a vector pointing the camera forward direction
        //  HINT: See how you wrote the CameraComponent::getViewMatrix, it should help you solve this one
        //  Firstly, i got the entity owner of the camera and from it, i got the transformation matrix that transforms
        //  from the local space to the world space
        auto cameraOwner = camera->getOwner();
        auto Matrix = cameraOwner->getLocalToWorldMatrix();
        glm::vec3 positionOfCamera = Matrix * glm::vec4(0, 0, 0, 1);
        glm::vec3 positionOfCameraPoints = Matrix * glm::vec4(0, 0, -1, 1);
        glm::vec3 cameraForward = glm::normalize(positionOfCameraPoints - positionOfCamera);

        std::sort(transparentCommands.begin(), transparentCommands.end(), [cameraForward](const RenderCommand &first, const RenderCommand &second)
                  {
            //TODO: (Req 9) Finish this function
            // HINT: the following return should return true "first" should be drawn before "second". 
            // here i need to check the first will be rendered first before the second so, i will use
            // dot product between the first and forward and second and forward and the first should be greter than first after dot product
            return glm::dot(cameraForward , first.center) > glm::dot(cameraForward , second.center); });
        // This function used to sort spot lights ascendingly according to the distance from the camera
        // Calculate the world position for two lights (first , second) and then use dot product to sort ascendingly
        std::sort(spot_light.begin(), spot_light.end(), [cameraForward](const LightComponent *first, const LightComponent *second)
                  {
            // get world position of first light
            auto firstPosition = glm::vec3(first->getOwner()->getLocalToWorldMatrix() * glm::vec4(first->getOwner()->localTransform.position, 1.0));
            auto secondPosition = glm::vec3(second->getOwner()->getLocalToWorldMatrix() * glm::vec4(second->getOwner()->localTransform.position, 1.0));
            return firstPosition.x > secondPosition.x; });
        // TODO: (Req 9) Get the camera ViewProjection matrix and store it in VP
        //  to get ViewProjection matrix , firstly get Projection matrix and then multiply it with View Matrix
        glm::mat4 ViewProjection = camera->getProjectionMatrix(this->windowSize) * camera->getViewMatrix();
        // TODO: (Req 9) Set the OpenGL viewport using viewportStart and viewportSize
        //  starts from point (0 , 0) and give it width and height of the screen
        glViewport(0, 0, this->windowSize.x, this->windowSize.y);
        // TODO: (Req 9) Set the clear color to black and the clear depth to 1
        //  these two lines are used for clearing configuration not clean itself
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClearDepth(1.0f);
        // TODO: (Req 9) Set the color mask to true and the depth mask to true (to ensure the glClear will affect the framebuffer)
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glDepthMask(GL_TRUE);

        // If there is a postprocess material, bind the framebuffer
        if (postprocessMaterial)
        {
            // TODO: (Req 11) bind the framebuffer
            //  here just i want to bind postprocess framebuffer
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postprocessFrameBuffer);
        }

        // TODO: (Req 9) Clear the color and depth buffers
        //  this line do the cleaning actually after choosing the line and depth clear above
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // TODO: (Req 9) Draw all the opaque commands
        //  Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command
        for (auto opaqueCommand : opaqueCommands)
        {
            opaqueCommand.material->setup();
            // TODO: (Phase 2) Add lights logic
            if (dynamic_cast<our::LightMaterial *>(opaqueCommand.material))
            {
                int index = 0;
                for (auto it = point_directional_lights.begin(); it != point_directional_lights.end(); it++, index++)
                {
                    opaqueCommand.material->shader->set("lights[" + std::to_string(index) + "].type", (*it)->light_type);
                    if ((*it)->light_type == 0)
                    {
                        opaqueCommand.material->shader->set("lights[" + std::to_string(index) + "].direction",
                                                            (*it)->direction);
                        opaqueCommand.material->shader->set("lights[" + std::to_string(index) + "].color",
                                                            (*it)->color);
                    }
                    else
                    {
                        glm::mat4 m = point_directional_lights[index]->getOwner()->getLocalToWorldMatrix();
                        glm::mat4 mVP = ViewProjection * m;
                        auto lightPosition =
                            glm::vec3(m *
                                      glm::vec4((point_directional_lights[index])->getOwner()->localTransform.position, 1.0));

                        opaqueCommand.material->shader->set("lights[" + std::to_string(index) + "].position",
                                                            lightPosition);
                        opaqueCommand.material->shader->set("lights[" + std::to_string(index) + "].color",
                                                            (*it)->color);
                        opaqueCommand.material->shader->set("lights[" + std::to_string(index) + "].attenuation",
                                                            (*it)->attenuation);
                    }
                }
                int index2 = 0;
                // Loop through the street_lights to send light data to the shader (limited by SPOT_NUM)
                for (auto it = spot_light.begin(); index2 < SPOT_NUM; it++, index++, index2++)
                {
                    // Set the light type for the current index
                    opaqueCommand.material->shader->set("lights[" + std::to_string(index) + "].type", (*it)->light_type);

                    // spot light
                    glm::mat4 m = spot_light[index2]->getOwner()->getLocalToWorldMatrix();
                    glm::mat4 mVP = m;
                    // for testing
                    auto lightPosition =
                        glm::vec3(
                            glm::vec4((spot_light[index2])->getOwner()->localTransform.position, 1.0));
                    lightPosition.y += 3.0; // Adjusting the position to simulate the upper part of the street light

                    // Set position, direction, color, attenuation, and cone angles for the spot light
                    opaqueCommand.material->shader->set("lights[" + std::to_string(index) + "].position",
                                                        lightPosition);
                    opaqueCommand.material->shader->set("lights[" + std::to_string(index) + "].direction",
                                                        (*it)->direction);
                    opaqueCommand.material->shader->set("lights[" + std::to_string(index) + "].color",
                                                        (*it)->color);
                    opaqueCommand.material->shader->set("lights[" + std::to_string(index) + "].attenuation",
                                                        (*it)->attenuation);
                    opaqueCommand.material->shader->set("lights[" + std::to_string(index) + "].cone_angles",
                                                        (*it)->cone_angles);
                }
                opaqueCommand.material->shader->set("light_count", (int32_t)point_directional_lights.size() + SPOT_NUM);
                opaqueCommand.material->shader->set("sky.top", glm::vec3(0.1, 0.5, 0.1));
                opaqueCommand.material->shader->set("sky.bottom", glm::vec3(0.1, 0.5, 0.1));
                opaqueCommand.material->shader->set("sky.horizon", glm::vec3(0.1, 0.5, 0.1));

                opaqueCommand.material->shader->set("VP", ViewProjection);
                opaqueCommand.material->shader->set("M", opaqueCommand.localToWorld);
                opaqueCommand.material->shader->set("M_IT", glm::transpose(glm::inverse(opaqueCommand.localToWorld)));
                opaqueCommand.material->shader->set("camera_position", positionOfCamera);
            }
            else
            {
                glm::mat4 modelMatrix = opaqueCommand.localToWorld;
                glm::mat4 ModelViewProjection = ViewProjection * modelMatrix;
                opaqueCommand.material->shader->set("transform", ModelViewProjection);
            }
            opaqueCommand.mesh->draw();
        }
        // If there is a sky material, draw the sky
        if (this->skyMaterial)
        {
            // TODO: (Req 10) setup the sky material
            //  setup the sky material
            this->skyMaterial->setup();
            // TODO: (Req 10) Get the camera position
            glm::vec3 cameraPosition = positionOfCamera;
            // TODO: (Req 10) Create a model matrix for the sy such that it always follows the camera (sky sphere center = camera position)
            our::Transform skyTransformObj;
            skyTransformObj.position = cameraPosition;
            glm::mat4 skySphereModel = skyTransformObj.toMat4();
            // TODO: (Req 10) We want the sky to be drawn behind everything (in NDC space, z=1)
            //  We can acheive the is by multiplying by an extra matrix after the projection but what values should we put in it?
            //  it should be the following matrix
            glm::mat4 alwaysBehindTransform = glm::mat4(
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 1.0f);
            // TODO: (Req 10) set the "transform" uniform CHECK HEREEEEEEE
            this->skyMaterial->shader->set("transform", alwaysBehindTransform * ViewProjection * skySphereModel);
            // TODO: (Req 10) draw the sky sphere
            //  Draw the sphere of the sky
            this->skySphere->draw();
        }
        // TODO: (Req 9) Draw all the transparent commands
        //  Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command
        for (auto transparentCommand : transparentCommands)
        {
            // TODO: (Phase 2) Add lights logic
            transparentCommand.material->setup();
            if (dynamic_cast<our::LightMaterial *>(transparentCommand.material))
            {
                int index = 0;
                for (auto it = point_directional_lights.begin(); it != point_directional_lights.end(); it++, index++)
                {
                    transparentCommand.material->shader->set("lights[" + std::to_string(index) + "].type",
                                                             (*it)->light_type);
                    if ((*it)->light_type == 0)
                    {
                        transparentCommand.material->shader->set("lights[" + std::to_string(index) + "].direction",
                                                                 (*it)->direction);
                        transparentCommand.material->shader->set("lights[" + std::to_string(index) + "].color",
                                                                 (*it)->color);
                    }
                    else
                    {
                        transparentCommand.material->shader->set("lights[" + std::to_string(index) + "].position",
                                                                 (*it)->getOwner()->localTransform.position);
                        transparentCommand.material->shader->set("lights[" + std::to_string(index) + "].color",
                                                                 (*it)->color);
                        transparentCommand.material->shader->set("lights[" + std::to_string(index) + "].attenuation",
                                                                 (*it)->attenuation);
                    }
                }
                int index2 = 0;
                for (auto it = spot_light.begin(); index2 < SPOT_NUM; it++, index++, index2++)
                {
                    transparentCommand.material->shader->set("lights[" + std::to_string(index) + "].type", (*it)->light_type);
                    glm::mat4 m = spot_light[index2]->getOwner()->getLocalToWorldMatrix();
                    glm::mat4 mVP = m;
                    auto lightPosition =
                        glm::vec3(
                            m * glm::vec4((spot_light[index2])->getOwner()->localTransform.position, 1.0));
                    lightPosition.y += 3.0; // to simulate the upper part of the streat light not the base part
                    transparentCommand.material->shader->set("lights[" + std::to_string(index) + "].position",
                                                             lightPosition);
                    transparentCommand.material->shader->set("lights[" + std::to_string(index) + "].direction",
                                                             (*it)->direction);
                    transparentCommand.material->shader->set("lights[" + std::to_string(index) + "].color",
                                                             (*it)->color);
                    transparentCommand.material->shader->set("lights[" + std::to_string(index) + "].attenuation",
                                                             (*it)->attenuation);
                    transparentCommand.material->shader->set("lights[" + std::to_string(index) + "].cone_angles",
                                                             (*it)->cone_angles);
                }
                // std::cout << "num of lightsis : " << (int32_t)lights_list.size() << std::endl;
                transparentCommand.material->shader->set("light_count", (int32_t)point_directional_lights.size());
                transparentCommand.material->shader->set("sky.top", glm::vec3(0.1, 0.5, 0.1));
                transparentCommand.material->shader->set("sky.bottom", glm::vec3(0.1, 0.5, 0.1));
                transparentCommand.material->shader->set("sky.horizon", glm::vec3(0.1, 0.5, 0.1));

                transparentCommand.material->shader->set("VP", ViewProjection);
                transparentCommand.material->shader->set("M", transparentCommand.localToWorld);
                transparentCommand.material->shader->set("M_IT",
                                                         glm::transpose(glm::inverse(transparentCommand.localToWorld)));
                transparentCommand.material->shader->set("camera_position", positionOfCamera); // eye * Model of camera
            }
            else
            {
                glm::mat4 ModelMatrix = transparentCommand.localToWorld;
                glm::mat4 ModelViewProjection = ViewProjection * ModelMatrix;
                transparentCommand.material->shader->set("transform", ModelViewProjection);
            }
            transparentCommand.mesh->draw();
        }

        // If there is a postprocess material, apply postprocessing
        if (postprocessMaterial)
        {
            // Create the post processing shader
            if (lastPostProcess != postProcessFilter)
            {
                // Create a sampler to use for sampling the scene texture in the post processing shader
                Sampler *postprocessSampler = new Sampler();
                postprocessSampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                postprocessSampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                postprocessSampler->set(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                postprocessSampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                // Create the post processing shader
                ShaderProgram *postprocessShader = new ShaderProgram();
                postprocessShader->attach("assets/shaders/fullscreen.vert", GL_VERTEX_SHADER);
                postprocessShader->attach(postProcessFilter, GL_FRAGMENT_SHADER);
                postprocessShader->link();

                // Create a post processing material
                postprocessMaterial = new TexturedMaterial();
                postprocessMaterial->shader = postprocessShader;
                postprocessMaterial->texture = colorTarget;
                postprocessMaterial->sampler = postprocessSampler;
                // The default options are fine but we don't need to interact with the depth buffer
                postprocessMaterial->pipelineState.depthMask = false;
                lastPostProcess = postProcessFilter;
            }
            
            // TODO: (Req 11) Return to the default framebuffer
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glBindVertexArray(postProcessVertexArray);
            // TODO: (Req 11) Setup the postprocess material and draw the fullscreen triangle
            postprocessMaterial->setup();
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
    }
}
