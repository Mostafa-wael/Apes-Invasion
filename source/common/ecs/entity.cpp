#include "entity.hpp"
#include "../components/component-deserializer.hpp"
#include "../deserialize-utils.hpp"
#include "glm/fwd.hpp"
#include "glm/gtx/quaternion.hpp"
#include "imgui.h"
#include <glm/gtx/euler_angles.hpp>
#include "systems/entity-debugger.hpp"

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

            for(auto c : children) {
                c->onImmediateGui();
            }

            ImGui::Indent(-10);
            ImGui::TreePop();
        }

        if(ImGui::IsItemClicked()) {
            EntityDebugger::selectedEntity = this;
        }
    }

    glm::mat4 Entity::getWorldRotation() const {
        Entity* currentParent  = parent;
        glm::quat localToWorld = localTransform.qRot;
        while(currentParent) {
            localToWorld  = currentParent->localTransform.qRot * localToWorld;
            currentParent = currentParent->parent;
        }
        return glm::toMat4(localToWorld);
    }

    glm::vec3 Entity::getWorldScale() const {
        Entity* currentParent  = parent;
        glm::vec3 localToWorld = localTransform.scale;
        while(currentParent) {
            localToWorld  = currentParent->localTransform.scale * localToWorld;
            currentParent = currentParent->parent;
        }
        return localToWorld;
    }

    glm::vec3 Entity::getWorldTranslation() const {
        Entity* currentParent  = parent;
        glm::vec3 localToWorld = localTransform.position;
        while(currentParent) {
            localToWorld  = currentParent->localTransform.position + localToWorld;
            currentParent = currentParent->parent;
        }
        return localToWorld;
    }

    typedef std::unordered_map<std::string, Component*>::const_iterator CompMapConstIter;
    std::pair<CompMapConstIter, CompMapConstIter>
    Entity::getComponentsIter() const {
        return {components.begin(), components.end()};
    }

    glm::vec3 Entity::getForward() const {
        return glm::vec3(getWorldRotation() * glm::vec4(0, 0, -1, 1));
    }

    glm::vec3 Entity::getRight() const {
        return glm::vec3(getWorldRotation() * glm::vec4(1, 0, 0, 1));
    }

    glm::vec3 Entity::getUp() const {
        return glm::vec3(getWorldRotation() * glm::vec4(0, 1, 0, 1));
    }

    void Entity::drawComponentAdder(std::string id) {
        ImGui::Combo(("##" + id).c_str(), &currComponent, componentsToAdd, 3);
        ImGui::SameLine();
        if(ImGui::Button(("Add component##" + id).c_str())) {
            if(std::strcmp(componentsToAdd[currComponent], "Mesh Renderer") == 0)
                addComponent<MeshRendererComponent>();
            // TODO: Implement logic for adding other components
        }
    }

    void Entity::drawTransform(std::string id) {
        if(ImGui::TreeNode(("Transform##" + id).c_str())) {

            localTransform.onImmediateGui();

            ImGui::TreePop();
        }
    }

    // Synnchronizes bullet rigidbodies to our data when we update something in the entity list
    void Entity::ourToBullet() {
        if(auto rb = getComponent<RigidBody>(); localTransform.changedInUI && rb) {
            rb->syncWithTransform(getWorldRotation(), getWorldTranslation());
            localTransform.changedInUI = false;
        }
    }

    void Entity::drawComponents(std::string id) {
        auto [compsBegin, compsEnd] = getComponentsIter();

        for(auto iter = compsBegin; iter != compsEnd; iter++) {
            // ImGui::SetNextItemOpen(true);
            if(ImGui::TreeNodeEx((iter->second->getIDPolymorphic() + "##" + id).c_str(),ImGuiTreeNodeFlags_DefaultOpen)) {

                iter->second->onImmediateGui();

                ImGui::TreePop();
            }
        }
    }

    void Entity::setParent(Entity* p) {
        // Remove this as a child of the old parent
        if(parent) {
            parent->children.erase(this);
        }

        parent = p;

        // Set this as the child of the new parent
        if(p) {
            p->children.insert(this);
        }
    }

    Entity* Entity::getParent() {
        return parent;
    }

} // namespace our