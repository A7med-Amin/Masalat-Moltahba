#pragma once

#include "../ecs/component.hpp"

#include <glm/glm.hpp>

namespace our {
    class MasalaComponent : public Component {
    public:
        static std::string getID() { return "Masala"; }

        void deserialize(const nlohmann::json &data) override;
    };
}