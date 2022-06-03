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
        float firingDelay = 0.5;
        float firingTimer = firingDelay;

        float projectileLifetime = 2;
        int projectilesPerDelay  = 4;

        float rotationSpeed      = 5;
        float projectileVelocity = 15.0f;
        float spawnDist          = 3.0f;

        bool t = false;

        static std::string getID() { return "Rotating Turret"; }
        virtual std::string getIDPolymorphic() override { return getID(); }

        virtual void deserialize(const nlohmann::json& data) override {
            // TODO: Deserialize for this if needed
        }

        virtual void onImmediateGui() override {
            ImGui::ProgressBar(1 - firingTimer);
        }
    };
} // namespace our
