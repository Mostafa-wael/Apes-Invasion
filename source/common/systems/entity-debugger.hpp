#pragma once

#include "ecs/world.hpp"
#include "imgui.h"
namespace our {
    class EntityDebugger {
        char entityName[128];

    public:
        void update(World* world, float deltaTime) {
            ImGui::Begin("Entities");
            ImGui::Text("Current frametime: %f", deltaTime);

            ImGui::InputText("Entity name", entityName, 128);
            ImGui::SameLine();
            if(ImGui::Button("Add entity"))
                addEntity(entityName, world);

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