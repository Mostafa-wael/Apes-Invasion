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

    // This template method create a component of type T,
        // adds it to the components map and returns a pointer to it
        template <typename T>
        T* Entity::addComponent() {
            static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
            T* component           = new T();
            component->owner       = this;
            components[T::getID()] = component;
            return component;
        }

        // This template method searhes for a component of type T and returns a pointer to it
        // If no component of type T was found, it returns a nullptr
        template <typename T>
        T* Entity::getComponent() {
            static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
            if(auto it = components.find(static_cast<std::string>(T::getID())); it != components.end()) {
                return dynamic_cast<T*>(it->second);
            }
            return nullptr;
        }

        // This template method searhes for a component of type T and deletes it
        template <typename T>
        void Entity::deleteComponent() {
            static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
            if(auto it = components.find(static_cast<std::string>(T::getID())); it != components.end()) {
                delete it->second;
                components.erase(it);
            }
        }


    glm::mat4 Entity::getWorldRotation() const {
        Entity* currentParent  = parent;
        glm::mat4 localToWorld = glm::toMat4(localTransform.qRot);
        while(currentParent) {
            localToWorld  = glm::toMat4(currentParent->localTransform.qRot) * localToWorld;
            currentParent = currentParent->parent;
        }
        return localToWorld;
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
            if(ImGui::TreeNode((iter->second->getIDPolymorphic() + "##" + id).c_str())) {

                iter->second->onImmediateGui();

                ImGui::TreePop();
            }
        }
    }

} // namespace our