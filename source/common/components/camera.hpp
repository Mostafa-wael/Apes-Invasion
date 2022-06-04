#pragma once

#include "../ecs/component.hpp"
#include "ecs/IImGuiDrawable.h"
#include "glm/gtc/constants.hpp"
#include "imgui.h"

#include <glm/mat4x4.hpp>
#include <string>

namespace our {

    // An enum that defines the type of the camera (ORTHOGRAPHIC or PERSPECTIVE)
    enum class CameraType {
        ORTHOGRAPHIC,
        PERSPECTIVE
    };

    // This component denotes that any renderer should draw the scene relative to this camera.
    // We do not define the eye, center or up here since they can be extracted from the entity local to world matrix
    class CameraComponent : public Component {
    public:
        CameraType cameraType; // The type of the camera
        float near, far;       // The distance from the camera center to the near and far plane
        float fovY;            // The field of view angle of the camera if it is a perspective camera
        float orthoHeight;     // The orthographic height of the camera if it is an orthographic camera

        // The ID of this component type is "Camera"
        static std::string getID() { return "Camera"; }
        virtual std::string getIDPolymorphic() override { return getID(); }

        // Reads camera parameters from the given json object
        void deserialize(const nlohmann::json& data) override;

        // Creates and returns the camera view matrix
        glm::mat4 getViewMatrix() const;

        // Creates and returns the camera projection matrix
        // "viewportSize" is used to compute the aspect ratio
        glm::mat4 getProjectionMatrix(glm::ivec2 viewportSize) const;

        virtual void onImmediateGui() override {
            ImGui::LabelText("", "Camera type: %s", cameraType == CameraType::PERSPECTIVE ? "Perspective" : "Orthographic");

            // Note that near, far are consecutive in memory, this should read/write them both properly.
            ImGui::DragFloat("Near", &near, 0.5, 0, 10);
            ImGui::DragFloat("Far", &far, 0.5, 10, 100);
            ImGui::DragFloat("FOV", &fovY, 0.05, 0, glm::pi<float>());

            if(cameraType == CameraType::ORTHOGRAPHIC) ImGui::LabelText("", "Orthographic height: %f", orthoHeight);
        }
    };

} // namespace our