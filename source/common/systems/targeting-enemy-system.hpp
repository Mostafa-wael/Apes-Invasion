#pragma once
#include "../components/targeting-enemy.hpp"
#include "../ecs/world.hpp"
#include "components/player-shooter.hpp"
#include "glm/fwd.hpp"
#include "mesh/mesh-utils.hpp"
#include "systems/physics.hpp"
#include "components/player-controller.hpp"

namespace our {
    class TargetingEnemySystem {

    public:
        PhysicsSystem* physicsSystem;
        Entity* playerEntity;
    
        void init(World* world, PhysicsSystem* physicsSys) {
            physicsSystem = physicsSys;
            for(auto&& entity : world->getEntities()) {
                auto te = entity->getComponent<TargetingEnemy>();
                if(te){
                    te->init();
                } else {
                    auto pc = entity->getComponent<PlayerControllerComponent>();
                    if(pc) {
                        playerEntity = pc->getOwner();
                    }
                }
                
                auto rb = entity->getComponent<RigidBody>();
                if(te && rb) {
                    rb->bulletRB->setGravity({0, 0, 0});
                }
            }
            
        }

        void update(World* world, float dt) {
            for(auto&& entity : world->getEntities()) {
                if(auto targetingEnemy = entity->getComponent<TargetingEnemy>(); targetingEnemy) {

                    // Aim the turret at the player
                    auto enemyEntity = targetingEnemy->getOwner();
                    if(enemyEntity) {
                        // Make the turret aim at the player
                        auto playerPos = playerEntity->getWorldTranslation();
                        auto enemyPos = enemyEntity->getWorldTranslation();
                        // auto enemyRotation = enemyEntity->getWorldRotation();
                        // auto inverseEnemyRotation = glm::inverse(enemyRotation);
                        // auto defaultEnemyRotation = glm::quat(glm::vec3(0, 0, 0));

                        // auto dir = glm::normalize(playerPos - selfPos);
                        // TODO: Make this account for parent rotation
                        using namespace glm;
                        auto newEnemyRotation = glm::lookAt(enemyPos, playerPos, glm::vec3(0, 1, 0));
                        quat orientation = conjugate(toQuat(newEnemyRotation));
                        // enemyEntity->localTransform.setEulerRotation(0, 0, 0);
                        enemyEntity->localTransform.qRot = orientation;
                        // selfRb->bulletRB->setAngularVelocity({0, angle, 0});
                    }

                    if(targetingEnemy->shootingBehaviour->canShoot(dt)) {
                        // Shoot a projectile
                        glm::vec3 spawnPos = enemyEntity->getWorldTranslation() + enemyEntity->getForward();
                        auto defaultShootingBehaviour = dynamic_cast<DefaultShootingBehaviour*>(targetingEnemy->shootingBehaviour);
                        targetingEnemy->shootingBehaviour->shoot(world, physicsSystem, entity, spawnPos, 
                        defaultShootingBehaviour->getProjectileSpeed() * enemyEntity->getForward());
                    }
                }
            }
        }
    };
} // namespace our