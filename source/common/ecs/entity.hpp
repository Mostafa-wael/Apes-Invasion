#pragma once

#include "LinearMath/btQuaternion.h"
#include "LinearMath/btTransform.h"
#include "component.hpp"
#include "components/mesh-renderer.hpp"
#include "ecs/IImGuiDrawable.h"
#include "ecs/rigidbody.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/quaternion.hpp"
#include "imgui.h"
#include "transform.hpp"
#include <cstdio>
#include <cstring>
#include <glm/glm.hpp>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace our {

    class World; // A forward declaration of the World Class

    class Entity : public IImGuiDrawable {
        World* world;                                           // This defines what world own this entity
        std::unordered_map<std::string, Component*> components; // A map of components that are owned by this entity
                                                                // The key is the ID of the component so an entity can only have one component of each type

        friend World;       // The world is a friend since it is the only class that is allowed to instantiate an entity
        Entity() = default; // The entity constructor is private since only the world is allowed to instantiate an entity
    public:
        std::string name;         // The name of the entity. It could be useful to refer to an entity by its name
        Entity* parent;           // The parent of the entity. The transform of the entity is relative to its parent.
                                  // If parent is null, the entity is a root entity (has no parent).
        Transform localTransform; // The transform of this entity relative to its parent.

        bool enabled = true;

        const char* componentsToAdd[3] = {
            "Mesh Renderer",
            "Rigid Body",
            "Shooter"};

        int currComponent = 0;
        static inline Entity* selectedEntity;

        World* getWorld() const { return world; } // Returns the world to which this entity belongs

        glm::mat4 getLocalToWorldMatrix() const; // Computes and returns the transformation from the entities local space to the world space
        void deserialize(const nlohmann::json&); // Deserializes the entity data and components from a json object

        // This template method create a component of type T,
        // adds it to the components map and returns a pointer to it
        template <typename T>
        T* addComponent() {
            static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
            T* component           = new T();
            component->owner       = this;
            components[T::getID()] = component;
            return component;
        }

        // This template method searhes for a component of type T and returns a pointer to it
        // If no component of type T was found, it returns a nullptr
        template <typename T>
        T* getComponent() {
            static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
            if(auto it = components.find(static_cast<std::string>(T::getID())); it != components.end()) {
                return dynamic_cast<T*>(it->second);
            }
            return nullptr;
        }

        // This template method searhes for a component of type T and deletes it
        template <typename T>
        void deleteComponent() {
            static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");
            if(auto it = components.find(static_cast<std::string>(T::getID())); it != components.end()) {
                delete it->second;
                components.erase(it);
            }
        }

        // Since the entity owns its components, they should be deleted alongside the entity
        ~Entity() {
            for(auto& it : components) {
                delete it.second;
            }
        }

        // Entities should not be copyable
        Entity(const Entity&)            = delete;
        Entity& operator=(Entity const&) = delete;

        glm::mat4 getWorldRotation() const {
            Entity* currentParent  = parent;
            glm::mat4 localToWorld = glm::toMat4(localTransform.qRot);
            while(currentParent) {
                localToWorld  = glm::toMat4(currentParent->localTransform.qRot) * localToWorld;
                currentParent = currentParent->parent;
            }
            return localToWorld;
        }

        glm::vec3 getWorldScale() const {
            Entity* currentParent  = parent;
            glm::vec3 localToWorld = localTransform.scale;
            while(currentParent) {
                localToWorld  = currentParent->localTransform.scale * localToWorld;
                currentParent = currentParent->parent;
            }
            return localToWorld;
        }

        glm::vec3 getWorldTranslation() const {
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
        getComponentsIter() const {
            return {components.begin(), components.end()};
        }

        glm::vec3 getForward() const {
            return glm::vec3(getWorldRotation() * glm::vec4(0, 0, -1, 1));
        }

        glm::vec3 getRight() const {
            return glm::vec3(getWorldRotation() * glm::vec4(1, 0, 0, 1));
        }

        glm::vec3 getUp() const {
            return glm::vec3(getWorldRotation() * glm::vec4(0, 1, 0, 1));
        }

        virtual void onImmediateGui() override;

        void drawComponentAdder(std::string id) {
            ImGui::Combo(("##" + id).c_str(), &currComponent, componentsToAdd, 3);
            ImGui::SameLine();
            if(ImGui::Button(("Add component##" + id).c_str())) {
                if(std::strcmp(componentsToAdd[currComponent], "Mesh Renderer") == 0)
                    addComponent<MeshRendererComponent>();
                // TODO: Implement logic for adding other components
            }
        }

        void drawTransform(std::string id) {
            if(ImGui::TreeNode(("Transform##" + id).c_str())) {

                localTransform.onImmediateGui();

                ImGui::TreePop();
            }
        }

        // Synnchronizes bullet rigidbodies to our data when we update something in the entity list
        void ourToBullet() {
            if(auto rb = getComponent<RigidBody>(); localTransform.changedInUI && rb) {
                rb->syncWithTransform(getWorldRotation(), getWorldTranslation());
                localTransform.changedInUI = false;
            }
        }

        void drawComponents(std::string id) {
            auto [compsBegin, compsEnd] = getComponentsIter();

            for(auto iter = compsBegin; iter != compsEnd; iter++) {
                if(ImGui::TreeNode((iter->second->getIDPolymorphic() + "##" + id).c_str())) {

                    iter->second->onImmediateGui();

                    ImGui::TreePop();
                }
            }
        }
    };
} // namespace our