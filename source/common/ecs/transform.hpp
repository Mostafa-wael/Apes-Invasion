#pragma once

#include "ecs/IImGuiDrawable.h"
#include "glm/gtc/constants.hpp"
#include "imgui.h"
#include <glm/glm.hpp>
#include <json/json.hpp>
#include <string>

namespace our {

    // A transform defines the translation, rotation & scale of an object relative to its parent
    struct Transform : public IImGuiDrawable {
    public:
        glm::vec3 position = glm::vec3(0, 0, 0); // The position is defined as a vec3. (0,0,0) means no translation
        glm::vec3 rotation = glm::vec3(0, 0, 0); // The rotation is defined using euler angles (y: yaw, x: pitch, z: roll). (0,0,0) means no rotation
        glm::vec3 scale    = glm::vec3(1, 1, 1); // The scale is defined as a vec3. (1,1,1) means no scaling.
        bool uniform       = false;

        // This function computes and returns a matrix that represents this transform
        glm::mat4 toMat4() const;
        // Deserializes the entity data and components from a json object
        void deserialize(const nlohmann::json&);

        void onImmediateGui() override {
            std::string transformId = std::to_string((long long)(this));
            std::string pos         = "Position ##" + transformId;
            std::string rot         = "Rotation ##" + transformId;
            std::string scl         = "Scale    ##" + transformId;
            std::string uni         = "Uniform scaling ##" + transformId;

            ImGui::DragFloat3(pos.c_str(), &position.x, 0.1f);
            ImGui::DragFloat3(rot.c_str(), &rotation.x, 0.05, 0, glm::half_pi<float>());

            if(uniform) {
                ImGui::DragFloat(scl.c_str(), &scale.x, 0.05, 0.1, 5);
                scale.y = scale.z = scale.x;
            } else {
                ImGui::DragFloat3(scl.c_str(), &scale.x, 0.05, 0.1, 5);
            }
            ImGui::SameLine();
            ImGui::Checkbox(uni.c_str(), &uniform);
        }
    };

} // namespace our