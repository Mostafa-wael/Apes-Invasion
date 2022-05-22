#pragma once

#include "../asset-loader.hpp"
#include "../ecs/component.hpp"
#include "../material/material.hpp"
#include "../mesh/mesh.hpp"
#include "ecs/IImGuiDrawable.h"
#include "imgui.h"
#include "util.h"
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace our {

    // This component denotes that any renderer should draw the given mesh using the given material at the transformation of the owning entity.
    class MeshRendererComponent : public Component {
    public:
        Mesh* mesh         = nullptr; // The mesh that should be drawn
        Material* material = nullptr; // The material used to draw the mesh
        int currMesh       = -1;
        int currMat        = -1;

        // The ID of this component type is "Mesh Renderer"
        static std::string
        getID() { return "Mesh Renderer"; }

        virtual std::string getIDPolymorphic() override { return getID(); }

        // Receives the mesh & material from the AssetLoader by the names given in the json object
        void deserialize(const nlohmann::json& data) override;

        virtual void onImmediateGui() override {
            if((mesh = editorModifyComponentAsset<Mesh>(mesh, currMesh, "Loaded meshes")))
                mesh->onImmediateGui();
            if((material = editorModifyComponentAsset<Material>(material, currMat,"Loaded materials")))
                material->onImmediateGui();
        }
    };
} // namespace our