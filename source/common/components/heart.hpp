#pragma once

#include "../ecs/component.hpp"

#include <glm/glm.hpp>

namespace our {
    /*
        Heart to be appearing at the top of the screen
    */
    class HeartComponent : public Component {
    public:
        int heartNumber = 0;
        // The ID of this component type is "Movement"
        static std::string getID() { return "Heart"; }

        // Reads linearVelocity & angularVelocity from the given json object
        void deserialize(const nlohmann::json &data) override;
    };

}