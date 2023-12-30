#pragma once

#include <application.hpp>
#include <iostream>

#include <ecs/world.hpp>
#include <systems/forward-renderer.hpp>
#include <systems/free-camera-controller.hpp>
#include <systems/movement.hpp>
#include <../common/components/player.hpp>
#include <asset-loader.hpp>
#include <systems/final-line.hpp>
#include <systems/collision.hpp>
#include <systems/repeat.hpp>
#include <systems/collision.hpp>




// This state shows how to use the ECS framework and deserialization.
class Playstate: public our::State {

    our::World world;
    our::ForwardRenderer renderer;
    our::FreeCameraControllerSystem cameraController;
    our::MovementSystem movementSystem;
    our::CollisionSystem collisionSystem;
    our::RepeatSystem repeatSystem;
    our::FinalLineSystem finalLineSystem;

    int heartCount = 3;
    float collisionStartTime = 0;

    our::PlayerComponent myPlayer;


    void onInitialize() override {
        // First of all, we get the scene configuration from the app config
        auto& config = getApp()->getConfig()["scene"];
        // If we have assets in the scene config, we deserialize them
        if(config.contains("assets")){
            our::deserializeAllAssets(config["assets"]);
        }
        // If we have a world in the scene config, we use it to populate our world
        if(config.contains("world")){
            world.deserialize(config["world"]);
        }
        // We initialize the camera controller system since it needs a pointer to the app
        // checkMasalaHeight(getApp());
        cameraController.enter(getApp());
        collisionSystem.enter(getApp());
        finalLineSystem.enter(getApp());
        renderer.enter(getApp());

        // We initialize the player component since it needs a pointer to the app
        myPlayer.enter(getApp());

        // Then we initialize the renderer
        auto size = getApp()->getFrameBufferSize();
        renderer.initialize(size, config["renderer"]);
    }

    void onDraw(double deltaTime) override {
        // Here, we just run a bunch of systems to control the world logic
        bool didCollide = false;
        movementSystem.update(&world, (float)deltaTime);
        myPlayer.update(&world, (float)deltaTime * getApp()->getDifficulty());
        // std::cout<<(myPlayer.speed);
        // float collisionStartTime = 0; // temporaryyyyyyyyyyyyyyyy
        didCollide = collisionSystem.update(&world,(float) deltaTime * getApp()->getDifficulty() , getApp()->heartCount, collisionStartTime);
        cameraController.update(&world, (float)deltaTime * getApp()->getDifficulty() , didCollide);
        
        repeatSystem.update(&world, (float) deltaTime);
        finalLineSystem.update(&world, (float) deltaTime, getApp()->heartCount);

        std::string postProcessFrag = "assets/shaders/postprocess/vignette.frag";
        
        // postProcessFrag = "assets/shaders/postprocess/sandWethereEffect.frag";
        if (collisionStartTime != 0) {
            collisionStartTime += (float) deltaTime;
            postProcessFrag = "assets/shaders/postprocess/Grain.frag";
        }
        // Collision effect for 100 time
        if (collisionStartTime >= 20 * deltaTime)collisionStartTime = 0;
        // And finally we use the renderer system to draw the scene
        renderer.render(&world, postProcessFrag);

        // Get a reference to the keyboard object
        auto& keyboard = getApp()->getKeyboard();

        if(keyboard.justPressed(GLFW_KEY_ESCAPE)){
            // If the escape  key is pressed in this frame, go to the play state
            getApp()->changeState("menu");
        }
    }

    void onDestroy() override {
        // Don't forget to destroy the renderer
        renderer.destroy();
        // On exit, we call exit for the camera controller system to make sure that the mouse is unlocked
        cameraController.exit();
        // Clear the world
        world.clear();
        // and we delete all the loaded assets to free memory on the RAM and the VRAM
        our::clearAllAssets();
    }
};