#include "health.hpp"
#include "../ecs/entity.hpp"
#include "../deserialize-utils.hpp"
#include "components/rigidbody.hpp"
#include <algorithm>
#include <iostream>

namespace our {
    void HealthComponent::deserialize(const nlohmann::json& data) {
        if(!data.is_object()) return;
        max_health = data.value("max-health", max_health);
        current_health = data.value("current-health", current_health);
    }
    
    void HealthComponent::heal(int heal_amout) {
        current_health = std::min(current_health+ heal_amout, max_health);
    }

    void HealthComponent::damage(int damage_amout) {
        current_health = std::max(current_health - damage_amout, 0);
        if(getOwner()->getComponent<RigidBody>()->tag != "player" && current_health <= 0) {
            delete getOwner();
        }
    }
}