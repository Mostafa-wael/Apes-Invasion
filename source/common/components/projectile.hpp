#pragma once
#include "ecs/component.hpp"
#include "ecs/world.hpp"
#include "rigidbody.hpp"
#include "systems/physics.hpp"

namespace our {
    class Projectile : public Component {

    public:
        float lifetime = 2; // How long this projectile will stay alive for
        Projectile()   = default;

        // Has implicit `this` or `Shooter*` as the first argument
        void onCollision(World* world, RigidBody* other) {
            // Check if the other entity is not the shooter
            if(other->getOwner()->name != "Rotating Turret") {
                other->bulletRB->applyCentralImpulse({0, 2, 0});
            }

            // world->markForRemoval(getOwner());
        }

        void deserialize(const nlohmann::json& data) {
            // No need to deserialize since projectiles
            // are created at runtime.
        }
    };
} // namespace our