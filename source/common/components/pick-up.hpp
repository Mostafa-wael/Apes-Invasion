#pragma once
#include "asset-loader.hpp"
#include "components/light.hpp"
#include "components/health.hpp"
#include "components/rigidbody.hpp"
#include "ecs/component.hpp"
#include "ecs/world.hpp"
#include "material/material.hpp"
#include "mesh/mesh-utils.hpp"
#include "rigidbody.hpp"
#include "systems/physics.hpp"
#include "util.h"
#include <functional>
#include <string>
#include <unordered_set>

namespace our {
    class PickUp : public Component {

    public:
        Material* material;
        std::string rigidbodyTag;
        int healAmount = 20;

        PickUp() = default;
        PickUp(Material* mat, std::string tag) : material(mat), rigidbodyTag(tag) {}

        static std::string getID() { return "Pick up"; }

        std::string getIDPolymorphic() override {
            return getID();
        }

        void onCollision(RigidBody* other) {
            auto pickupRB = getOwner()->getComponent<RigidBody>();

            if(pickupRB->tag == other->tag || other->tag != "player") return;


            if(getOwner())
                getOwner()->getWorld()->markForRemoval(getOwner());

            if(other->getOwner()->getComponent<HealthComponent>()) 
            {
                other->getOwner()->getComponent<HealthComponent>()->heal(healAmount);
            }
            
        }

        void deserialize(const nlohmann::json& data) override {
            if(!data.is_object()) return;
            healAmount = data.value("heal amount", healAmount);
        }
    };
} // namespace our