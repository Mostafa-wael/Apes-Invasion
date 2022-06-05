#pragma once

#include "../ecs/component.hpp"
#include "imgui.h"

#include <glm/glm.hpp>

namespace our {
    class HealthComponent : public Component {
    public:
        int max_health     = 100;
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
            ImGui::Begin("Health", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove);
            ImGui::ProgressBar((current_health / (max_health * 1.0)));
            ImGui::End();
        }
    };

} // namespace our