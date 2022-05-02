#pragma once
#include "BulletCollision/CollisionDispatch/btCollisionObject.h"
#include "LinearMath/btQuaternion.h"
#include "LinearMath/btScalar.h"
#include "LinearMath/btTransform.h"
#include "LinearMath/btVector3.h"
#include "btBulletDynamicsCommon.h"
#include "components/camera.hpp"
#include "components/mesh-renderer.hpp"
#include "ecs/IImGuiDrawable.h"
#include "ecs/rigidbody.hpp"
#include "ecs/transform.hpp"
#include "ecs/world.hpp"
#include "glm/common.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/quaternion_float.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float4.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/matrix.hpp"
#include "imgui.h"
#include "mesh/mesh.hpp"
#include "systems/physics-debug.hpp"
#include <stdio.h>
#include <string>

namespace our {
    class Physics : public IImGuiDrawable {
    public:
        btDefaultCollisionConfiguration* collisionConfiguration;
        btCollisionDispatcher* dispatcher;
        btBroadphaseInterface* overlappingPairCache;
        btSequentialImpulseConstraintSolver* solver;
        btDiscreteDynamicsWorld* dynamicsWorld;
        // MeshRendererComponent box;
        bool simulate        = true;
        int framesToSimulate = 10000;

        PhysDebugDraw* debugDrawer;

        World* world;

        void initialize(World* w) {
            ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
            collisionConfiguration = new btDefaultCollisionConfiguration();

            ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
            dispatcher = new btCollisionDispatcher(collisionConfiguration);

            ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
            overlappingPairCache = new btDbvtBroadphase();

            ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
            solver = new btSequentialImpulseConstraintSolver;

            dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

            dynamicsWorld->setGravity(btVector3(0, -9.81, 0));

            debugDrawer = new PhysDebugDraw();
            dynamicsWorld->setDebugDrawer(debugDrawer);

            world = w;

            for(auto&& e : world->getEntities())
                if(auto rb = e->getComponent<RigidBody>()) dynamicsWorld->addRigidBody(rb->body);
        }

        void update(float dt) {
            debugDraw();

            if(!simulate || framesToSimulate == 0) {
                return;
            }

            framesToSimulate--;
            int i;

            dynamicsWorld->stepSimulation(dt);

            bulletToOur();
        }

        void destroy() {

            for(int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--) {
                btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
                btRigidBody* body      = btRigidBody::upcast(obj);
                if(body && body->getMotionState()) {
                    delete body->getMotionState();
                }
                dynamicsWorld->removeCollisionObject(obj);
                delete obj;
            }

            //delete dynamics world
            delete dynamicsWorld;

            //delete solver
            delete solver;

            //delete broadphase
            delete overlappingPairCache;

            //delete dispatcher
            delete dispatcher;

            delete collisionConfiguration;
        }

        // Reads data from bullet physics and updates object transform for graphics drawing
        void bulletToOur() {
            for(auto&& e : world->getEntitiesMut()) {
                if(!e->enabled) continue;
                if(auto rb = e->getComponent<RigidBody>()) {
                    btTransform rbT;
                    rb->body->getMotionState()->getWorldTransform(rbT);

                    e->localTransform.position = {rbT.getOrigin().x(),
                                                  rbT.getOrigin().y(),
                                                  rbT.getOrigin().z()};

                    btQuaternion qRot      = rbT.getRotation();
                    e->localTransform.qRot = {qRot.w(), qRot.x(), qRot.y(), qRot.z()};
                }
            }
        }

        void debugDraw() const {

            // Get view and projection matrices for proper drawing
            glm::mat4 view, projection;
            for(auto&& e : world->getEntities()) {
                if(auto cam = e->getComponent<CameraComponent>()) {
                    view       = cam->getViewMatrix();
                    projection = cam->getProjectionMatrix({1280, 720});
                    break;
                }
            }

            debugDrawer->SetMatrices(view, projection);

            // Draw what bullet physics sees
            dynamicsWorld->debugDrawWorld();
        }

        // Reads our data and sets bullet rigidbodies with it
        void ourToBullet() {
            for(auto e : world->getEntities()) {
                if(!e->enabled) continue;

                if(auto rb = e->getComponent<RigidBody>()) {
                    // Can move both getWorldRotation/Translation to inside the method
                    // but it segfaults for some reason.
                    rb->syncWithTransform(e->getWorldRotation(), e->getWorldTranslation());
                }
            }
        }

        virtual void onImmediateGui() override {
            ImGui::Begin("Physics");
            ImGui::Indent(10);

            if(ImGui::Checkbox("simulate", &simulate) && simulate) {
                ourToBullet();
            }

            ImGui::InputInt("frames to simulate", &framesToSimulate);

            for(int j = dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--) {

                if(ImGui::TreeNode(("Rigidbody ##" + std::to_string(j)).c_str())) {

                    btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[j];
                    btRigidBody* body      = btRigidBody::upcast(obj);
                    btTransform trans;
                    if(body && body->getMotionState()) {
                        body->getMotionState()->getWorldTransform(trans);
                    } else {
                        trans = obj->getWorldTransform();
                    }
                    float x[] = {trans.getOrigin().x(), trans.getOrigin().y(), trans.getOrigin().z()};
                    ImGui::DragFloat3(("kak##" + std::to_string(j)).c_str(), x);
                    ImGui::TreePop();
                }
            }

            ImGui::Indent(-10);
            ImGui::End();
        };
    };
} // namespace our