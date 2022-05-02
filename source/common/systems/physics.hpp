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
        bool simulate[2] = {false, false};
        int framesToSimulate        = 10000;

        PhysDebugDraw* debugDraw;

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

            dynamicsWorld->setGravity(btVector3(0, -9.81, 0));

            debugDraw = new PhysDebugDraw();
            dynamicsWorld->setDebugDrawer(debugDraw);
        }

        void addRbs(our::World* world) {
            for(auto&& e : world->getEntitiesMut()) {
                if(e->name == "camera") continue;

                RigidBody* rb = e->addComponent<RigidBody>();

                glm::vec3 wPos   = e->getWorldTranslation();
                glm::vec3 wRot   = glm::eulerAngles(glm::quat(e->getWorldRotation()));
                glm::vec3 wScale = e->getWorldScale();

                auto [wAABBPos, aabbHalfExtents] = e->getComponent<MeshRendererComponent>()->mesh->getAABB(glm::eulerAngleXYZ(0, 0, 0), wScale);
                wAABBPos                         = glm::translate(glm::mat4(1), wPos) * glm::vec4(wAABBPos, 1);

                if(e->name == "ground") {
                    *rb = RigidBody(aabbHalfExtents, wAABBPos, wRot, 0);
                    rb->body->setCollisionFlags(rb->body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
                } else {
                    *rb = RigidBody(aabbHalfExtents, wAABBPos, wRot, 1);
                    rb->body->applyImpulse({1, -5, 1}, rb->body->getCenterOfMassPosition());
                }
                rb->body->setActivationState(DISABLE_DEACTIVATION);
                dynamicsWorld->addRigidBody(rb->body);
            }
        }

        void update(World* world, float dt) {

            if(simulate[0] && !simulate[1]) {
                for(auto i = world->getEntities().begin(); i != world->getEntities().end() ; i++) {
                    auto e = *i;
                    if(auto rb = e->getComponent<RigidBody>()) {
                      // Can move both getWorldRotation/Translation to inside the method
                      // but it segfaults for some reason.
                      rb->syncWithTransform(e->getWorldRotation(), e->getWorldTranslation());
                    }
                }
            }
            
            glm::mat4 view, projection;
            for(auto&& e : world->getEntities()) {
                if(auto cam = e->getComponent<CameraComponent>()) {
                    view       = cam->getViewMatrix();
                    projection = cam->getProjectionMatrix({1280, 720});
                    break;
                }
            }

            debugDraw->SetMatrices(view, projection);
            dynamicsWorld->debugDrawWorld();

            if(!simulate[0] || framesToSimulate == 0) {
                return;
            }

            simulate[1] = simulate[0];

            framesToSimulate--;
            int i;

            dynamicsWorld->stepSimulation(dt);

            for(auto&& e : world->getEntitiesMut()) {
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
                ImGui::InputInt("frames to simulate", &framesToSimulate);

                ImGui::Indent(-10);
            }
        };
    };
} // namespace our