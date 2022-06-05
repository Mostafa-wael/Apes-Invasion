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
            ImGui::Begin("Health");
            ImGui::Text("Health: %d/%d", current_health, max_health);
            ImGui::SliderInt("health", &current_health, 0, max_health);
            ImGui::End();
        }
    };

} // namespace our