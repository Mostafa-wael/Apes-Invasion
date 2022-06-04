#pragma once

#include "../ecs/component.hpp"
#include "imgui.h"

#include <glm/glm.hpp>

namespace our {
    class HealthComponent : public Component {
    public:
        int max_health = 100;
        int current_health = 100;
        bool changedInUI   = false;
        

        static std::string getID() { return "Health"; }

        std::string getIDPolymorphic() override {
            return getID();
        }

        void deserialize(const nlohmann::json& data) override;

        void heal(int heal_amout);
        void damage(int damage_amout);

            virtual void onImmediateGui() override {
                std::string healthId = std::to_string((long long)(this));
                std::string max         = "maximum health ##" + healthId;
                std::string cur         = "current health ##" + healthId;
                changedInUI |= ImGui::DragInt(cur.c_str(), &current_health);
                changedInUI |= ImGui::DragInt(max.c_str(), &max_health);
                ImGui::SliderInt("health", &current_health, 0, max_health);
        }
    };

}