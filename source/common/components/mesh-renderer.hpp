#pragma once

#include "../asset-loader.hpp"
#include "../ecs/component.hpp"
#include "../material/material.hpp"
#include "../mesh/mesh.hpp"
#include "ecs/IImGuiDrawable.h"
#include "imgui.h"
#include <string>

namespace our {

    // This component denotes that any renderer should draw the given mesh using the given material at the transformation of the owning entity.
    class MeshRendererComponent : public Component {
    public:
        Mesh* mesh;         // The mesh that should be drawn
        Material* material; // The material used to draw the mesh

        // The ID of this component type is "Mesh Renderer"
        static std::string getID() { return "Mesh Renderer"; }

        // Receives the mesh & material from the AssetLoader by the names given in the json object
        void deserialize(const nlohmann::json& data) override;

        virtual void onImmediateGui() override {
            std::string meshRendHeaderID = std::to_string((long long)&mesh) + std::to_string((long long)&material);
            std::string meshRendLabel    = "Mesh Renderer ##" + meshRendHeaderID;
            if(ImGui::CollapsingHeader(meshRendLabel.c_str())) {
                ImGui::Indent(10);
                mesh->onImmediateGui();
                material->onImmediateGui();
                ImGui::Indent(-10);
            }
        }
    };
} // namespace our