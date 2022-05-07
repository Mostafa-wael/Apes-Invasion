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
        float firingDelay = 0.5;
        float timer       = firingDelay;

        float lifeTime     = 2;
        float deleteTimer  = lifeTime;
        int numProjectiles = 1;

        float rotationSpeed = 50;
        float velocity      = 15.0f;
        float spawnDist     = 3.0f;

        World* zaWardo;
        Physics* physicsSystem;

        int projToSpawn   = 0;
        int projToDespawn = 0;
        std::vector<Entity*> projectiles;
        int maxProjectiles;

        bool t = false;

        void init(Physics* p) {
            maxProjectiles = (numProjectiles / firingDelay) * lifeTime;

            for(int i = 0; i < maxProjectiles; i++)
                projectiles.push_back(spawnProjectile());

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
            e->parent                  = getOwner();
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
            Entity* e = projectiles[projToSpawn];

            projToSpawn = (projToSpawn + 1) % maxProjectiles;

            e->enabled = true;

            auto rb = e->getComponent<RigidBody>();

            e->localTransform.position = position;
            rb->syncWithTransform(e->getWorldRotation(), e->getWorldTranslation());

            rb->body->setLinearVelocity({velocity.x, velocity.y, velocity.z});
            physicsSystem->dynamicsWorld->addRigidBody(rb->body);
            rb->body->setGravity({0, 0, 0});
        }

        void addToPool() {
            Entity* rm    = projectiles[projToDespawn];
            projToDespawn = (projToDespawn + 1) % maxProjectiles;
            physicsSystem->dynamicsWorld->removeRigidBody(rm->getComponent<RigidBody>()->body);
            rm->enabled = false;
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
