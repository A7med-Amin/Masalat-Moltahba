#pragma once

#include "../ecs/component.hpp"

#include <glm/glm.hpp>

namespace our {
    class FinalLineComponent : public Component {
    public:

        static std::string getID() { return "FinalLine"; }

        void deserialize(const nlohmann::json &data) override;
    };

}