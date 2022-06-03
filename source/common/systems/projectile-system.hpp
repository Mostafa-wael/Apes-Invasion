#pragma once

#include "components/projectile.hpp"
#include "ecs/world.hpp"
namespace our {
    class ProjectileSystem {

    public:
        void update(World* world, float dt) {
            for(auto&& entity : world->getEntities()) {

                auto projectile = entity->getComponent<Projectile>();

                if(entity->enabled && projectile) {
                    projectile->lifetime -= dt;

                    if(projectile->lifetime < 0) {
                        world->markForRemoval(projectile->getOwner());
                    }
                }
            }
        }
    };
} // namespace our