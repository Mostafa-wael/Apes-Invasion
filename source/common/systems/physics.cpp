#include "physics.hpp"
#include "../components/rigidbody.hpp"
#include "LinearMath/btVector3.h"
#include "ecs/entity.hpp"
#include "input/keyboard.hpp"
#include "physics.hpp"
#include "systems/entity-debugger.hpp"
#include <cstddef>

namespace our {
    void PhysicsSystem::initialize(World* w) {
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

        for(auto&& e : world->getEntities()) {
            if(auto rb = e->getComponent<RigidBody>()) {
                dynamicsWorld->addRigidBody(rb->bulletRB);

                // Need a refrence to the physics system so we can remove the rigidbody on its destruction
                rb->init(this);
            }
        }
    }

    void PhysicsSystem::update(float dt) {

        if(drawDebug)
            debugDraw();

        if(!simulate)
            return;

        dynamicsWorld->stepSimulation(dt);

        collisionCallbacks();

        bulletToOur();

        editorPickObject();
    }

    void PhysicsSystem::destroy() {

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

    void PhysicsSystem::editorPickObject() {
        if(EntityDebugger::app->getMouse().justPressed(GLFW_MOUSE_BUTTON_1)) {
            EntityDebugger::pickObject(dynamicsWorld);
        }
    }

    // Reads data from bullet physics and updates object transform for graphics drawing
    void PhysicsSystem::bulletToOur() {
        for(auto&& e : world->getEntitiesMut()) {
            if(!e->enabled) continue;
            if(auto rb = e->getComponent<RigidBody>()) {

                btTransform rbT;
                rb->bulletRB->getMotionState()->getWorldTransform(rbT);

                glm::vec3 newWorldTranslation = {rbT.getOrigin().x(), rbT.getOrigin().y(), rbT.getOrigin().z()};
                glm::quat newWorldRotation =
                    glm::quat(rbT.getRotation().w(), rbT.getRotation().x(), rbT.getRotation().y(), rbT.getRotation().z());

                e->localTransform.position = newWorldTranslation;
                e->localTransform.qRot     = newWorldRotation;
            }
        }
    }

    void PhysicsSystem::debugDraw() const {

        auto pMat = EntityDebugger::editorCamera->getProjectionMatrix(EntityDebugger::app->getWindowSize());
        debugDrawer->SetMatrices(EntityDebugger::editorCamera->getViewMatrix(), pMat);

        // Draw what bullet physics sees
        dynamicsWorld->debugDrawWorld();
    }

    // Reads our data and sets bullet rigidbodies with it
    void PhysicsSystem::ourToBullet() {
        for(auto e : world->getEntities()) {
            if(!e->enabled) continue;

            if(auto rb = e->getComponent<RigidBody>()) {
                // Can move both getWorldRotation/Translation to inside the method
                // but it segfaults for some reason.
                rb->syncWithTransform(e->getWorldRotation(), e->getWorldTranslation());
            }
        }
    }

    void PhysicsSystem::onImmediateGui() {
        ImGui::Begin("Physics");
        ImGui::Indent(10);

        ImGui::Checkbox("Debug drawing enabled", &drawDebug);

        if(ImGui::Checkbox("simulate", &simulate) && simulate) {
            ourToBullet();
        }

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

    void PhysicsSystem::collisionCallbacks() {
        btDispatcher* dp       = dynamicsWorld->getDispatcher();
        const int numManifolds = dp->getNumManifolds();

        for(int m = 0; m < numManifolds; ++m) {

            btPersistentManifold* man = dp->getManifoldByIndexInternal(m);
            const btRigidBody* obA    = static_cast<const btRigidBody*>(man->getBody0());
            const btRigidBody* obB    = static_cast<const btRigidBody*>(man->getBody1());

            Entity* entityA = static_cast<Entity*>(obA->getUserPointer());
            Entity* entityB = static_cast<Entity*>(obB->getUserPointer());

            if(entityA->enabled && entityB->enabled) {
                RigidBody* aRB = entityA->getComponent<RigidBody>();
                RigidBody* bRB = entityB->getComponent<RigidBody>();

                if(aRB && aRB->onCollision)
                    aRB->onCollision(bRB);

                if(bRB && bRB->onCollision)
                    bRB->onCollision(aRB);
            }
        }
    }
} // namespace our