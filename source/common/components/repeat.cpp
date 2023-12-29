#include "repeat.hpp"
#include "../ecs/entity.hpp"
#include "../deserialize-utils.hpp"

namespace our {
    // Translate the object by move it forward within the screen
    void RepeatComponent::deserialize(const nlohmann::json &data) {
        if (!data.is_object()) return;
        translation = data.value("translation", translation);
    }
}