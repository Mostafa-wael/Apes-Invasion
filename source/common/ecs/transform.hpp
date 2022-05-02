#pragma once

#include "ecs/IImGuiDrawable.h"
#include "glm/fwd.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtc/quaternion.hpp"
#include "imgui.h"
#include <cstdio>
#include <glm/glm.hpp>
#include <json/json.hpp>
#include <string>

namespace our {

    // A transform defines the translation, rotation & scale of an object relative to its parent
    struct Transform : public IImGuiDrawable {
    public:
        glm::vec3 position = glm::vec3(0, 0, 0); // The position is defined as a vec3. (0,0,0) means no translation
        glm::vec3 scale    = glm::vec3(1, 1, 1); // The scale is defined as a vec3. (1,1,1) means no scaling.
        glm::quat qRot     = glm::quat();
        bool uniform       = false;
        bool changedInUI   = false;

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


            changedInUI |= ImGui::DragFloat3(pos.c_str(), &position.x, 0.1f);

            glm::vec3 rotation = getEulerRotation();
            changedInUI |= ImGui::DragFloat3(rot.c_str(), &rotation.x, 0.05, 0, glm::half_pi<float>());

            setEulerRotation(rotation.x, rotation.y, rotation.z);

            ImGui::Spacing();


            ImGui::Checkbox(uni.c_str(), &uniform);
            if(uniform) {
                changedInUI |= ImGui::DragFloat(scl.c_str(), &scale.x, 0.05, 0.1, 10);
                scale.y = scale.z = scale.x;
            } else {
                changedInUI |= ImGui::DragFloat3(scl.c_str(), &scale.x, 0.05, 0.1, 10);
            }

        }

        void setEulerRotation(float x, float y, float z) {
            qRot = glm::quat({x, y, z});
        }

        void setEulerRotation(glm::vec3 eRot) {
            qRot = glm::quat(eRot);
        }

        glm::vec3 getEulerRotation() {
            return glm::eulerAngles(qRot);
        }
    };

} // namespace our