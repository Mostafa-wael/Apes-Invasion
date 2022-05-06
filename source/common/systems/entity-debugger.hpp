#pragma once

#include "ecs/world.hpp"
#include "imgui.h"
#include <cstring>
namespace our {
    class EntityDebugger {
        char entityName[128];

    public:
        void update(World* world, float deltaTime) {
            ImGui::Begin("Entities");
            ImGui::Text("Current frametime: %f", deltaTime);

            ImGui::InputText("Entity name", entityName, 128);

            float item_spacing = ImGui::GetStyle().ItemSpacing.x, window_width = ImGui::GetWindowWidth();
            float full_button_width = window_width - 2 * item_spacing;

            if(ImGui::Button("Add entity", {full_button_width, 0}) && std::strlen(entityName) > 0)
                addEntity(entityName, world);

            ImGui::Spacing();

            // TODO: Scene saving when serialization is done
            // char filePath[256];
            // ImGui::InputText("Filepath", filePath, 256);
            // if (ImGui::Button("Save")) {
            // }

            for(auto entity : world->getEntities()) {
                if(!entity->enabled) continue;
                // Imgui treats elements with the same label/ID as one element
                // To differentiate each entity, added an ID to the end of each element's name
                // Imgui ignores the ##

                entity->onImmediateGui();

                ImGui::Separator();
            }

            ImGui::End();
        }

        void addEntity(char* entityName, World* world) {
            auto e  = world->add();
            e->name = std::string(entityName);
        }
    };
} // namespace our