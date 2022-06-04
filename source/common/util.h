#pragma once

#include "asset-loader.hpp"
#include "imgui.h"
#include <string>
#include <vector>
#include "btBulletDynamicsCommon.h"
#include "glm/glm.hpp"

namespace our {
    std::vector<char> stringsToImGuiComboList(std::vector<std::string> strings);

    // Used to set component values for components with assets as their data
    // Meshes, materials, etc.
    template <typename T>
    T* editorModifyComponentAsset(T* curr, int& currSelection, std::string guiLabel) {
        const std::vector<std::string> loadedMeshes = AssetLoader<T>::getLoadedAssetsNames();
        std::vector<char> ImGuiComboList            = our::stringsToImGuiComboList(loadedMeshes);
        std::string label                           = guiLabel + "##" + std::to_string((long long)curr);
        T* ret                                      = curr;

        if(curr) {
            if(currSelection == -1) {
                auto loadedMeshesPtrs = AssetLoader<T>::getLoadedAssetsValues();
                auto foundIter        = std::find(loadedMeshesPtrs.cbegin(), loadedMeshesPtrs.cend(), curr);
                currSelection         = foundIter - loadedMeshesPtrs.begin();
            }

            // If condition executes if menu option chosen
            if(ImGui::Combo(label.c_str(), &currSelection, ImGuiComboList.data()))
                ret = AssetLoader<T>::get(loadedMeshes[currSelection]);
        } else {
            currSelection = 0; // Arbitrary choice for new components
            ImGui::Combo(label.c_str(), &currSelection, ImGuiComboList.data());
            ret = AssetLoader<T>::get(loadedMeshes[currSelection]);
        }

        return ret;
    }

    btVector3 glmToBtVector3(glm::vec3 glmVec);
} // namespace our