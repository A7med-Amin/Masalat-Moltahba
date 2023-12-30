//TODO: (Phase 2)

#include <iostream>
#include "repeat.hpp"

#pragma once

#include "../components/player.hpp"
#include "../components/masala.hpp"
#include "../components/repeat.hpp"

#include <glm/glm.hpp>
using namespace std;

/*
    This component is responsible for repeating objects to continue with us
    within the screen by moving it forward in the screen by applying translation
*/

namespace our {
    void RepeatSystem::update(World *world, float deltaTime) {
        glm::vec3 playerPosition;
        PlayerComponent *player;
        for (auto entity: world->getEntities()) {
            // Get the player component if it exists
            player = entity->getComponent<PlayerComponent>();
            // If the player component exists
            if (player) {
                // Get the player's position in the world
                playerPosition =
                        glm::vec3(entity->getLocalToWorldMatrix() *
                                glm::vec4(entity->localTransform.position, 1.0));
                break;
            }
        }

        // If the player component doesn't exist, return
        if (!player) {
            return;
        }

        // Repeat the entities
        RepeatComponent *repeatComponent;
        Entity *repeatEntity;
        for (auto entity: world->getEntities()) {
            // Get Reepeat component 
            repeatEntity = entity;
            repeatComponent = repeatEntity->getComponent<RepeatComponent>();
            // Get repeat componenet position in the world
            glm::vec3 &repeatPosition = repeatEntity->localTransform.position;
            // If the repeat component exists
            if (repeatComponent) {
                // print the position of the player and the repeat component
                cout << "repeat component for entity: " << " " << entity->name << " " << repeatPosition.x << " " << repeatPosition.y << " " << repeatPosition.z << endl;
                if (playerPosition[2] <= repeatPosition[2] - 5) {
                    // Repear masala component only
                    MasalaComponent *masalaComponent = repeatEntity->getComponent<MasalaComponent>();
                    // Prevent the repeating after the end of the level
                    if (masalaComponent) {
                        cout << "repeat masala component for entity: " << " " << entity->name << " " << repeatPosition.x << " " << repeatPosition.y << " " << repeatPosition.z << endl;
                        if ((repeatPosition + repeatComponent->translation).x < -1995) {
                            world->markForRemoval(repeatEntity);
                            continue;
                        }
                    }
                    // Repeat the entity (translate it by the translation vector)
                    // Move it forward to make it still appearing within the screen
                    repeatPosition += repeatComponent->translation;
                }
            }
        }
        // Delete the entities that are marked for removal
        world->deleteMarkedEntities();
    }
}