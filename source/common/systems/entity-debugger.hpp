#pragma once

#include "BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"
#include "BulletDynamics/Dynamics/btSimpleDynamicsWorld.h"

#include "ImGuizmo.h"
#include "application.hpp"
#include "components/camera.hpp"
#include "components/health.hpp"
#include "components/rigidbody.hpp"

#include "ecs/entity.hpp"
#include "ecs/world.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "imgui.h"
#include "systems/physics.hpp"
#include <cstring>
#include <iostream>

namespace our {
    class EntityDebugger {
        static inline char entityName[128];
        static inline float moveToObjDist = 10.0f;
        static inline bool wireframe      = false;
        static inline bool enabled        = false;

    public:
        static inline Application* app;
        static inline Entity* selectedEntity;
        static inline CameraComponent* editorCamera;
        static inline PhysicsSystem* physicsSystem;
        static inline float objectPickDist = 1000;

        static void init(CameraComponent* camera, Application* a, PhysicsSystem* phys) {
            editorCamera  = camera;
            app           = a;
            physicsSystem = phys;
        }

        static void update(World* world, float deltaTime) {
            if(app->getKeyboard().justPressed(GLFW_KEY_TAB)) {
                enabled = !enabled;
            }

            if(!enabled) {
                app->getMouse().lockMouse(app->getWindow());
                return;
            } else {
                app->getMouse().unlockMouse(app->getWindow());
            }

            ImGui::Begin("Help", NULL, ImGuiWindowFlags_HorizontalScrollbar);
            ImGui::BulletText("Press WASD to move the camera, QE to move down/up");
            ImGui::BulletText("Press and hold the right mouse button to look around");
            ImGui::BulletText("Click on a physics object to select it (you should see axes on the object's pivot)");
            ImGui::BulletText("With a selected object:");
            ImGui::Indent(10);
            ImGui::BulletText("press T to translate it, R to rotate it, and Y to scale it");
            ImGui::BulletText("Press F to teleport near that object");
            ImGui::Indent(-10);
            ImGui::BulletText("Note that objects with rigidbodies do not support scaling, you'll only scale the mesh, not the collision");
            ImGui::BulletText("Press tab to toggle the entity debugger");
            ImGui::End();

            ImGui::Begin("Entities");
            ImGui::Text("Current frametime: %f", deltaTime);

            // Compute button width to span the whole ImGui window
            float item_spacing = ImGui::GetStyle().ItemSpacing.x, window_width = ImGui::GetWindowWidth();
            float full_button_width = window_width - 2 * item_spacing;

            // Another way to set item width if there's no parameter for it
            ImGui::PushItemWidth(full_button_width);
            ImGui::InputTextWithHint("##1297129387", "Entity name", entityName, 128);
            ImGui::PopItemWidth();

            // Draw a green button
            ImGui::PushStyleColor(ImGuiCol_Button, {0.25, 0.65, 0.15, 1});
            if(ImGui::Button("Add entity", {full_button_width, 0}) && std::strlen(entityName) > 0)
                addEntity(entityName, world);
            ImGui::PopStyleColor();

            ImGui::Spacing();

            // TODO: Scene saving when serialization is done
            // char filePath[256];
            // ImGui::InputText("Filepath", filePath, 256);
            // if (ImGui::Button("Save")) {
            // }

            for(auto entity : world->getEntities()) {
                if(!entity->enabled || entity->getParent() != nullptr) continue;
                // Imgui treats elements with the same label/ID as one element
                // To differentiate each entity, added an ID to the end of each element's name
                // Imgui ignores the ##

                entity->onImmediateGui();

                ImGui::Separator();
            }

            ImGui::End();

            ImGui::Begin("Chosen entity ");

            if(selectedEntity) {

                if(app->getKeyboard().justPressed(GLFW_KEY_F)) {
                    moveToSelectedObject();
                }

                ImGuizmo::BeginFrame();

                glm::mat4 selectedEntityLocalTransform = selectedEntity->localTransform.toMat4();
                EditTransform(editorCamera, &selectedEntityLocalTransform, app);
                glm::vec3 scale, translation, skew;
                glm::quat orein;
                glm::vec4 perspective;
                glm::decompose(selectedEntityLocalTransform, scale, orein, translation, skew, perspective);

                selectedEntity->localTransform.position    = translation;
                selectedEntity->localTransform.qRot        = orein;
                selectedEntity->localTransform.scale       = scale;
                selectedEntity->localTransform.changedInUI = true;

                if(auto rb = selectedEntity->getComponent<RigidBody>()) {
                    rb->syncWithTransform(selectedEntity->getWorldRotation(), selectedEntity->getWorldTranslation(), true);
                }

                std::string id = std::to_string((long long)selectedEntity);

                selectedEntity->drawComponentAdder(id);

                selectedEntity->drawTransform(id);

                selectedEntity->ourToBullet();

                selectedEntity->drawComponents(id);
            }

            ImGui::End();

            if(physicsSystem)
                physicsSystem->onImmediateGui();
        }

