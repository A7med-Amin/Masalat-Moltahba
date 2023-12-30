#include "light.hpp"
#include "../ecs/entity.hpp"
#include "../deserialize-utils.hpp"

// DONE TODO: (Phase 2) Create the `LightComponent`

namespace our
{
    void LightComponent::deserialize(const nlohmann::json &data)
    {
        // Check if the data isn't object -> return
        if (!data.is_object())
            return;

        // load 'light_type' from json object and put it into light_type property
        light_type = data.value("light_type", light_type);

        // load 'position' from json object and put it into position property
        position = data.value("position", position);

        // load 'direction' from json object and put it into direction property
        direction = data.value("direction", direction);

        // load 'color' from json object and put it into color property
        color = data.value("color", color);

        // load 'attenuation' from json object and put it into attenuation property
        attenuation = data.value("attenuation", attenuation);

        // load 'cone_angles' from json object and put it into cone_angles property
        cone_angles = data.value("cone_angles", cone_angles);
    }
}