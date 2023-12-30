#pragma once

#include "../ecs/component.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

namespace our
{
    // An enum that defines the type of the light (directonal , point or spot light)
    enum class LightType
    {
        DIRECTIONAL_LIGHT,
        POINT_LIGHT,
        SPOT_LIGHT
    };

    class LightComponent : public Component
    {
    public:
        LightType LightType;
        glm::vec3 color;
        glm::vec2 coneAngles;
        glm::vec3 attenuation;
        static std::string getID() { return "Light"; }
        void deserialize(const nlohmann::json &data) override;
        glm::vec3 getLightColor();
    };

}