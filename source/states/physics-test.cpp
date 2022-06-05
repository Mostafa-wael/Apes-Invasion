#include "physics-test.hpp"
#include "components/rotating-turret.hpp"
#include "systems/entity-debugger.hpp"

void PhysicsTest::onInitialize() {
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

    our::CameraComponent* cam;
    for(auto e : world.getEntities()) {
        if(auto camComp = e->getComponent<our::CameraComponent>()) {
            cam = camComp;
            break;
        }
    }

    our::EntityDebugger::init(cam, getApp(), &p);

    p.initialize(&world);

    rotatingTurretSys.init(&world, &p);
}

void PhysicsTest::onDraw(double deltaTime) {
    // Here, we just run a bunch of systems to control the world logic
    movementSystem.update(&world, (float)deltaTime);
    cameraController.update(&world, (float)deltaTime);
    rotatingTurretSys.update(&world, (float)(deltaTime));
    // And finally we use the renderer system to draw the scene
    renderer.render(&world);
    projectileSystem.update(&world, dt);

    p.update(deltaTime);

    world.deleteMarkedEntities();

    dt = deltaTime;
}

void PhysicsTest::onDestroy() {
    // Don't forget to destroy the renderer
    renderer.destroy();
    // On exit, we call exit for the camera controller system to make sure that the mouse is unlocked
    cameraController.exit();
    // and we delete all the loaded assets to free memory on the RAM and the VRAM
    our::clearAllAssets();

    p.destroy();
}

void PhysicsTest::onImmediateGui() {
    our::EntityDebugger::update(&world, dt);
}
