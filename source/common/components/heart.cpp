#include "heart.hpp"
#include "../ecs/entity.hpp"
#include "../deserialize-utils.hpp"

namespace our {
    // Read number of hearts to be rendered
    void HeartComponent::deserialize(const nlohmann::json &data) {
        if (!data.is_object()) return;
        heartNumber = data.value("number", heartNumber);
    }
}