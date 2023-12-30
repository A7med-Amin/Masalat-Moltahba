#pragma once

#include "../ecs/world.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include "../application.hpp"


namespace our {

    // The final line system is responsible for detection of final line reach.
    class FinalLineSystem {
    public:
        FinalLineSystem() {
            app = nullptr;
        }

        Application *app; // The application in which the state runs
        // When a state enters, it should call this function and give it the pointer to the application
        void enter(Application *app) {
            this->app = app;
        }

        // This should be called every frame to update all entities containing a MovementComponent.
        void update(World *world, float deltaTime, int &heartCount);
    };

}
