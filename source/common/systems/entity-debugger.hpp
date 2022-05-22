#pragma once

#include "ecs/world.hpp"
#include "imgui.h"
#include <cstdlib>
namespace our {
    class EntityDebugger {
    public:
        static void update(World* world, float deltaTime) {
            if(ImGui::CollapsingHeader("Entities")) {

                // TODO: Scene saving when serialization is done
                // char filePath[256];
                // ImGui::InputText("Filepath", filePath, 256);
                // if (ImGui::Button("Save")) {

                // }

                if(ImGui::Button("Exit")){
                    exit(0);
                }

                for(auto entity : world->getEntities()) {
                    // Imgui treats elements with the same label/ID as one element
                    // To differentiate each entity, added an ID to the end of each element's name
                    // Imgui ignores the ##
                    entity->onImmediateGui();
                    ImGui::Separator();
                }
            }
        }
    };
} // namespace our