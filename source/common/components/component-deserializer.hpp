#pragma once

#include "../ecs/entity.hpp"
#include "camera.hpp"
#include "rigidbody.hpp"
#include "components/player-shooter.hpp"
#include "components/health.hpp"
#include "free-camera-controller.hpp"
#include "mesh-renderer.hpp"
#include "movement.hpp"
#include "rigidbody.hpp"
#include "targeting-enemy.hpp"
#include "rotating-turret.hpp"
#include "player-controller.hpp"
#include "light.hpp"
#include <iostream>
namespace our {

    // Given a json object, this function picks and creates a component in the given entity
    // based on the "type" specified in the json object which is later deserialized from the rest of the json object
    inline void deserializeComponent(const nlohmann::json& data, Entity* entity) {
        std::string type     = data.value("type", "");
        Component* component = nullptr;
        if(type == CameraComponent::getID()) {
            component = entity->addComponent<CameraComponent>();
        } else if(type == MeshRendererComponent::getID()) {
            component = entity->addComponent<MeshRendererComponent>();
        } else if(type == FreeCameraControllerComponent::getID()) {
            component = entity->addComponent<FreeCameraControllerComponent>();
        } else if(type == MovementComponent::getID()) {
            component = entity->addComponent<MovementComponent>();
        } else if(type == RigidBody::getID()) {
            component = entity->addComponent<RigidBody>();
        } else if(type == RotatingTurret::getID()) {
            component = entity->addComponent<RotatingTurret>();
        } else if (type ==  PlayerControllerComponent::getID()){
            component = entity->addComponent<PlayerControllerComponent>();
        } else if (type == PlayerShooter::getID()){
            component = entity->addComponent<PlayerShooter>();
        } else if (type ==  LightComponent::getID()){
            component = entity->addComponent<LightComponent>();
        } else if (type == HealthComponent::getID()){
            component = entity->addComponent<HealthComponent>();
        } else if (type == TargetingEnemy::getID()){
            component = entity->addComponent<TargetingEnemy>();
        } else {
            std::cout << "Could not find component with type: " << type << std::endl;
        }

        if(component) component->deserialize(data);
    }

} // namespace our