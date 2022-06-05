#pragma once

#include "asset-loader.hpp"
#include "btVector3.h"
#include "components/mesh-renderer.hpp"
#include "components/player-shooter.hpp"
#include "components/projectile.hpp"
#include "ecs/component.hpp"
#include "ecs/world.hpp"
#include "glm/ext/quaternion_trigonometric.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/trigonometric.hpp"
#include "imgui.h"
#include "player-shooter.hpp"
#include "rigidbody.hpp"
#include "systems/physics.hpp"
#include <functional>
#include <iostream>
#include <iterator>
#include <queue>
#include <string>
#include <vector>

namespace our {
    class TargetingEnemy : public Component {
    public:
        IShootingBehaviour* shootingBehaviour;
        int projectilesBeforeCooldown = 4;     //  How many projectiles to spawn around the turret
        float projectileSpeed         = 15.0f; // How fast the projectiles go
        float spawnDist               = 4.0f;  // How far away radially the projectiles spawn
        float firingDelay             = 0.5;   //

        void init() {
            shootingBehaviour        = new DefaultShootingBehaviour(projectileSpeed, firingDelay, spawnDist, projectilesBeforeCooldown);

            float projectileLifetime             = 2; // How long each projectile should stay alive before removing itself
            auto TargetingEnemyRB                = getOwner()->getComponent<RigidBody>();
            TargetingEnemyRB->tag                = "TargetingEnemy";
            shootingBehaviour->projectileToShoot = Projectile(AssetLoader<Material>::get("danger"), projectileLifetime, TargetingEnemyRB->tag);
        }

        static std::string getID() { return "Targeting Enemy"; }
        virtual std::string getIDPolymorphic() override { return getID(); }

        virtual void deserialize(const nlohmann::json& data) override {
            projectilesBeforeCooldown = data.value("projectilesBeforeCooldown", projectilesBeforeCooldown);
            projectileSpeed           = data.value("projectileSpeed", projectileSpeed);
            spawnDist                 = data.value("spawnDist", spawnDist);
            firingDelay               = data.value("firingDelay", firingDelay);
        }
    };
} // namespace our
