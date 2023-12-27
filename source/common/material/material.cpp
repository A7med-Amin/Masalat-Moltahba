// #include "material.hpp"

// #include "../asset-loader.hpp"
// #include "deserialize-utils.hpp"

// namespace our {

//     // This function should setup the pipeline state and set the shader to be used
//     void Material::setup() const {
//         //TODO: (Req 7) Write this function
//         pipelineState.setup();
//         shader->use();
//     }

//     // This function read the material data from a json object
//     void Material::deserialize(const nlohmann::json& data){
//         if(!data.is_object()) return;

//         if(data.contains("pipelineState")){
//             pipelineState.deserialize(data["pipelineState"]);
//         }
//         shader = AssetLoader<ShaderProgram>::get(data["shader"].get<std::string>());
//         transparent = data.value("transparent", false);
//     }

//     // This function should call the setup of its parent and
//     // set the "tint" uniform to the value in the member variable tint 
//     void TintedMaterial::setup() const {
//         //TODO: (Req 7) Write this function
//         Material::setup();
//         // GLint tintloc = shader->getUniformLocation("tint");
//         shader->set("tint", tint);
//         // if (tintloc != -1)
//         // {
//         //     glUniform4fv(tintloc, 1, glm::value_ptr(this->tint));
//         // }
//         // else
//         // {
//         //     fprintf(stderr, "OH MY GOD!");
//         // }
//     }

//     // This function read the material data from a json object
//     void TintedMaterial::deserialize(const nlohmann::json& data){
//         Material::deserialize(data);
//         if(!data.is_object()) return;
//         tint = data.value("tint", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
//     }

//     // This function should call the setup of its parent and
//     // set the "alphaThreshold" uniform to the value in the member variable alphaThreshold
//     // Then it should bind the texture and sampler to a texture unit and send the unit number to the uniform variable "tex" 
//     void TexturedMaterial::setup() const {
//         //TODO: (Req 7) Write this function
//         TintedMaterial::setup();
//         // GLint alphaloc = shader->getUniformLocation("alphaThreshold");

//         shader->set("alphaThreshold", alphaThreshold);
//         // glActiveTexture(GL_TEXTURE0);
//         texture->bind();
//         GLuint textureID = texture->getOpenGLName();
//         sampler->bind(textureID);
//         shader->set("tex", 0);

//         // if (alphaloc != -1)
//         // {
//         //     glUniform1f(alphaloc, this->alphaThreshold);
//         // }
//         // else
//         // {
//         //     fprintf(stderr, "OH MY GOD!");
//         // }

//         // // Setting the sampler
//         // GLint tex = shader->getUniformLocation("tex");
//         // glActiveTexture(GL_TEXTURE0);
//         // texture->bind();
//         // // GLuint textID = texture->getOpenGLName();
//         // sampler->bind(0);
//         // if (tex != -1)
//         // {
//         //     glUniform1f(tex, 0);
//         // }
//         // else
//         // {
//         //     fprintf(stderr, "OH MY GOD!");
//         // }
//     }

//     // This function read the material data from a json object
//     void TexturedMaterial::deserialize(const nlohmann::json& data){
//         TintedMaterial::deserialize(data);
//         if(!data.is_object()) return;
//         alphaThreshold = data.value("alphaThreshold", 0.0f);
//         texture = AssetLoader<Texture2D>::get(data.value("texture", ""));
//         sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));
//         int x;
//     }

// }

#include "material.hpp"

#include "../asset-loader.hpp"
#include "deserialize-utils.hpp"
#include <iostream>

namespace our
{

    // This function should setup the pipeline state and set the shader to be used
    void Material::setup() const
    {
        // DONE#TODO: Write this function
        pipelineState.setup();
        shader->use();
    }

    // This function read the material data from a json object
    void Material::deserialize(const nlohmann::json &data)
    {
        if (!data.is_object())
            return;

        if (data.contains("pipelineState"))
        {
            pipelineState.deserialize(data["pipelineState"]);
        }
        shader = AssetLoader<ShaderProgram>::get(data["shader"].get<std::string>());
        transparent = data.value("transparent", false);
    }

    // This function should call the setup of its parent and
    // set the "tint" uniform to the value in the member variable tint
    void TintedMaterial::setup() const
    {
        // DONE#TODO: Write this function
        Material::setup();
        shader->set("tint", tint);
    }

    // This function read the material data from a json object
    void TintedMaterial::deserialize(const nlohmann::json &data)
    {
        Material::deserialize(data);
        if (!data.is_object())
            return;
        tint = data.value("tint", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    // This function should call the setup of its parent and
    // set the "alphaThreshold" uniform to the value in the member variable alphaThreshold
    // Then it should bind the texture and sampler to a texture unit and send the unit number to the uniform variable "tex"
    void TexturedMaterial::setup() const
    {
        // DONE#TODO: Write this function
        TintedMaterial::setup();
        shader->set("alphaThreshold", alphaThreshold);
        // First, we set the active texture unit. 
        // Here, we pick unit 0 which is actually the active unit by default but we still wrote this line for demonstration.
        glActiveTexture(GL_TEXTURE0);
        // When we bind the texture, we also bind it to the active unit. So this texture is now bound to unit 0.
        texture->bind();
        // To tell OpenGL which sampler object we will use for this unit,
        // we bind the sampler to unit 0 (which is specified by the 1st parameter of the following function).
        if(sampler != nullptr)
            sampler->bind(0);
        // send the unit number to the uniform variable "tex"
        shader->set("tex", 0);
    }

    // This function read the material data from a json object
    void TexturedMaterial::deserialize(const nlohmann::json &data)
    {
        TintedMaterial::deserialize(data);
        if (!data.is_object())
            return;
        alphaThreshold = data.value("alphaThreshold", 0.0f);
        texture = AssetLoader<Texture2D>::get(data.value("texture", ""));
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));
    }
    // This function should call the setup of its parent and
    // set the "alphaThreshold" uniform to the value in the member variable alphaThreshold
    // Then it should bind each texture and sampler to a texture unit and send the unit number to 
    // corresponding uniform variable e.g"Material.albedo"
    

    // TODO: (Phase 2) add LightingMaterial::setup() 
    
    // TODO: (Phase 2) add LightingMaterial::deserialize()

}