#pragma once

#include "../ecs/component.hpp"

#include <glm/glm.hpp>

namespace our {
    class PlayerComponent : public Component {
    public:
        glm::float32 speed = 0; // Each frame, the entity should move as follows: position += linearVelocity * deltaTime

        static std::string getID() { return "Player"; }

        void deserialize(const nlohmann::json& data) override;
    };

}