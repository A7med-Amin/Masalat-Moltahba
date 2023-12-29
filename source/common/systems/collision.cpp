//TODO: (Phase 2)


#include "collision.hpp"

#pragma once

#include "../ecs/world.hpp"
#include "../components/collision.hpp"
#include "../components/player.hpp"
#include "../components/masala.hpp"
#include "../components/repeat.hpp"
#include "../components/heart.hpp"
#include "../components/gem-heart.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <iostream>

#include "../mesh/mesh-utils.hpp"
#include "../texture/texture-utils.hpp"

#include "../application.hpp"


namespace our {
    void CollisionSystem::update(World *world, float deltaTime, int &heartCount,
                                float &collisionStartTime) {
        PlayerComponent *player; // The player component if it exists
        glm::vec3 playerPosition; // The player's position in the world
        Entity *playerEntity;   // The player entity if it exists

        for (auto entity: world->getEntities()) {   // search for the player entity
            // Get the player component if it exists
            playerEntity = entity;
            player = playerEntity->getComponent<PlayerComponent>();
            // If the player component exists
            if (player) {
                playerPosition =
                        glm::vec3(playerEntity->getLocalToWorldMatrix() *
                                  glm::vec4(playerEntity->localTransform.position, 1.0)); // get the player's position in the world
                break;
            }
        }
        if (!player) {
            return; // If the player doesn't exist, we can't do collision detection
        }

        // Get player collision bounding box
        glm::vec3 playerStart = playerEntity->getComponent<CollisionComponent>()->start + playerPosition;   // get the player's start position
        glm::vec3 playerEnd = playerEntity->getComponent<CollisionComponent>()->end + playerPosition;   // get the player's end position

        // For each entity in the world
        for (auto entity: world->getEntities()) {
            // Get the collision component if it exists
            CollisionComponent *collision = entity->getComponent<CollisionComponent>();
            // If the collision component exists
            if (collision) {
                auto objectPosition = entity->localTransform.position; // get the object's position in the world
                // Get object collision bounding box
                glm::vec3 objectStart = collision->start + objectPosition;  // get the object's start position
                glm::vec3 objectEnd = collision->end + objectPosition; // get the object's end position
                bool collided = true;
                // loop for each axis (x,y,z)
                for (int i = 0; i < 3; ++i) {
                    if (playerStart[i] > objectEnd[i] || playerEnd[i] < objectStart[i]) { // if the player and object don't overlap on this axis
                        collided = false; // then they don't collide
                        break;
                    }
                }
                if (collided) {
                    // Player hits an obstacle
                    if (entity->getComponent<MasalaComponent>()) { // if the object is an obstacle
                        if (collisionStartTime == 0)
                            collisionStartTime = deltaTime; // start counting the time of collision for postprocessing effect

                        CollisionSystem::decreaseHearts(world, heartCount);

                        if (heartCount < 1) { // if the player has no more hearts

                            app->changeState("game-over"); // go to the game over state
                        }
                    } 
                    // Player takes a heart
                    else if(entity->getComponent<GemHeartComponent>()) // if the object is a gem heart
                    {
                        if(heartCount < 3) // if the player has less than 3 hearts which is max
                        {
                            heartCount++; // increase the count of hearts
                        }

                        // Make heart to disappear
                        entity->localTransform.scale = glm::vec3(0.0f, 0.0f, 0.0f);
                        entity->localTransform.position = glm::vec3(0.0f, 0.0f, 0.0f); 

                        for (auto heartEntity: world->getEntities()) {  // search for the heart entity
                            HeartComponent *heart = heartEntity->getComponent<HeartComponent>(); // get the heart component if it exists
                            if (heart && heart->heartNumber == heartCount) { // if the heart component exists and it's the heart that we want to increase
                                heartEntity->localTransform.scale.x = 0.0009f; // make the heart appear
                                heartEntity->localTransform.scale.y = 0.0009f; // make the heart appear
                                heartEntity->localTransform.scale.z = 0.0009f; // make the heart appear
                                break;
                            }
                        }
                    }

                    RepeatComponent *repeatComponent = entity->getComponent<RepeatComponent>();
                    glm::vec3 &repeatPosition = entity->localTransform.position;
                    if (repeatComponent) { // if the object is a repeat object
                        repeatPosition += repeatComponent->translation; // move the object forward
                    }
                    break;
                }
            }
        }
    }

    // decrease the hearts
    void CollisionSystem::decreaseHearts(World *world, int &heartCount) { 

        for (auto heartEntity: world->getEntities()) {  // search for the heart entity
            HeartComponent *heart = heartEntity->getComponent<HeartComponent>();
            if (heart && heart->heartNumber == heartCount) { // if the heart component exists and it's the heart that we want to decrease
                // make the heart disappear
                heartEntity->localTransform.scale.x = 0.0f;  
                heartEntity->localTransform.scale.y = 0.0f;  
                heartEntity->localTransform.scale.z = 0.0f;  
                heartCount--; // decrease the count of hearts
                break;
            }
        }
    }
}