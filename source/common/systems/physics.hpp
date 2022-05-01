#pragma once
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
#include "glm/ext/vector_float3.hpp"
#include "glm/fwd.hpp"
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
        bool simulate[2] = {false, false};
        int steps        = 10000;

        PhysDebugDraw* debugDraw;

        static void myTickCallback(btDynamicsWorld* world, btScalar timeStep) {
            int numManifolds = world->getDispatcher()->getNumManifolds();
            printf("numManifolds = %d\n", numManifolds);
        }

        void initialize() {
            ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
            collisionConfiguration = new btDefaultCollisionConfiguration();

            ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
            dispatcher = new btCollisionDispatcher(collisionConfiguration);

            ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
            overlappingPairCache = new btDbvtBroadphase();

            ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
            solver = new btSequentialImpulseConstraintSolver;

            dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

            dynamicsWorld->setGravity(btVector3(0, -1, 0));
            dynamicsWorld->setInternalTickCallback(myTickCallback);

            debugDraw = new PhysDebugDraw();
            dynamicsWorld->setDebugDrawer(debugDraw);
        }

        void addRbs(our::World* world) {
            for(auto&& e : world->getEntitiesMut()) {
                if(e->name == "camera") continue;

                RigidBody* rb = e->addComponent<RigidBody>();
                btTransform t;

                auto worldMat             = e->getLocalToWorldMatrix();
                glm::vec3 aabbCenter      = e->getComponent<MeshRendererComponent>()->mesh->aabbCenter;
                glm::vec3 aabbHalfExtents = e->getComponent<MeshRendererComponent>()->mesh->aabbHalfExtents;
                glm::vec3 wAABBPos        = glm::vec4(aabbCenter, 1);
                glm::vec3 wRot            = e->localTransform.getEulerRotation();

                if(e->name == "ground") {
                    *rb = RigidBody(aabbHalfExtents, wAABBPos, wRot, 0);

                } else {
                    *rb = RigidBody(aabbHalfExtents, wAABBPos, wRot, 1);
                }
                dynamicsWorld->addRigidBody(rb->body);
            }
        }

        void update(World* world, float dt) {

            if(simulate[0] && !simulate[1]) {
                for(auto e : world->getEntities()) {
                    if(auto rb = e->getComponent<RigidBody>()) {
                        btTransform t;
                        t.setFromOpenGLMatrix(glm::value_ptr(e->getLocalToWorldMatrix()));

                        rb->body->setWorldTransform(t);
                    }
                }
            }

            simulate[1] = simulate[0];

            if(!simulate[0] || steps == 0) {
                simulate[0] = false;
                return;
            }

            steps--;
            int i;

            glm::mat4 view, projection;
            for(auto&& e : world->getEntitiesMut()) {
                if(auto rb = e->getComponent<RigidBody>()) {
                    btTransform rbT;
                    rb->body->getMotionState()->getWorldTransform(rbT);

                    e->localTransform.position = {rbT.getOrigin().x(),
                                                  rbT.getOrigin().y(),
                                                  rbT.getOrigin().z()};

                    btQuaternion qRot      = rbT.getRotation();
                    e->localTransform.qRot = {qRot.x(), qRot.y(), qRot.z(), qRot.w()};

                    rb->body->applyTorque({0, 10 * dt, 0});
                    rb->body->activate(true);
                }
                if(auto cam = e->getComponent<CameraComponent>()) {
                    view       = cam->getViewMatrix();
                    projection = cam->getProjectionMatrix({1280, 720});
                }
            }

            dynamicsWorld->stepSimulation(dt);

            debugDraw->SetMatrices(view, projection);
            dynamicsWorld->debugDrawWorld();
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

        virtual void onImmediateGui() override {
            if(ImGui::CollapsingHeader("Physics")) {
                ImGui::Indent(10);
                ImGui::Checkbox("simulate", &simulate[0]);
                ImGui::InputInt("steps", &steps);

                for(int j = dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--) {
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
                }
                ImGui::Indent(-10);
            }
        };
    };
} // namespace our