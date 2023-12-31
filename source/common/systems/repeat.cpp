//TODO: (Phase 2)

#include <iostream>
#include "repeat.hpp"

#pragma once

#include "../components/player.hpp"
#include "../components/masala.hpp"
#include "../components/repeat.hpp"
#include "../components/gem-heart.hpp"

#include <glm/glm.hpp>
using namespace std;

/*
    This component is responsible for repeating objects to continue with us
    within the screen by moving it forward in the screen by applying translation
*/

 int generateRandomNumber(int min, int max)
    {
        // Generate a random number between min and max (inclusive)
        int randomNumber = rand() % (max - min + 1) + min;
        return randomNumber;
    }
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

        RepeatComponent *repeatComponent;
        Entity *repeatEntity;
        for (auto entity: world->getEntities()) {
            repeatEntity = entity;
            repeatComponent = repeatEntity->getComponent<RepeatComponent>();
            glm::vec3 &repeatPosition = repeatEntity->localTransform.position;
            int randomX;
            if (repeatComponent) {
                randomX = repeatComponent->random;
                if (playerPosition[2] <= repeatPosition[2] ) {
                    MasalaComponent *masalaComponent = repeatEntity->getComponent<MasalaComponent>();
                    GemHeartComponent *gemHeartComponent = repeatEntity->getComponent<GemHeartComponent>();

                    if (masalaComponent) {
                        if ((repeatPosition + repeatComponent->translation).z < -495) {
                            world->markForRemoval(repeatEntity);
                            continue;
                        }
                    }

                     if (gemHeartComponent) {
                        if ((repeatPosition + repeatComponent->translation).z < -450) {
                            world->markForRemoval(repeatEntity);
                            continue;
                        }
                    }

                    if(randomX){
                        repeatPosition.x += generateRandomNumber(-5, 5);
                            if(repeatPosition.x < -8){
                        repeatPosition.x = -8;
                    }
                    if(repeatPosition.x > 8){
                        repeatPosition.x = 8;
                    }
                    }

                

                    repeatPosition += repeatComponent->translation;
                    if(repeatPosition.z < -490){
                        repeatPosition.z = -490;
                    }
                }
            }
        }
        // Delete the entities that are marked for removal
        world->deleteMarkedEntities();
    }
}