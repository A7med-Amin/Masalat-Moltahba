// TODO: (Phase 2)

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

namespace our
{
    struct BoundingBox
    {
        float minX, maxX;
        float minY, maxY;
        float minZ, maxZ;
    };

    float absolute(float value)
    {
        return value >= 0 ? value : -value;
    }
    bool checkCollision(const BoundingBox &box1, const BoundingBox &box2)
    {
        return (box1.minX < box2.maxX && box1.maxX > box2.minX) &&
               (box1.minY < box2.maxY && box1.maxY > box2.minY) &&
               (box1.minZ - 3 < box2.maxZ && box1.maxZ - 3 > box2.minZ);
    }

    // Example usage

    bool CollisionSystem::update(World *world, float deltaTime, int &heartCount,
                                 float &collisionStartTime)
    {
        PlayerComponent *player;  // The player component if it exists
        glm::vec3 playerPosition; // The player's position in the world
        Entity *playerEntity;     // The player entity if it exists
        bool collided = false;

        for (auto entity : world->getEntities())
        { // search for the player entity
            // Get the player component if it exists
            playerEntity = entity;
            player = playerEntity->getComponent<PlayerComponent>();
            // If the player component exists
            if (player)
            {
                playerPosition =
                    glm::vec3(playerEntity->getLocalToWorldMatrix() *
                              glm::vec4(playerEntity->localTransform.position, 1.0)); // get the player's position in the world
                break;
            }
        }
        if (!player)
        {
            return false; // If the player doesn't exist, we can't do collision detection
        }

        // Get player collision bounding box
        glm::vec3 playerStart = playerEntity->getComponent<CollisionComponent>()->start + playerPosition; // get the player's start position
        glm::vec3 playerEnd = playerEntity->getComponent<CollisionComponent>()->end + playerPosition;     // get the player's end position

        // For each entity in the world
        int iterator = 0;
        for (auto entity : world->getEntities())
        {
            // Get the collision component if it exists
            // std::string type = data.value("type", "");
            CollisionComponent *collision = entity->getComponent<CollisionComponent>();
            // If the collision component exists
            if (collision)
            {
                iterator += 1;
                auto objectPosition = entity->localTransform.position; // get the object's position in the world
                auto objectScale = entity->localTransform.scale;       // get the object's scale

                // Get object collision bounding box
                glm::vec3 objectStart = (collision->start + objectPosition); // get the object's start position
                glm::vec3 objectEnd = (objectPosition + collision->end);   // get the object's end position

                // loop for each axis (x,y,z)
                // for (int i = 0; i < 3; ++i) {
                // std::cout<< objectPosition[0]<< " ++++++++++ " << objectPosition[1]<<std::endl;
                // std::cout<<"player x1:"<< playerStart[0]<< " ------- " << "player x2:"<<playerEnd[0]<<std::endl;
                // std::cout<<"object x1:"<< objectStart[0]<< " ------- " << "object x2:"<< objectEnd[0]<<std::endl;
                // std::cout<<"player y1:"<< playerStart[1]<< " ------- " << "player y2:"<<playerEnd[1]<<std::endl;
                // std::cout<<"object y1:"<< objectStart[1]<< " ------- " << "object y2:"<< objectEnd[1]<<std::endl;
                // std::cout<<"player z1:"<< playerStart[2]<< " ------- " << "player z1:"<<playerEnd[2]<<std::endl;
                // std::cout<<"object z2:"<< objectStart[2]<< " ------- " << "object z2:"<< objectEnd[2]<<std::endl;
                // std::cout<<"========================================="<<std::endl;
                // if ((playerStart[0] < objectEnd[0] && playerEnd[0] > objectStart[0])
                // && (playerStart[1] < objectEnd[1] && playerEnd[1] > objectStart[1])
                // && (playerStart[2] < objectEnd[2] && playerEnd[2] > objectStart[2])
                //     ) { // if the player and object don't overlap on this axis
                //     collided = true; // then they don't collide
                //     // break;
                // }
                // }


                // if( absolute(playerPosition.x - objectPosition.x) < 1.0f && absolute(playerPosition.y - objectPosition.y) < 1.0f && absolute(playerPosition.z - objectPosition.z) < 1.0f)
                // {
                //     collided = true;

                // }

                BoundingBox playerBox = {playerStart[0], playerEnd[0], playerStart[1], playerEnd[1], playerStart[2], playerEnd[2]};
                BoundingBox obstacleBox = {objectStart[0], objectEnd[0], objectStart[1] - 1, objectEnd[1] - 1, objectStart[2], objectEnd[2]};
                collided = checkCollision(playerBox, obstacleBox);

                if (collided)
                {
                    if(entity->getComponent<PlayerComponent>())
                    {
                        std::cout << "Collide with Player"<< std::endl;
                        continue;
                    }

                    // Player hits an obstacle
                    if (entity->getComponent<MasalaComponent>())
                    { // if the object is an obstacle
                        std::cout << "collided with obstacle : " << heartCount << std::endl;

                        if (collisionStartTime == 0)
                            collisionStartTime = deltaTime; // start counting the time of collision for postprocessing effect

                        CollisionSystem::decreaseHearts(world, heartCount);

                        if (heartCount < 1)
                        { // if the player has no more hearts

                            app->changeState("game-over"); // go to the game over state
                        }
                    }

                    // Player takes a heart
                    else if (entity->getComponent<GemHeartComponent>()) // if the object is a gem heart
                    {
                        std::cout << "collided with heart" << std::endl;
                        if (heartCount < 3) // if the player has less than 3 hearts which is max
                        {
                            heartCount++; // increase the count of hearts
                        }

                        // Make heart to disappear
                        entity->localTransform.scale = glm::vec3(0.0f, 0.0f, 0.0f);
                        entity->localTransform.position = glm::vec3(0.0f, 0.0f, 0.0f);

                        for (auto heartEntity : world->getEntities())
                        {                                                                        // search for the heart entity
                            HeartComponent *heart = heartEntity->getComponent<HeartComponent>(); // get the heart component if it exists
                            if (heart && heart->heartNumber == heartCount)
                            {                                                  // if the heart component exists and it's the heart that we want to increase
                                heartEntity->localTransform.scale.x = 0.0009f; // make the heart appear
                                heartEntity->localTransform.scale.y = 0.0009f; // make the heart appear
                                heartEntity->localTransform.scale.z = 0.0009f; // make the heart appear
                                break;
                            }
                        }
                    }

                    RepeatComponent *repeatComponent = entity->getComponent<RepeatComponent>();
                    glm::vec3 &repeatPosition = entity->localTransform.position;
                    if (repeatComponent)
                    {                                                   // if the object is a repeat object
                        repeatPosition += repeatComponent->translation; // move the object forward
                    }
                    break;
                }
                // else{
                //         std::cout<<"notttttttttttttttt";

                // }
            }
        }
        // std::cout<< iterator <<std::endl;
        return (collided);
    }

    // decrease the hearts
    void CollisionSystem::decreaseHearts(World *world, int &heartCount)
    {

        for (auto heartEntity : world->getEntities())
        { // search for the heart entity
            HeartComponent *heart = heartEntity->getComponent<HeartComponent>();
            if (heart && heart->heartNumber == heartCount)
            { // if the heart component exists and it's the heart that we want to decrease
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