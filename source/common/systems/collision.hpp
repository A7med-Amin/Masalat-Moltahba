// TODO: (Phase 2) collision system
#pragma once

#include "../ecs/world.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include "../application.hpp"

#ifdef USE_SOUND

#include "../../vendor/irrKlang/include/irrKlang.h"

#endif

namespace our
{
    // The collision system is responsible for detecting collisions between entities.
    class CollisionSystem
    {
#ifdef USE_SOUND
        irrklang::ISoundEngine *soundEngine;
#endif
    public:
        CollisionSystem()
        {
#ifdef USE_SOUND
            soundEngine = irrklang::createIrrKlangDevice();
#endif
        }
        Application *app; // The application in which the state runs
        // When a state enters, it should call this function and give it the pointer to the application
        void enter(Application *app)
        {
            this->app = app;
        }

        // This should be called every frame to update all entities containing a MovementComponent.
        bool update(World *world, float deltaTime, int &heartCount,
                    float &collisionStartTime);

        // this function checks masala height if it is higher than threshold it decreases in height, othoerwise, ti increases in height
        void checkMasalaHeight(World *world, float deltaTime);
        // This function is called when the player collides with an obstacle
        void decreaseHearts(World *world, int &heartCount);
    };
}