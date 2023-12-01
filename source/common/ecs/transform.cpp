#include "entity.hpp"
#include "../deserialize-utils.hpp"

#include <glm/gtx/euler_angles.hpp>

namespace our {

    // This function computes and returns a matrix that represents this transform
    // Remember that the order of transformations is: Scaling, Rotation then Translation
    // HINT: to convert euler angles to a rotation matrix, you can use glm::yawPitchRoll
    glm::mat4 Transform::toMat4() const {
        //TODO: (Req 3) Write this function
        // Create transformation matrix with the order: Scaling, Rotation, Translation
        glm::mat4 matrix = glm::mat4(1.0f); // Identity matrix

        // Apply scaling
        glm::mat4 scale_matrix = glm::scale(matrix, scale);

        // Apply rotation (convert euler angles to rotation matrix)
        glm::mat4 rotation_matrix = glm::yawPitchRoll(rotation.y, rotation.x, rotation.z);

        // Apply translation
        glm::mat4 translation_matrix = glm::translate(matrix, position);
        return translation_matrix * rotation_matrix * scale_matrix; 
    }

     // Deserializes the entity data and components from a json object
    void Transform::deserialize(const nlohmann::json& data){
        position = data.value("position", position);
        rotation = glm::radians(data.value("rotation", glm::degrees(rotation)));
        scale    = data.value("scale", scale);
    }

}