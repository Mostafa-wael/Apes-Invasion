#pragma once

#include "components/camera.hpp"
#include "components/rotating-turret.hpp"
#include "systems/projectile-system.hpp"
#include "systems/rotating-turret-system.hpp"
#include "ecs/entity.hpp"
#include "components/rotating-turret.hpp"
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

// This state shows how to use the ECS framework and deserialization.
class PhysicsTest : public our::State {

    our::World world;
    our::ForwardRenderer renderer;
    our::FreeCameraControllerSystem cameraController;
    our::MovementSystem movementSystem;
    our::Physics p;
    our::RotatingTurretSystem rotatingTurretSys;
    our::ProjectileSystem projectileSystem;

    float dt;

    void onInitialize() override;

    void onDraw(double deltaTime) override;

    void onDestroy() override;

    void onImmediateGui() override;
};