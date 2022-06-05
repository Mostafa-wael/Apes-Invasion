#include "application.hpp"
#include "components/pick-up.hpp"
#include "ecs/entity.hpp"
#include "ecs/world.hpp"
#include "glm/fwd.hpp"
#include "systems/physics.hpp"

namespace our {
    class PickupSystem {
        Application* app;
        World* world;

    public:
        void init(World* w) {
            world = w;
            for(auto entity : world->getEntities()) {
                if(!entity->enabled || entity->getParent() != nullptr) continue;
                
                if(entity->getComponent<our::PickUp>()) {
                    entity->getComponent<RigidBody>()->setOnCollision(std::bind(
                        &PickUp::onCollision, entity->getComponent<PickUp>(), std::placeholders::_1));
                }
            }
        }
    };
}