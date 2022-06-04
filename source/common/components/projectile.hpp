#pragma once
#include "asset-loader.hpp"
#include "ecs/component.hpp"
#include "ecs/world.hpp"
#include "material/material.hpp"
#include "mesh/mesh-utils.hpp"
#include "rigidbody.hpp"
#include "systems/physics.hpp"
#include "util.h"
#include <functional>

namespace our {
    class Projectile : public Component {

    public:
        float lifetime = 2; // How long this projectile will stay alive for
        Material* material;

        Projectile() : material(AssetLoader<Material>::get("defaultProjectileMaterial")), lifetime(1){};
        Projectile(Material* mat, float life) : material(mat), lifetime(life) {}

        // Has implicit `this` or `Shooter*` as the first argument
        void onCollision(World* world, Entity* owner, RigidBody* other) {

            // Arbitrary effect to debug collision
            other->bulletRB->applyCentralImpulse({0, 2, 0});

            world->markForRemoval(owner);
        }

        void deserialize(const nlohmann::json& data) {
            // No need to deserialize since projectiles
            // are created at runtime.
        }

        virtual Entity* spawn(World* world, PhysicsSystem* physics, Entity* shootingEntity, float lifetime, glm::vec3 spawnPos, glm::vec3 velocity) {
            auto e  = world->add();
            e->name = "sphere##" + std::to_string((long long)e);
            e->setParent(shootingEntity);

            auto mR      = e->addComponent<MeshRendererComponent>();
            mR->mesh     = mesh_utils::sphere({6, 6});
            mR->material = material;

            auto rb = e->addComponent<RigidBody>();
            rb->fromSphereTranslationRotation(1, spawnPos, {0, 0, 0}, "dynamic", 1);
            rb->physicsSystem = physics;

            // Some stuff require us to already have the rigidbody added to bullet's physics world
            physics->dynamicsWorld->addRigidBody(rb->bulletRB);

            // One of them is disabling gravity
            rb->bulletRB->setGravity({0, 0, 0});
            rb->bulletRB->setLinearVelocity(glmToBtVector3(velocity));

            auto proj      = e->addComponent<Projectile>();
            proj->lifetime = lifetime;

            return e;
        }
    };
} // namespace our