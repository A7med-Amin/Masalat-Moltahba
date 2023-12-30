#include "final-line.hpp"
#include "../ecs/entity.hpp"
#include "../deserialize-utils.hpp"

namespace our {
    void FinalLineComponent::deserialize(const nlohmann::json &data) {
        if (!data.is_object()) return;
    }
}