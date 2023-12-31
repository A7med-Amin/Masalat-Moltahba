#pragma once

#include "../ecs/component.hpp"

#include <glm/glm.hpp>

namespace our {

    /*
    This component is responsible for repeating objects to continue with us
    within the screen by moving it forward in the screen by applying translation
    */
    class RepeatComponent : public Component {
    public:
        glm::vec3 translation = {0, 0, 0};
        bool random = false;

        static std::string getID() { return "Repeat"; }

        void deserialize(const nlohmann::json &data) override;
    };

}