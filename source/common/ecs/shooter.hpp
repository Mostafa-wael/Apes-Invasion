#pragma once

#include "asset-loader.hpp"
#include "components/mesh-renderer.hpp"
#include "ecs/component.hpp"
#include "ecs/rigidbody.hpp"
#include "ecs/world.hpp"
#include "glm/ext/quaternion_trigonometric.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/trigonometric.hpp"
#include "imgui.h"
#include "systems/physics.hpp"
#include <queue>
#include <string>
#include <vector>
namespace our {
    class Shooter : public Component {
    public:
        float firingDelay   = 0.2;
        float timer         = firingDelay;
        float lifeTime      = 1;
        float deleteTimer   = lifeTime;
        int numProjectiles  = 8;
        float rotationSpeed = 50;
        float velocity      = 20.0f;
        float spawnDist     = 5.0f;

        std::queue<Entity*> pool;

        World* zaWardo;
        Physics* physicsSystem;

        std::queue<Entity*> projectiles;
        bool t = false;

        void init(Physics* p) {
            int maxProjectiles = (numProjectiles / firingDelay) * lifeTime;

            for(int i = 0; i < maxProjectiles; i++)
                pool.push(spawnProjectile());

            if(auto rb = getOwner()->getComponent<our::RigidBody>()) {
                rb->body->setSpinningFriction(0);
            }

            physicsSystem = p;
        }

        void update(float dt) {
            timer -= dt;
            deleteTimer -= dt;

            if(auto selfRb = getOwner()->getComponent<RigidBody>()) {
                selfRb->body->setAngularVelocity({0, rotationSpeed * dt, 0});
            }

            if(timer < 0) {
                timer = firingDelay;

                glm::vec3 forward = getOwner()->getForward();
                for(int i = 0; i < numProjectiles; i++) {

                    glm::vec3 spawnPos = getOwner()->getWorldTranslation() + forward * spawnDist + getOwner()->getUp();
                    getFromPool(spawnPos, forward * velocity);
                    forward = toMat4(glm::angleAxis(glm::radians(360.0f / numProjectiles), glm::vec3(0, 1, 0))) * glm::vec4(forward, 1);
                }
            }

            if(deleteTimer < 0) {
                deleteTimer = firingDelay;
                for(int i = 0; i < numProjectiles; i++) {
                    if(projectiles.size() > 0) {
                        addToPool();
                    }
                }
            }
        }

        Entity* spawnProjectile() {

            auto e                     = zaWardo->add();
            e->enabled                 = false;
            e->name                    = "sphere##" + std::to_string((long long)e);
            e->localTransform.position = {0, 0, 0};

            auto mR      = e->addComponent<MeshRendererComponent>();
            mR->mesh     = AssetLoader<Mesh>::get("sphere");
            mR->material = AssetLoader<Material>::get("danger");

            auto rb = e->addComponent<RigidBody>();
            rb->fromSphereTranslationRotation(1, e->localTransform.position, {0, 0, 0}, "dynamic", 1);

            return e;
        }

        void getFromPool(glm::vec3 position, glm::vec3 velocity) {
            if(pool.size() > 0) {
                Entity* e = pool.front();
                pool.pop();

                e->enabled = true;

                auto rb = e->getComponent<RigidBody>();

                e->localTransform.position = position;
                rb->syncWithTransform(e->getWorldRotation(), e->getWorldTranslation());

                rb->body->setLinearVelocity({velocity.x, velocity.y, velocity.z});
                physicsSystem->dynamicsWorld->addRigidBody(rb->body);
                rb->body->setGravity({0, 0, 0});

                projectiles.push(e);
            }
        }

        void addToPool() {
            Entity* rm;
            rm = projectiles.front();
            projectiles.pop();

            pool.push(rm);
            physicsSystem->dynamicsWorld->removeRigidBody(rm->getComponent<RigidBody>()->body);
        }

        static std::string getID() { return "Shooter"; }
        virtual std::string getIDPolymorphic() override { return getID(); }

        virtual void deserialize(const nlohmann::json& data) override {
            zaWardo = getOwner()->getWorld();
        }

        virtual void onImmediateGui() override {
            ImGui::ProgressBar(1 - timer);
        }
    };
} // namespace our
