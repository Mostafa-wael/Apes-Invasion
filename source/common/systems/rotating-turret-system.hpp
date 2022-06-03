#pragma once
#include "../components/rotating-turret.hpp"
#include "../ecs/world.hpp"
#include "systems/physics.hpp"

namespace our {
    class RotatingTurretSystem {

    public:
        Physics* physicsSystem;

        void init(World* world, Physics* physicsSys) {
            physicsSystem = physicsSys;
        }

        void update(World* world, float dt) {
            for(auto&& entity : world->getEntities()) {
                if(auto rotatingTurret = entity->getComponent<RotatingTurret>(); rotatingTurret) {

                    // Rotate the turret
                    auto selfRb = rotatingTurret->getOwner()->getComponent<RigidBody>();
                    if(selfRb) {
                        selfRb->bulletRB->setAngularVelocity({0, rotatingTurret->rotationSpeed, 0});
                    }

                    // Fire with delay
                    rotatingTurret->firingTimer -= dt;

                    if(rotatingTurret->firingTimer < 0) {
                        rotatingTurret->firingTimer = rotatingTurret->firingDelay;

                        Entity* turretEntity = rotatingTurret->getOwner();
                        glm::vec3 forward    = turretEntity->getForward();
                        for(int i = 0; i < rotatingTurret->projectilesPerDelay; i++) {

                            glm::vec3 spawnPos = turretEntity->getWorldTranslation() + forward * rotatingTurret->spawnDist + turretEntity->getUp();
                            auto projectile    = spawnProjectile(world, rotatingTurret, spawnPos, forward * rotatingTurret->projectileVelocity);

                            physicsSystem->dynamicsWorld->addRigidBody(projectile->getComponent<RigidBody>()->bulletRB);

                            // Rotate the forward vector by 360/numberProjectiles each firing event
                            forward = toMat4(glm::angleAxis(glm::radians(360.0f / rotatingTurret->projectilesPerDelay), glm::vec3(0, 1, 0))) * glm::vec4(forward, 1);
                        }
                    }
                }
            }
        }

    private:
        Entity* spawnProjectile(World* zaWardo, RotatingTurret* rt, glm::vec3 spawnPos, glm::vec3 movementVelocity) {

            auto e                     = zaWardo->add();
            e->name                    = "sphere##" + std::to_string((long long)e);
            e->localTransform.position = spawnPos;
            e->setParent(rt->getOwner());

            auto mR      = e->addComponent<MeshRendererComponent>();
            mR->mesh     = AssetLoader<Mesh>::get("sphere");
            mR->material = AssetLoader<Material>::get("danger");

            auto rb = e->addComponent<RigidBody>();
            rb->fromSphereTranslationRotation(1, e->localTransform.position, {0, 0, 0}, "dynamic", 1);
            rb->physicsSystem = physicsSystem;
            rb->bulletRB->clearGravity();

            auto mv = movementVelocity;
            rb->bulletRB->setLinearVelocity({mv.x, mv.y, mv.z});

            auto proj = e->addComponent<Projectile>();

            // Set collision callback
            // std::bind takes a function as the first argument
            // The rest of the arguments are either a parameter we wish to bind to
            // an argument.
            // Or a placeholder to allow for the use of the function with an argument.
            // How to use a member function?
            // Send the function as &ClassName::functionName (function pointer),
            // Bind the instance you want to use to the first argument
            // In C++, the first argument in member functions is implicitly set to `this`,
            // but to use the member function in this context, we must bind it manually.
            // You then bind any other parameters you wish to.
            // This converts the function from fn(Shooter*, RigidBody*) -> void
            // To fn(RigidBody*) -> void
            rb->setOnCollision(std::bind(&Projectile::onCollision,
                                         proj, zaWardo, std::placeholders::_1));

            // Here, we use std::bind to convert the lambda from a function that takes (Entity*, RigidBody*)
            // and returns void, to a function that takes (RigidBody*) and returns void.
            // This is done by binding the entity we wish to use to the function's first parameter
            // rb->setOnCollision(
            //     std::bind(
            //  //        vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
            //         [](Entity* e, RigidBody* other) {
            //             std::cout << e->name << std::endl;
            //         },
            //         e, // Bound to the first argument
            //         std::placeholders::_1 // Bound to the second
            //      )
            //  );

            // Can also let the lambda capture values automatically
            // rb->setOnCollision(
            //     vvv
            //     [&](RigidBody* other) {
            //         std::cout << e->name << std::endl;
            //     });

            return e;
        }
    };
} // namespace our