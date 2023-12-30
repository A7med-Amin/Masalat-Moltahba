#pragma once

#include "../ecs/component.hpp"

#include <glm/glm.hpp>

namespace our {

/*
    Heart within the game to takes to give you more life
*/    
class GemHeartComponent : public Component {
    public:
        static std::string getID() { return "GemHeart"; }
        void deserialize(const nlohmann::json &data) override;
    };

}