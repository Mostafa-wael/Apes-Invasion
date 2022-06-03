#pragma once

#include "asset-loader.hpp"
#include "btVector3.h"
#include "components/mesh-renderer.hpp"
#include "components/projectile.hpp"
#include "ecs/component.hpp"
#include "ecs/world.hpp"
#include "glm/ext/quaternion_trigonometric.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/fwd.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/trigonometric.hpp"
#include "imgui.h"
#include "rigidbody.hpp"
#include "systems/physics.hpp"
#include <functional>
#include <iostream>
#include <iterator>
#include <queue>
#include <string>
#include <vector>

namespace our {
    class RotatingTurret : public Component {
    public:
        float firingTimer = firingDelay; // Timer to keep track of how much is left before the next shooting event
        float firingDelay = 0.5;         // Delay between shooting events

        float projectileLifetime = 2; // How long each projectile should stay alive before removing itself
        int projectilesPerEvent  = 4; //  How many projectiles to spawn around the turret

        float rotationSpeed      = 5;     // How fast the turret rotates, constant regardless of framerate
        float projectileSpeed = 15.0f; // How fast the projectiles go
        float spawnDist          = 4.0f;  // How far away radially the projectiles spawn


        static std::string getID() { return "Rotating Turret"; }
        virtual std::string getIDPolymorphic() override { return getID(); }

        virtual void deserialize(const nlohmann::json& data) override {
            firingDelay = data.value("firingDelay", 0.5f);
            projectileLifetime = data.value("projetileLifetime", 2);
            projectilesPerEvent = data.value("projectilesPerEvent", 4);
            rotationSpeed = data.value("rotationSpeed", 5);
            projectileSpeed = data.value("projectileSpeed", 15.0f);
            spawnDist = data.value("spawnDistance", 4.0f);

            firingTimer = firingDelay;
        }

        virtual void onImmediateGui() override {
            ImGui::ProgressBar(1 - firingTimer);
        }
    };
} // namespace our
