#pragma once

#include "asset-loader.hpp"
#include "components/light.hpp"
#include "components/projectile.hpp"
#include "glad/gl.h"
#include "glm/ext/scalar_constants.hpp"
#include "glm/gtc/constants.hpp"
#include "imgui.h"
#include "mesh/mesh-utils.hpp"
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
#include "mesh/mesh.hpp"
#include "shader/shader.hpp"
#include "systems/player-shooter-system.hpp"
#include "systems/projectile-system.hpp"
#include "systems/rotating-turret-system.hpp"
#include "texture/texture2d.hpp"
#include <texture/texture-utils.hpp>
#include "systems/targeting-enemy-system.hpp"
#include "systems/health-system.hpp"
#include <fstream>
#include <iostream>

// This state shows how to use the ECS framework and deserialization.
class Playstate : public our::State {

    our::World world;
    our::ForwardRenderer renderer;
    our::FreeCameraControllerSystem cameraController;
    our::MovementSystem movementSystem;
    our::PlayerControllerSystem playerControllerSystem;
    our::RotatingTurretSystem rotatingTurretSystem;
    our::ProjectileSystem projectileSystem;
    our::PlayerShooterSystem playerShooterSystem;
    our::HealthSystem healthSystem;
    our::TargetingEnemySystem targetingEnemySystem;
    our::Entity* player;

    our::PhysicsSystem physicsSystem;
    float dt;

    our::ShaderProgram* reticleShader;
    our::Mesh* reticleMesh;
    our::Texture2D* reticleTexture;
    our::LightComponent* light;
    std::string main_menu_scene_path = "config/start.jsonc";


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

        physicsSystem.initialize(&world);

        our::CameraComponent* cam                        = nullptr;
        our::PlayerControllerComponent* playerController = nullptr;
        for(auto e : world.getEntities()) {
            if(auto camComp = e->getComponent<our::CameraComponent>())
                cam = camComp;

            if(auto playerCont = e->getComponent<our::PlayerControllerComponent>())
                playerController = playerCont;

            if(auto lightComp = e->getComponent<our::LightComponent>())
                light = lightComp;

            if(playerController && cam && light)
                break;
        }

        playerControllerSystem.init(playerController, getApp());

        rotatingTurretSystem.init(&world, &physicsSystem);

        playerShooterSystem.init(&world, getApp(), &physicsSystem);

        targetingEnemySystem.init(&world, &physicsSystem);

        healthSystem.init(&world, getApp(), &physicsSystem);

        our::EntityDebugger::init(cam, getApp(), &physicsSystem);

        reticleShader = new our::ShaderProgram();
        reticleShader->attach(config["assets"]["shaders"]["textured"]["reticle-vs"], GL_VERTEX_SHADER);
        reticleShader->attach(config["assets"]["shaders"]["textured"]["reticle-fs"], GL_FRAGMENT_SHADER);
        reticleShader->link();

        reticleMesh    = our::mesh_utils::loadReticle();
        reticleTexture = our::texture_utils::loadImage(config["assets"]["textures"]["reticle"]);

        // get the player entity
        for(auto entity : (&world)->getEntities()) {
            if(!entity->enabled || entity->getParent() != nullptr) continue;
            if(entity->getComponent<our::RigidBody>() && entity->getComponent<our::RigidBody>()->tag == "player") {
                player = entity;
                break;
            }
        }
    }

    void onDraw(double deltaTime) override {

        if(player->getComponent<our::HealthComponent>()->current_health <= 0) {
            getApp()->isGameOver = true;


            std::ifstream file_in(main_menu_scene_path);
            if(!file_in) {
                std::cerr << "Couldn't open file: " << main_menu_scene_path << std::endl;
                return;
            }
            nlohmann::json app_config = nlohmann::json::parse(file_in, nullptr, true, true);
            getApp()->setConfig(app_config);
            getApp()->changeState("main-menu");
        }

        // Here, we just run a bunch of systems to control the world logic
        movementSystem.update(&world, (float)deltaTime);
        cameraController.update(&world, (float)deltaTime);

        // And finally we use the renderer system to draw the scene
        renderer.render(&world);
        physicsSystem.update(deltaTime);

        playerControllerSystem.update(dt);
        playerShooterSystem.update(dt);
        rotatingTurretSystem.update(&world, dt);
        targetingEnemySystem.update(&world, dt);

        projectileSystem.update(&world, dt);

        healthSystem.update(dt);

        world.deleteMarkedEntities();
        dt = deltaTime;

        reticleShader->use();
        glActiveTexture(GL_TEXTURE0);
        reticleTexture->bind();
        reticleShader->set("tex", 0);
        reticleMesh->draw();
    }

    void onDestroy() override {
        // Don't forget to destroy the renderer
        renderer.destroy();
        // On exit, we call exit for the camera controller system to make sure that the mouse is unlocked
        cameraController.exit();
        // and we delete all the loaded assets to free memory on the RAM and the VRAM
        our::clearAllAssets();
        world.deleteMarkedEntities();
        world.clear();
        physicsSystem.destroy();
    }

    void onImmediateGui() override {
        player->getComponent<our::HealthComponent>()->onImmediateGui();
        ImGui::Begin("Score", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove);
        ImGui::Text("Score: %d", getApp()->score);
        ImGui::End();

        our::EntityDebugger::update(&world, dt);
    }
};