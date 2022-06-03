#pragma once

#include "glm/ext/scalar_constants.hpp"
#include "glm/gtc/constants.hpp"
#include "imgui.h"
#include "systems/entity-debugger.hpp"
#include "systems/physics.hpp"
#include <application.hpp>

#include <asset-loader.hpp>
#include <ecs/world.hpp>
#include <string>
#include <systems/forward-renderer.hpp>
#include <systems/free-camera-controller.hpp>
#include <systems/movement.hpp>
#include <systems/player-controller-system.hpp>

#include "components/camera.hpp"
#include "components/rigidbody.hpp"
#include "ecs/entity.hpp"

// This state shows how to use the ECS framework and deserialization.
class Playstate : public our::State {

    our::World world;
    our::ForwardRenderer renderer;
    our::FreeCameraControllerSystem cameraController;
    our::MovementSystem movementSystem;
    our::PlayerControllerSystem playerControllerSystem;

    our::Physics p;
    float dt;

    void onInitialize() override {
        
        // First of all, we get the scene configuration from the app config
        auto& config = getApp()->getConfig()["scene"];

        // If we have assets in the scene config, we deserialize them
        if(config.contains("assets")) {
            our::deserializeAllAssets(config["assets"]);
        }

        // If we have a world in the scene config, we use it to populate our world
        if(config.contains("world")) {
            world.deserialize(config["world"]);
        }

        // We initialize the camera controller system since it needs a pointer to the app
        cameraController.enter(getApp());

        // Then we initialize the renderer
        auto size = getApp()->getFrameBufferSize();
        renderer.initialize(size, config["renderer"]);
        renderer.app = getApp();

        p.initialize(&world);


        our::CameraComponent* cam = nullptr;
        our::PlayerControllerComponent* playerController = nullptr;
        for(auto e : world.getEntities()) {
            if(auto camComp = e->getComponent<our::CameraComponent>())
                cam = camComp;

            if(auto playerCont = e->getComponent<our::PlayerControllerComponent>())
                playerController = playerCont;

            if(playerController && cam)
                break;
        }

        our::EntityDebugger::init(cam, getApp());

        playerControllerSystem.init(playerController, getApp());
    }

    void onDraw(double deltaTime) override {
        // Here, we just run a bunch of systems to control the world logic
        movementSystem.update(&world, (float)deltaTime);
        cameraController.update(&world, (float)deltaTime);

        // And finally we use the renderer system to draw the scene
        renderer.render(&world);
        p.update(deltaTime);

        playerControllerSystem.update(dt);

        world.deleteMarkedEntities();
        dt = deltaTime;
    }

    void onDestroy() override {
        // Don't forget to destroy the renderer
        renderer.destroy();
        // On exit, we call exit for the camera controller system to make sure that the mouse is unlocked
        cameraController.exit();
        // and we delete all the loaded assets to free memory on the RAM and the VRAM
        our::clearAllAssets();
        p.destroy();
    }

    void onImmediateGui() override {
        ImGui::Begin("KAK Engine");
        ImGui::Text("Current frametime: %f", dt);
        our::EntityDebugger::update(&world, dt);
        ImGui::End();

        p.onImmediateGui();
    }
};