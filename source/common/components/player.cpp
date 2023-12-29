#include "player.hpp"
#include "../ecs/entity.hpp"
#include "../deserialize-utils.hpp"

namespace our {
    // Moves the player by given speed from json
    void PlayerComponent::deserialize(const nlohmann::json &data) {
        if (!data.is_object()) return;
        this->speed = data.value("speed", this->speed);
    }
}