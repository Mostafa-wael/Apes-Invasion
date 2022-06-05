#pragma once
#include "asset-loader.hpp"
#include "components/light.hpp"
#include "components/health.hpp"
#include "ecs/component.hpp"
#include "ecs/world.hpp"
#include "material/material.hpp"
#include "mesh/mesh-utils.hpp"
#include "rigidbody.hpp"
#include "systems/physics.hpp"
#include "util.h"
#include <functional>
#include <string>
#include <unordered_set>

// #define MINIAUDIO_IMPLEMENTATION
// #include "../../../vendor/miniaudio.h"
// > gcc -W -Wall -Wextra -g simple_playback.c -lpthread -lm -ldl -o simple_playback
// #include <stdio.h>

namespace our {
    class Projectile : public Component {

    public:
        Material* material;
        float lifetime = 2; // How long this projectile will stay alive for
        std::string rigidbodyTag;
        int damageAmount = 10;

        Projectile() = default;
        Projectile(Material* mat, float life, std::string tag) : material(mat), lifetime(life), rigidbodyTag(tag) {}

        // Has implicit `this` or `Shooter*` as the first argument
        // owner: entity carrying this component, need to pass it manually for the callback
        void onCollision(RigidBody* other) {
            auto projectileRB = getOwner()->getComponent<RigidBody>();


            // Ignore other objects that have the same tag as you
            if(projectileRB->tag == other->tag) return;

            other->bulletRB->applyCentralForce(projectileRB->bulletRB->getLinearVelocity() * projectileRB->bulletRB->getMass());

            if(getOwner())
                getOwner()->getWorld()->markForRemoval(getOwner());
                
            auto otherProjectile = other->getOwner()->getComponent<Projectile>();
            if(otherProjectile)
                otherProjectile->getOwner()->getWorld()->markForRemoval(otherProjectile->getOwner());

            if(other->getOwner()->getComponent<HealthComponent>()) 
            {
                other->getOwner()->getComponent<HealthComponent>()->damage(damageAmount);

            }
            
        }

        void deserialize(const nlohmann::json& data) {
            // No need to deserialize since projectiles
            // are created at runtime.
        }

        static Entity* spawn(
            World* world,
            PhysicsSystem* physics,
            glm::vec3 spawnPos,
            glm::vec3 velocity,
            Projectile projectile) {
            auto e  = world->add();
            e->name = "sphere##" + std::to_string((long long)e);

            auto mR      = e->addComponent<MeshRendererComponent>();
            mR->mesh     = AssetLoader<Mesh>::get("sphere");
            mR->material = projectile.material;

            auto rb = e->addComponent<RigidBody>();
            rb->tag = projectile.rigidbodyTag;
            rb->fromSphereTranslationRotation(1, spawnPos, {0, 0, 0}, "dynamic", 1);
            rb->physicsSystem = physics;

            // Some stuff require us to already have the rigidbody added to bullet's physics world
            physics->dynamicsWorld->addRigidBody(rb->bulletRB);

            // One of them is disabling gravity
            rb->bulletRB->setGravity({0, 0, 0});
            rb->bulletRB->setLinearVelocity(glmToBtVector3(velocity));

            auto proj      = e->addComponent<Projectile>();
            proj->lifetime = projectile.lifetime;

            return e;
        }
    };
} // namespace our