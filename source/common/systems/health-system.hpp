#include "application.hpp"
#include "components/health.hpp"
#include "components/player-shooter.hpp"
#include "ecs/entity.hpp"
#include "ecs/world.hpp"
#include "glm/fwd.hpp"
#include "systems/physics.hpp"

namespace our {
    class HealthSystem {
        Application* app;
        World* world;

    public:
        void init(World* w, Application* a, PhysicsSystem* p) {
            app   = a;
            world = w;
        }

        void update(float dt) {
        for(auto entity : world->getEntities()) {
            if(!entity->enabled || entity->getParent() != nullptr) continue;
            if(entity->getComponent<our::HealthComponent>() && entity->getComponent<our::RigidBody>() && entity->getComponent<our::RigidBody>()->tag != "player") {
                if(entity->getComponent<our::HealthComponent>()->current_health <= 0) {
                    world->markForRemoval(entity);
                    app->score += 10;
                }
            }
        }
        }
    };
} // namespace our