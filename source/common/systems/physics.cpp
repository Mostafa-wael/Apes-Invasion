#include "physics.hpp"
#include "ImGuizmo.h"
#include "ecs/entity.hpp"
#include "input/keyboard.hpp"

#include "physics.hpp"

namespace our {
    void Physics::EditTransform(const CameraComponent* camera, glm::mat4* matrix, glm::vec2 windowSize, Keyboard* kb) {
        static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
        static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);

        if(kb->isPressed(GLFW_KEY_T))
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        if(kb->isPressed(GLFW_KEY_R))
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
        if(kb->isPressed(GLFW_KEY_E)) // r Key
            mCurrentGizmoOperation = ImGuizmo::SCALE;

        if(ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if(ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
        ImGui::SameLine();
        if(ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
            mCurrentGizmoOperation = ImGuizmo::SCALE;

        // float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        // ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(*matrix), matrixTranslation, matrixRotation, matrixScale);
        // ImGui::InputFloat3("Tr", matrixTranslation, 3);
        // ImGui::InputFloat3("Rt", matrixRotation, 3);
        // ImGui::InputFloat3("Sc", matrixScale, 3);
        // ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, glm::value_ptr(*matrix));

        if(mCurrentGizmoOperation != ImGuizmo::SCALE) {
            if(ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
                mCurrentGizmoMode = ImGuizmo::LOCAL;
            ImGui::SameLine();
            if(ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
                mCurrentGizmoMode = ImGuizmo::WORLD;
        }

        // static bool useSnap(false);
        // if(ImGui::IsKeyPressed(83))
        //     useSnap = !useSnap;

        // ImGui::Checkbox("", &useSnap);
        ImGui::SameLine();
        glm::vec3 snap;

        // Config config{int(windowSize.x), int(windowSize.y), false};
        // switch(mCurrentGizmoOperation) {
        // case ImGuizmo::TRANSLATE:
        //     // snap = config.mSnapTranslation;
        //     ImGui::InputFloat3("Snap", &snap.x);
        //     break;
        // case ImGuizmo::ROTATE:
        //     // snap = config.mSnapRotation;
        //     ImGui::InputFloat("Angle Snap", &snap.x);
        //     break;
        // case ImGuizmo::SCALE:
        //     // snap = config.mSnapScale;
        //     ImGui::InputFloat("Scale Snap", &snap.x);
        //     break;
        // }

        ImGuiIO& io = ImGui::GetIO();

        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
        ImGuizmo::Manipulate(
            glm::value_ptr(camera->getViewMatrix()),
            glm::value_ptr(camera->getProjectionMatrix({windowSize.x, windowSize.y})),
            mCurrentGizmoOperation, mCurrentGizmoMode,
            glm::value_ptr(*matrix),
            NULL,
            NULL /* useSnap  ? &snap.x : NULL */);
    }

    void Physics::initialize(World* w, Application* a, CameraComponent* cam) {
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

    void Physics::update(float dt) {
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

    void Physics::rayCast() {
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
            float dist        = 1000;

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
                hit             = true;
                Entity* closest = nullptr;
                glm::mat4 closestTransform;
                float minDist = INFINITY;

                for(int i = 0; i < allResults.m_hitPointWorld.size(); i++) {
                    dynamicsWorld->getDebugDrawer()->drawSphere(allResults.m_hitPointWorld.at(i), 1, btVector3(1, 0, 0));
                    float currDist = allResults.m_hitPointWorld.at(i).distance2(from);
                    if(currDist < minDist) {
                        closest          = static_cast<Entity*>(allResults.m_collisionObjects[0]->getUserPointer());
                        closestTransform = closest->localTransform.toMat4();
                        minDist          = currDist;
                    }
                }

                hitEntity          = closest;
                hitEntityTransform = closestTransform;
            } else {
                hit = false;
            }
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
        if(hit) {
            ImGuizmo::BeginFrame();
            EditTransform(camera, &hitEntityTransform, app->getWindowSize(), &app->getKeyboard());
            glm::vec3 scale, translation, skew;
            glm::quat orein;
            glm::vec4 perspective;
            glm::decompose(hitEntityTransform, scale, orein, translation, skew, perspective);

            hitEntity->localTransform.position    = translation;
            hitEntity->localTransform.qRot        = orein;
            hitEntity->localTransform.scale       = scale;
            hitEntity->localTransform.changedInUI = true;

            Entity::selectedEntity = hitEntity;

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

} // namespace our