#pragma once
#include "BulletCollision/CollisionDispatch/btCollisionObject.h"
#include "BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"
#include "ImGuizmo.h"
#include "LinearMath/btQuaternion.h"
#include "LinearMath/btScalar.h"
#include "LinearMath/btTransform.h"
#include "LinearMath/btVector3.h"
#include "application.hpp"
#include "btBulletDynamicsCommon.h"
#include "components/camera.hpp"
#include "components/mesh-renderer.hpp"
#include "ecs/IImGuiDrawable.h"
#include "ecs/entity.hpp"
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
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/matrix.hpp"
#include "imgui.h"
#include "mesh/mesh.hpp"
#include "systems/physics-debug.hpp"
#include <stdio.h>
#include <string>

#include "GLFW/glfw3.h"

namespace our {
    struct Config {
        int mWidth;
        int mHeight;
        bool mFullscreen;
    };

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
        Application* app;
        CameraComponent* camera;

        bool hit = false;
        glm::mat4 hitObjTransform;
        Entity* hitEntity;

        void initialize(World* w, Application* a, CameraComponent* cam) {
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

            world  = w;
            app    = a;
            camera = cam;

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

            rayCast();
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

        void EditTransform(const CameraComponent* camera, glm::mat4* matrix, glm::vec2 windowSize, Keyboard* kb);

        // Perhaps it'd be wise to split the real physics simulation from the picking system
        // By having 2 dynamic worlds. One for picking and the other for simulating physics.
        // We'd also have 2 components: RigidBody and Pickable.

        // The picking world would not need to simulate physics, it would only keep track of objects and their
        // AABBs, it would get updated after each frame with new object positions.

        // Might be good to look into btGhostObject
        void rayCast() {
            if(app->getMouse().justPressed(GLFW_MOUSE_BUTTON_1)) {

                // https://antongerdelan.net/opengl/raycasting.html
                glm::vec2 mousePos     = app->getMouse().getMousePosition();
                glm::vec2 framBuffSize = app->getWindowSize();

                float x           = (2.0f * mousePos.x) / framBuffSize.x - 1.0f;
                float y           = 1.0f - (2.0f * mousePos.y) / framBuffSize.y;
                float z           = 1.0f;
                glm::vec3 ray_nds = {x, y, z};

                glm::vec4 ray_clip = {ray_nds.x, ray_nds.y, -1.0, 1.0};
                glm::vec4 ray_eye  = inverse(camera->getProjectionMatrix(framBuffSize)) * ray_clip;
                ray_eye            = {ray_eye.x, ray_eye.y, -1.0, 0.0};

                glm::vec3 ray_wor = inverse(camera->getViewMatrix()) * ray_eye;
                float dist        = 20;

                glm::vec3 rayStart = camera->getOwner()->getWorldTranslation();
                glm::vec3 rayEnd   = rayStart + ray_wor * dist;

                btVector3 from = {rayStart.x, rayStart.y, rayStart.z};
                btVector3 to   = {rayEnd.x, rayEnd.y, rayEnd.z};

                btCollisionWorld::AllHitsRayResultCallback
                    allResults(from, to);

                allResults.m_flags |= btTriangleRaycastCallback::EFlags::kF_KeepUnflippedNormal;
                //kF_UseGjkConvexRaytest flag is now enabled by default, use the faster but more approximate algorithm
                //allResults.m_flags |= btTriangleRaycastCallback::kF_UseSubSimplexConvexCastRaytest;
                allResults.m_flags |= btTriangleRaycastCallback::EFlags::kF_UseSubSimplexConvexCastRaytest;

                dynamicsWorld->getDebugDrawer()->drawLine(from, to, btVector4(1, 1, 1, 1));

                dynamicsWorld->rayTest(from, to, allResults);

                if(allResults.hasHit()) {
                    printf("%s", "HIT!\n");
                    hit = true;
                    for(int i = 0; i < allResults.m_hitPointWorld.size(); i++) {
                        dynamicsWorld->getDebugDrawer()->drawSphere(allResults.m_hitPointWorld.at(i), 1, btVector3(1, 0, 0));
                        hitEntity       = static_cast<Entity*>(allResults.m_collisionObjects[0]->getUserPointer());
                        hitObjTransform = hitEntity->localTransform.toMat4();
                        break;
                    }
                } else {
                    hit = false;
                }
            }
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
            if(hit) {
                ImGuizmo::BeginFrame();
                EditTransform(camera, &hitObjTransform, app->getWindowSize(), &app->getKeyboard());
                glm::vec3 scale, translation, skew;
                glm::quat orein;
                glm::vec4 perspective;
                glm::decompose(hitObjTransform, scale, orein, translation, skew, perspective);

                hitEntity->localTransform.position    = translation;
                hitEntity->localTransform.qRot        = orein;
                hitEntity->localTransform.scale       = scale;
                hitEntity->localTransform.changedInUI = true;
                if(auto rb = hitEntity->getComponent<RigidBody>()) {
                    rb->syncWithTransform(hitEntity->getWorldRotation(), hitEntity->getWorldTranslation(), true);
                }
            }

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