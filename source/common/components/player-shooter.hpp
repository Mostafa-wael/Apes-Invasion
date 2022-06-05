#pragma once
#include "components/player-shooter.hpp"
#include "components/projectile.hpp"
#include "components/rigidbody.hpp"
#include "ecs/entity.hpp"
#include "ecs/world.hpp"
#include "systems/physics.hpp"
#include "util.h"
#include "json/json.hpp"
#include <functional>
namespace our {

    // Defines different logic for shooting
    // Examples
    //      Shooting in an arc
    //      Shooting straight
    //      Shooting in different patterns
    //      etc..
    class IShootingBehaviour {
    public:
        bool canFire = true;

        // Defines what projectiles will be spawned when shooting
        Projectile projectileToShoot;

        // Add other functions / fields to use in `shoot` if needed

        // Just on projectile
        virtual void shoot(World* world, PhysicsSystem* physicsSystem, Entity* shootingEntity, glm::vec3 spawnPos, glm::vec3 velocity) = 0;

        // For any data that changes over time (timers for example)
        virtual bool canShoot(float dt) { return canFire; }
    };

    class DefaultShootingBehaviour : public IShootingBehaviour {
        float timer;
        float projectilesLeft;

        float projectileSpeed         = 10;
        float firingDelay             = 0.2;
        float spawnDist               = 5;
        int projectilesBeforeCooldown = 4;

    public:
        DefaultShootingBehaviour(float ps, float fd, float sd, float pbcd) : projectileSpeed(ps), firingDelay(fd), spawnDist(sd),projectilesBeforeCooldown(pbcd) {
            projectilesLeft = projectilesBeforeCooldown;
            timer           = firingDelay;
        }

        float getProjectileSpeed() { return projectileSpeed; }

        virtual void shoot(World* world, PhysicsSystem* physics, Entity* shootingEntity, glm::vec3 spawnPos, glm::vec3 velocity) override {
            if(canFire && projectilesLeft-- > 0) {
                auto projectileEntity = Projectile::spawn(world, physics, spawnPos, velocity, projectileToShoot);

                auto projectileComponent = projectileEntity->getComponent<Projectile>();
                auto projectileRB        = projectileEntity->getComponent<RigidBody>();

                projectileRB->setOnCollision(std::bind(
                    &Projectile::onCollision, projectileComponent, std::placeholders::_1));

            } else {
                canFire = false;
            }
        }

        virtual bool canShoot(float dt) override {
            if(!canFire) {
                timer -= dt;
            }

            if(timer < 0) {
                canFire         = true;
                timer           = firingDelay - dt;
                projectilesLeft = projectilesBeforeCooldown;
            }

            return canFire;
        }
    };

    class RadialShootingBehaviour : public IShootingBehaviour {
    public:
        float timer;
        int projectilesPerEvent  = 4; //  How many projectiles to spawn around the turret
        float projectileSpeed = 15.0f; // How fast the projectiles go
        float spawnDist       = 4.0f;  // How far away radially the projectiles spawn
        float firingDelay = 0.5; // Delay between shooting events
        float rotationSpeed   = 5;     // How fast the turret rotates, constant regardless of framerate

        RadialShootingBehaviour(float ps, float fd, float sd, float rs, int ppe) : projectileSpeed(ps), firingDelay(fd), spawnDist(sd), rotationSpeed(rs), projectilesPerEvent(ppe) {
            timer           = firingDelay;
        }

        virtual void shoot(World* world, PhysicsSystem* physics, Entity* shootingEntity, glm::vec3 spawnPos, glm::vec3 velocity) override {
            if(canFire) {
                Entity* turretEntity = shootingEntity;
                glm::vec3 forward    = turretEntity->getForward();
                for(int i = 0; i < projectilesPerEvent; i++) {
                    glm::vec3 spawnPos = shootingEntity->getWorldTranslation() + forward * spawnDist + shootingEntity->getUp();
                    glm::vec3 velocity = forward *projectileSpeed;
                    auto projectileEntity = Projectile::spawn(world, physics, spawnPos, velocity, projectileToShoot);

                    auto projectileComponent = projectileEntity->getComponent<Projectile>();
                    auto projectileRB        = projectileEntity->getComponent<RigidBody>();

                    projectileRB->setOnCollision(std::bind(
                        &Projectile::onCollision, projectileComponent, std::placeholders::_1));

                    // Rotate the forward vector by 360/numberProjectiles each firing event
                    forward = toMat4(glm::angleAxis(glm::radians(360.0f / projectilesPerEvent), glm::vec3(0, 1, 0))) * glm::vec4(forward, 1);
                }
                canFire = false;
            } else {
                canFire = false;
            }
        }

        virtual bool canShoot(float dt) override {
            if(!canFire) {
                timer -= dt;
            }

            if(timer < 0) {
                canFire         = true;
                timer           = firingDelay - dt;
            }

            return canFire;
        }
    };


    class PlayerShooter : public Component {
    public:
        IShootingBehaviour* shootingBehaviour;

        static std::string getID() { return "Player Shooter"; }

        void init(World* world) {
            auto playerRB = getOwner()->getComponent<RigidBody>(); // Ignore the ship's collision

            float projectileLifetime             = 5;
            shootingBehaviour                    = new DefaultShootingBehaviour(20, 0.5, 5, 3);
            playerRB->tag = "player";

            shootingBehaviour->projectileToShoot = Projectile(AssetLoader<Material>::get("playerProjectile"), projectileLifetime, playerRB->tag);

        }

        virtual void deserialize(const nlohmann::json& data) override {
        }
    };
} // namespace our