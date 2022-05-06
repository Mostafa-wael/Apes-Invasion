#include "physics.hpp"
#include "ImGuizmo.h"
#include "ecs/entity.hpp"
#include "input/keyboard.hpp"

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

} // namespace our