        static void addEntity(char* entityName, World* world) {
            auto e  = world->add();
            e->name = std::string(entityName);
        }

        static void EditTransform(const CameraComponent* camera, glm::mat4* matrix, Application* app) {
            static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
            static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);

            if(app->getKeyboard().isPressed(GLFW_KEY_T))
                mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
            if(app->getKeyboard().isPressed(GLFW_KEY_R))
                mCurrentGizmoOperation = ImGuizmo::ROTATE;
            if(app->getKeyboard().isPressed(GLFW_KEY_Y))
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
                glm::value_ptr(camera->getProjectionMatrix({app->getWindowSize().x, app->getWindowSize().y})),
                mCurrentGizmoOperation, mCurrentGizmoMode,
                glm::value_ptr(*matrix),
                NULL,
                NULL /* useSnap  ? &snap.x : NULL */);
        }

        static void moveToSelectedObject() {
            glm::vec3 selectedEntityPos = selectedEntity->getWorldTranslation();
            glm::vec3 editorCameraPos   = editorCamera->getOwner()->getWorldTranslation();

            glm::vec3 objToCam = editorCameraPos - selectedEntityPos;
            objToCam           = glm::normalize(objToCam);

            // Assuming the editor camera is not a child of anything
            editorCameraPos                                   = selectedEntityPos + objToCam * moveToObjDist;
            editorCamera->getOwner()->localTransform.position = editorCameraPos;
        }

        static void toggleWireframeRender() {
            wireframe = !wireframe;
            if(wireframe) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            } else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }

        static void pickObject(btDiscreteDynamicsWorld* dynamicsWorld) {

            // https://antongerdelan.net/opengl/raycasting.html
            glm::vec2 mousePos     = EntityDebugger::app->getMouse().getMousePosition();
            glm::vec2 framBuffSize = EntityDebugger::app->getWindowSize();

            float x           = (2.0f * mousePos.x) / framBuffSize.x - 1.0f;
            float y           = 1.0f - (2.0f * mousePos.y) / framBuffSize.y;
            float z           = 1.0f;
            glm::vec3 ray_nds = {x, y, z};

            glm::vec4 ray_clip = {ray_nds.x, ray_nds.y, -1.0, 1.0};
            glm::vec4 ray_eye  = inverse(EntityDebugger::editorCamera->getProjectionMatrix(framBuffSize)) * ray_clip;
            ray_eye            = {ray_eye.x, ray_eye.y, -1.0, 0.0};

            glm::vec3 ray_wor = inverse(EntityDebugger::editorCamera->getViewMatrix()) * ray_eye;

            glm::vec3 rayStart = EntityDebugger::editorCamera->getOwner()->getWorldTranslation();
            glm::vec3 rayEnd   = rayStart + ray_wor * objectPickDist;

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

                Entity* closest = nullptr;
                float minDist   = INFINITY;

                for(int i = 0; i < allResults.m_hitPointWorld.size(); i++) {
                    btVector3 currHitPoint = allResults.m_hitPointWorld.at(i);

                    dynamicsWorld->getDebugDrawer()->drawSphere(currHitPoint, 1, btVector3(1, 0, 0));

                    float currDist = currHitPoint.distance2(from);
                    if(currDist < minDist) {
                        closest = static_cast<Entity*>(allResults.m_collisionObjects[0]->getUserPointer());
                        minDist = currDist;
                    }
                }

                EntityDebugger::selectedEntity = closest;
            } else {
                EntityDebugger::selectedEntity = nullptr;
            }
        }
    };
} // namespace our