#include "entity.hpp"
#include "../components/component-deserializer.hpp"
#include "../deserialize-utils.hpp"
#include "glm/fwd.hpp"
#include "imgui.h"

#include <glm/gtx/euler_angles.hpp>

namespace our {

    // This function returns the transformation matrix from the entity's local space to the world space
    // Remember that you can get the transformation matrix from this entity to its parent from "localTransform"
    // To get the local to world matrix, you need to combine this entities matrix with its parent's matrix and
    // its parent's parent's matrix and so on till you reach the root.
    glm::mat4 Entity::getLocalToWorldMatrix() const {
        //TODO: (Req 7) Write this function
        Entity* currentParent  = parent;
        glm::mat4 localToWorld = localTransform.toMat4();
        while(currentParent) {
            localToWorld  = parent->localTransform.toMat4() * localToWorld;
            currentParent = currentParent->parent;
        }
        return localToWorld;
    }

    // Deserializes the entity data and components from a json object
    void Entity::deserialize(const nlohmann::json& data) {
        if(!data.is_object()) return;
        name = data.value("name", name);
        localTransform.deserialize(data);
        if(data.contains("components")) {
            if(const auto& components = data["components"]; components.is_array()) {
                for(auto& component : components) {
                    deserializeComponent(component, this);
                }
            }
        }
    }

    void Entity::onImmediateGui() {
        if(ImGui::TreeNode((name + "##" + std::to_string((long long)this)).c_str())) {
            ImGui::Indent(10);

            // Brute force solution
            // TODO: figre out a better way to display child hierarchies.
            for(auto e : world->getEntities()) {
                if(e->parent == this) e->onImmediateGui();
            }
            ImGui::Indent(-10);
            ImGui::TreePop();
        }

        if(ImGui::IsItemClicked()) {
            selectedEntity = this;
        }
    }

} // namespace our