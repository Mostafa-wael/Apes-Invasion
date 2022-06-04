#include "Dynamics/btRigidBody.h"
#include "GLFW/glfw3.h"
#include "application.hpp"
#include "components/player-controller.hpp"
#include "components/rigidbody.hpp"
#include "ecs/entity.hpp"
#include "util.h"

namespace our {
    class PlayerControllerSystem {
        PlayerControllerComponent* playerController;
        Application* app;

    public:
        void init(PlayerControllerComponent* pc, Application* a) {
            playerController = pc;
            app              = a;

            if(playerController) { // Disable gravity
                // The player rigidbody should be added to the physics world by now
                btRigidBody* bulletRB = playerController->getOwner()->getComponent<RigidBody>()->bulletRB;
                bulletRB->setGravity({0, 0, 0});
                bulletRB->setMassProps(0.1, {0, 0, 0});
                bulletRB->setDamping(0.9, 0);
            }
        }

        void update(float dt) {
            if(!playerController) return;

            Entity* playerEntity  = playerController->getOwner();
            RigidBody* playerRB   = playerEntity->getComponent<RigidBody>();
            btRigidBody* bulletRB = playerRB->bulletRB;

            glm::vec3& position = playerEntity->localTransform.position;
            glm::vec3 rotation = playerEntity->localTransform.getEulerRotation();


            // Rotate camera with mouse movement
            glm::vec2 delta = app->getMouse().getMouseDelta();
            rotation.x -= delta.y * playerController->rotationSensitivity.x * dt; // The y-axis controls the pitch
            rotation.y -= delta.x * playerController->rotationSensitivity.y * dt; // The x-axis controls the yaw
            playerEntity->localTransform.setEulerRotation(rotation);

            // We get the camera model matrix (relative to its parent) to compute the front, up and right directions
            glm::mat4 matrix = playerEntity->localTransform.toMat4();

            glm::vec3 front = playerEntity->getForward(),
                      up    = playerEntity->getUp(),
                      right = playerEntity->getRight();

            float speed = playerController->speed;

            // If left shift is pressed, apply an impulse to the local left or right
            if(app->getKeyboard().isPressed(GLFW_KEY_A) && app->getKeyboard().justPressed(GLFW_KEY_LEFT_SHIFT))
                bulletRB->applyCentralImpulse(glmToBtVector3(-right * speed * playerController->boostImpulseMultiplier));

            if(app->getKeyboard().isPressed(GLFW_KEY_D) && app->getKeyboard().justPressed(GLFW_KEY_LEFT_SHIFT))
                bulletRB->applyCentralImpulse(glmToBtVector3(right * speed * playerController->boostImpulseMultiplier));

            // Cap speed at the controller's max
            if(bulletRB->getLinearVelocity().norm() < playerController->maxSpeed * speed) {
                // We change the camera position based on the keys WASD/QE
                // S & W moves the player back and forth
                if(app->getKeyboard().isPressed(GLFW_KEY_W)) bulletRB->applyCentralForce(glmToBtVector3(front * speed));
                if(app->getKeyboard().isPressed(GLFW_KEY_S)) bulletRB->applyCentralForce(-glmToBtVector3(front * speed));
                // Q & E moves the player up and down
                if(app->getKeyboard().isPressed(GLFW_KEY_E)) bulletRB->applyCentralForce(glmToBtVector3(up * speed));
                if(app->getKeyboard().isPressed(GLFW_KEY_Q)) bulletRB->applyCentralForce(-glmToBtVector3(up * speed));
                // A & D moves the player left or right
                if(app->getKeyboard().isPressed(GLFW_KEY_D)) bulletRB->applyCentralForce(glmToBtVector3(right * speed));
                if(app->getKeyboard().isPressed(GLFW_KEY_A)) bulletRB->applyCentralForce(-glmToBtVector3(right * speed));
            }
        }
    };
} // namespace our