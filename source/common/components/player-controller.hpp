#include "ecs/component.hpp"
#include <string>

namespace our {
    class PlayerControllerComponent : public Component {
    public:
        float speed                = 20;
        float boostSpeedMultiplier = 2;

        virtual void deserialize(const nlohmann::json& data) override {

        }

        static std::string getID()  {
            return "Player Controller";
        }

        std::string getIDPolymorphic() override {
            return getID();
        }
    };
} // namespace our