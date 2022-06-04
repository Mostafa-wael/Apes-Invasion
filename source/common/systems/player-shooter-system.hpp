#include "application.hpp"
#include "components/player-shooter.hpp"
#include "ecs/entity.hpp"
#include "ecs/world.hpp"
#include "glm/fwd.hpp"
#include "systems/physics.hpp"

namespace our {
    class PlayerShooterSystem {
        PlayerShooter* playerShooter;
        Application* app;
        PhysicsSystem* phys;
        World* world;

    public:
        void init(World* w, Application* a, PhysicsSystem* p) {
            for(auto&& entity : w->getEntities()) {
                if(auto ps = entity->getComponent<PlayerShooter>()) {
                    playerShooter = ps;
                    playerShooter->init(w);
                    break;
                }
            }
            app   = a;
            phys  = p;
            world = w;
        }

        void update(float dt) {
            if(playerShooter->shootingBehaviour->canShoot(dt) && app->getMouse().isPressed(0)) {
                glm::vec3 spawnPos = playerShooter->getOwner()->getWorldTranslation() + playerShooter->getOwner()->getForward() * 10.0f;
                playerShooter->shootingBehaviour->shoot(world, phys, playerShooter->getOwner(), spawnPos, playerShooter->getOwner()->getForward() * 20.0f);
            }
        }
    };
} // namespace our