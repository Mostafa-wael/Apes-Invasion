#pragma once
#include "../components/rotating-turret.hpp"
#include "../ecs/world.hpp"
#include "components/rigidbody.hpp"
#include "mesh/mesh-utils.hpp"
#include "systems/physics.hpp"

namespace our {
    class RotatingTurretSystem {

    public:
        PhysicsSystem* physicsSystem;

        void init(World* world, PhysicsSystem* physicsSys) {
            physicsSystem = physicsSys;
            for(auto&& entity : world->getEntities()) {
                auto rt = entity->getComponent<RotatingTurret>();
                auto rb = entity->getComponent<RigidBody>();
                if(rt && rb) {
                    rb->bulletRB->setGravity({0, 0, 0});
                }
            }
        }

        void update(World* world, float dt) {
            for(auto&& entity : world->getEntities()) {
                if(auto rotatingTurret = entity->getComponent<RotatingTurret>(); rotatingTurret) {

                    // Rotate the turret
                    auto selfRb = rotatingTurret->getOwner()->getComponent<RigidBody>();
                    if(selfRb) {
                        selfRb->bulletRB->setAngularVelocity({0, rotatingTurret->rotationSpeed, 0});
                    }

                    if(rotatingTurret->shootingBehaviour->canShoot(dt)) {
                        Entity* turretEntity = rotatingTurret->getOwner();
                        glm::vec3 forward    = turretEntity->getForward();
                        for(int i = 0; i < rotatingTurret->projectilesPerEvent; i++) {

                            glm::vec3 spawnPos = turretEntity->getWorldTranslation() + forward * rotatingTurret->spawnDist + turretEntity->getUp();
                            rotatingTurret->shootingBehaviour->shoot(world, physicsSystem, entity, spawnPos, forward * rotatingTurret->projectileSpeed);

                            // Rotate the forward vector by 360/numberProjectiles each firing event
                            forward = toMat4(glm::angleAxis(glm::radians(360.0f / rotatingTurret->projectilesPerEvent), glm::vec3(0, 1, 0))) * glm::vec4(forward, 1);
                        }
                    }
                }
            }
        }
    };
} // namespace our