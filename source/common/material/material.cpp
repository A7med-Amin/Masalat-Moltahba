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
    
    // DONE TODO: (Phase 2) add LightingMaterial::setup() 
    void LightMaterial::setup() const{
        // This function will be used for any setup at first
        TintedMaterial::setup();
        // Activate Texture unit 0 to be used in next operations
        glActiveTexture(GL_TEXTURE0);
        // albedo texture to the currently active texture unit (texture unit 0 in this case). Binding a texture means making it the current texture that will be used in subsequent rendering operations.
        albedo->bind();
        // set a shader uniform "material.albedo" to texture unit 0
        shader->set("material.albedo", 0);
        // bind the sampler to texture unit 0
        sampler->bind(0);

        // Repeat previoud lines for the rest of textures
        glActiveTexture(GL_TEXTURE1);
        specular->bind();
        shader->set("material.specular", 1);
        sampler->bind(1);

        glActiveTexture(GL_TEXTURE2);
        emissive->bind();
        shader->set("material.emissive", 2);
        sampler->bind(2);

        glActiveTexture(GL_TEXTURE3);
        roughness->bind();
        shader->set("material.roughness", 3);
        sampler->bind(3);

        glActiveTexture(GL_TEXTURE4);
        ambient_occlusion->bind();
        shader->set("material.ambient_occlusion", 4);
        sampler->bind(4);
    }

    void LightMaterial::deserialize(const nlohmann:: json& data){
        // Call deserialize function for the base class
        TintedMaterial::deserialize(data);
        // Check if the data does not object
        if(!data.is_object()) return;
        // load "albedo" value from Json object and assign the value to class member value -> albedo //
        albedo = AssetLoader<Texture2D>::get(data.value("albedo" , "red"));
        // load "emissive" value from Json object and assign the value to class member value -> emmissive //
        emissive = AssetLoader<Texture2D>::get(data.value("emissive" , "red"));
        // load "specular" value from Json object and assign the value to class member value -> specular //
        specular = AssetLoader<Texture2D>::get(data.value("specular" , "red"));
        // load "ambient_occlusion" value from Json object and assign the value to class member value -> ambient_occlusion //
        ambient_occlusion = AssetLoader<Texture2D>::get(data.value("ambient_occlusion" , "red"));
        // load "roughness" value from Json object and assign the value to class member value -> roughness //
        roughness = AssetLoader<Texture2D>::get(data.value("roughness" , "red"));
        // load "sampler" value from Json object and assign the value to class member value -> sampler //
        sampler = AssetLoader<Sampler>::get(data.value("sampler" , "red"));
    }
}