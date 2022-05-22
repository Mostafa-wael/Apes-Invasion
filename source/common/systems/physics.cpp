#include "physics.hpp"
#include "LinearMath/btVector3.h"
#include "ecs/entity.hpp"
#include "input/keyboard.hpp"
#include "physics.hpp"
#include "systems/entity-debugger.hpp"
#include <cstddef>

namespace our {
    void Physics::initialize(World* w) {
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

    void Physics::update(float dt) {
        debugDraw();

        if(!simulate || framesToSimulate == 0) {
            return;
        }

        framesToSimulate--;
        int i;

        dynamicsWorld->stepSimulation(dt);

        bulletToOur();

        editorPickObject();
    }

    void Physics::destroy() {

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

    void Physics::editorPickObject() {
        if(EntityDebugger::app->getMouse().justPressed(GLFW_MOUSE_BUTTON_1)) {
            EntityDebugger::pickObject(dynamicsWorld);
        }
    }

    // Reads data from bullet physics and updates object transform for graphics drawing
    void Physics::bulletToOur() {
        for(auto&& e : world->getEntitiesMut()) {
            if(!e->enabled) continue;
            if(auto rb = e->getComponent<RigidBody>()) {

                btTransform rbT;
                rb->body->getMotionState()->getWorldTransform(rbT);

                glm::vec3 newWorldTranslation = {rbT.getOrigin().x(), rbT.getOrigin().y(), rbT.getOrigin().z()};
                glm::quat newWorldRotation =
                    glm::quat(rbT.getRotation().w(), rbT.getRotation().x(), rbT.getRotation().y(), rbT.getRotation().z());

                e->localTransform.position = newWorldTranslation;
                e->localTransform.qRot     = newWorldRotation;
            }
        }
    }

    void Physics::debugDraw() const {

        auto pMat = EntityDebugger::editorCamera->getProjectionMatrix(EntityDebugger::app->getWindowSize());
        debugDrawer->SetMatrices(EntityDebugger::editorCamera->getViewMatrix(), pMat);

        // Draw what bullet physics sees
        dynamicsWorld->debugDrawWorld();
    }

    // Reads our data and sets bullet rigidbodies with it
    void Physics::ourToBullet() {
        for(auto e : world->getEntities()) {
            if(!e->enabled) continue;

            if(auto rb = e->getComponent<RigidBody>()) {
                // Can move both getWorldRotation/Translation to inside the method
                // but it segfaults for some reason.
                rb->syncWithTransform(e->getWorldRotation(), e->getWorldTranslation());
            }
        }
    }

    void Physics::onImmediateGui() {
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

} // namespace our