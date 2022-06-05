#pragma once
#include "../components/rotating-turret.hpp"
#include "../ecs/world.hpp"
#include "components/player-shooter.hpp"
#include "components/rigidbody.hpp"
#include "glm/fwd.hpp"
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
                if(rt){
                    rt->init();
                }
                
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
                        selfRb->bulletRB->setAngularVelocity({0,  dynamic_cast<RadialShootingBehaviour*>(rotatingTurret->shootingBehaviour)->rotationSpeed, 0});
                    }

                    if(rotatingTurret->shootingBehaviour->canShoot(dt)) {
                        rotatingTurret->shootingBehaviour->shoot(world, physicsSystem, entity, glm::vec3(0), glm::vec3(0));
                    }
                }
            }
        }
    };
} // namespace our