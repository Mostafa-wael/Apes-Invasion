#include "../deserialize-utils.hpp"
#include "ecs/component.hpp"
#include "glm/glm.hpp"

#include <string>

namespace our {
    class PlayerControllerComponent : public Component {
    public:
        float speed                = 5;
        float boostSpeedMultiplier = 20;
        float maxSpeed             = 12.0f;

        // Vertical and horzontal mouse sensitivities repectively
        glm::vec2 rotationSensitivity = glm::vec2(0.5, 1);

        virtual void deserialize(const nlohmann::json& data) override {
            speed                = data.value("movementSpeed", speed);
            boostSpeedMultiplier = data.value("boostSpeedMultiplier", boostSpeedMultiplier);
            rotationSensitivity  = data.value("rotationSensitivity", rotationSensitivity);
        }

        static std::string getID() {
            return "Player Controller";
        }

        std::string getIDPolymorphic() override {
            return getID();
        }
    };
} // namespace our