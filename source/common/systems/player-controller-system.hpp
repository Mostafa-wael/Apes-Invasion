#include "application.hpp"
#include "components/player-controller.hpp"
#include "ecs/entity.hpp"

namespace our {
    class PlayerControllerSystem {
        PlayerControllerComponent* playerController;
        Application* app;

    public:
        void init(PlayerControllerComponent* pc, Application* a) {
            playerController = pc;
            app              = a;
        }

        void update(float dt) {
            Entity* playerEntity = playerController->getOwner();
            glm::vec3& position  = playerEntity->localTransform.position;

            // We get the camera model matrix (relative to its parent) to compute the front, up and right directions
            glm::mat4 matrix = playerEntity->localTransform.toMat4();

            glm::vec3 front = playerEntity->getForward(),
                      up    = playerEntity->getUp(),
                      right = playerEntity->getRight();

            float speed = playerController->speed;
            // If the LEFT SHIFT key is pressed, we multiply the position sensitivity by the speed up factor
            if(app->getKeyboard().isPressed(GLFW_KEY_LEFT_SHIFT)) speed *= playerController->boostSpeedMultiplier;

            // We change the camera position based on the keys WASD/QE
            // S & W moves the player back and forth
            if(app->getKeyboard().isPressed(GLFW_KEY_W)) position += front * (dt * speed);
            if(app->getKeyboard().isPressed(GLFW_KEY_S)) position -= front * (dt * speed);
            // Q & E moves the player up and down
            if(app->getKeyboard().isPressed(GLFW_KEY_E)) position += up * (dt * speed);
            if(app->getKeyboard().isPressed(GLFW_KEY_Q)) position -= up * (dt * speed);
            // A & D moves the player left or right
            if(app->getKeyboard().isPressed(GLFW_KEY_D)) position += right * (dt * speed);
            if(app->getKeyboard().isPressed(GLFW_KEY_A)) position -= right * (dt * speed);
        }
    };
} // namespace our