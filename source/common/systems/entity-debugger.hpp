#pragma once

#include "ecs/entity.hpp"
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

            if(Entity::selectedEntity) {
                std::string id = std::to_string((long long)Entity::selectedEntity);

                Entity::selectedEntity->drawComponentAdder(id);

                Entity::selectedEntity->drawTransform(id);

                Entity::selectedEntity->ourToBullet();

                Entity::selectedEntity->drawComponents(id);
            }
            
            ImGui::End();
        }

        void addEntity(char* entityName, World* world) {
            auto e  = world->add();
            e->name = std::string(entityName);
        }
    };
} // namespace our