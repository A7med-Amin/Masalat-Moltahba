#pragma once
#include "../ecs/component.hpp"
#include <glm/gtx/euler_angles.hpp>
#include <glm/glm.hpp>
#include <json/json.hpp>
namespace our
{
    class LightComponent : public Component
    {
    public:
        int light_type; // this member variable will be used to specify type of light(Spot_light , Point_light , Direction_light)
        glm::vec3 direction; // Direction of light
        glm::vec3 position; // Position of light source
        glm::vec3 color; // Color of light
        glm::vec3 attenuation; // Attenuation of light
        glm::vec3 cone_angles; // Cones_angles of spot light

        // The ID of this component type is "Light"
        static std::string getID() { return "Light"; }

        // Reads light component data from the given JSON object
        void deserialize(const nlohmann::json &data) override;
    };
}