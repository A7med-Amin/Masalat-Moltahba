#pragma once

#include "../ecs/entity.hpp"
#include "camera.hpp"
#include "mesh-renderer.hpp"
#include "free-camera-controller.hpp"
#include "movement.hpp"
#include "light.hpp"
#include "collision.hpp"
#include "masala.hpp"
#include "heart.hpp"
#include "gem-heart.hpp"
#include "player.hpp"
#include "repeat.hpp"
#include "final-line.hpp"

namespace our {

    // Given a json object, this function picks and creates a component in the given entity
    // based on the "type" specified in the json object which is later deserialized from the rest of the json object
    inline void deserializeComponent(const nlohmann::json& data, Entity* entity){
        std::string type = data.value("type", "");
        Component* component = nullptr;
        //TODO: (Req 8) Add an option to deserialize a "MeshRendererComponent" to the following if-else statement
        if(type == CameraComponent::getID()){
            component = entity->addComponent<CameraComponent>();
        } else if (type == FreeCameraControllerComponent::getID()) {
            component = entity->addComponent<FreeCameraControllerComponent>();
        } else if (type == MovementComponent::getID()) {
            component = entity->addComponent<MovementComponent>();
        }
        else if(type == MeshRendererComponent::getID())
        {
            component = entity->addComponent<MeshRendererComponent>();
        }

        // TODO: (Phase 2) Add new components conditions here
        else if(type == LightComponent::getID())
        {
            component = entity->addComponent<LightComponent>();
        }

        else if(type == CollisionComponent::getID())
        {
            component = entity->addComponent<CollisionComponent>();
        }

        else if(type == MasalaComponent::getID())
        {
            component = entity->addComponent<MasalaComponent>();
        }

        else if(type == HeartComponent::getID())
        {
            component = entity->addComponent<HeartComponent>();
        }

        else if(type == GemHeartComponent::getID())
        {
            component = entity->addComponent<GemHeartComponent>();
        }

        else if(type == PlayerComponent::getID())
        {
            component = entity->addComponent<PlayerComponent>();
        }

        else if(type == RepeatComponent::getID())
        {
            component = entity->addComponent<RepeatComponent>();
        }

        else if(type == FinalLineComponent::getID())
        {
            component = entity->addComponent<FinalLineComponent>();
        }

        if(component) component->deserialize(data);
    }

}