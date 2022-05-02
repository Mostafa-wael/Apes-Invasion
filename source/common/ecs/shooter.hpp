#pragma once

#include "asset-loader.hpp"
#include "components/mesh-renderer.hpp"
#include "ecs/component.hpp"
#include "ecs/rigidbody.hpp"
#include "ecs/world.hpp"
#include "imgui.h"
#include "systems/physics.hpp"
#include <queue>
#include <string>
#include <vector>
namespace our {
    class Shooter : public Component {
    public:
        float timer       = 1;
        float deleteTimer = 2;
        World* zaWardo;
        Physics* physicsSystem;

        std::queue<Entity*> projectiles;
        bool t = false;

        void update(float dt) {
            timer -= dt;
            deleteTimer -= dt;
            if(timer < 0) {
                timer = 1;

                auto e  = zaWardo->add();
                e->name = "sphere" + std::to_string((long long)e);

                e->localTransform.position = getOwner()->localTransform.position + glm::vec3(0, 3, 0);

                auto mR      = e->addComponent<MeshRendererComponent>();
                mR->mesh     = AssetLoader<Mesh>::get("sphere");
                mR->material = AssetLoader<Material>::get("metal");

                auto rb = e->addComponent<RigidBody>();
                rb->fromMeshRenderer(mR, "dynamic", 1);
                rb->body->setGravity({0, -10, 0});

                float x = e->localTransform.position.x;
                float y = e->localTransform.position.y;
                float z = e->localTransform.position.z;

                rb->body->applyImpulse({0, 10, 0}, {x, y, z});

                physicsSystem->dynamicsWorld->addRigidBody(rb->body);
                projectiles.push(e);
            }

            if(deleteTimer < 0) {
                deleteTimer = 2;
                if(projectiles.size() > 0) {
                    Entity* rm;
                    rm = projectiles.front();
                    projectiles.pop();

                    zaWardo->markForRemoval(rm);
                    physicsSystem->dynamicsWorld->removeRigidBody(rm->getComponent<RigidBody>()->body);
                }
            }
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
