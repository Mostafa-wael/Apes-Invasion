#include "Camera.h"
#include "GLFW/glfw3.h"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "input/keyboard.hpp"
#include "input/mouse.hpp"

Camera::Camera(glm::vec3 pos, glm::vec3 up, float yaw, float pitch)
    : frnt(glm::vec3(0.0f, 0.0f, -1.0f)), moveSpeed(SPEED),
      mouseSens(SENSITIVITY), zoom(ZOOM) {

  this->pos = pos;
  wrldUp = up;
  this->yaw = yaw;
  this->pitch = pitch;

  updateCameraVectors();
}

void Camera::ProcessMouseMovement(const glm::vec2 &delta,
                                  GLboolean constrainPitch) {

  if (!rMouseDown)
    return;

  yaw += delta.x * mouseSens;
  pitch += delta.y * mouseSens;

  if (constrainPitch) {
    if (pitch > 89.0f)
      pitch = 89.0f;
    if (pitch < -89.0f)
      pitch = -89.0f;
  }

  updateCameraVectors();
}

void Camera::ProcessButtonPress(float dT, const our::Keyboard &keeb,
                                const our::Mouse &maus, GLFWwindow *window) {

  float velocity = moveSpeed * dT;

  if (keeb.isPressed(GLFW_KEY_W))
    pos += frnt * velocity;
  if (keeb.isPressed(GLFW_KEY_S))
    pos -= frnt * velocity;
  if (keeb.isPressed(GLFW_KEY_A))
    pos -= rgt * velocity;
  if (keeb.isPressed(GLFW_KEY_D))
    pos += rgt * velocity;
  if (keeb.isPressed(GLFW_KEY_SPACE))
    pos += wrldUp * velocity;
  if (keeb.isPressed(GLFW_KEY_LEFT_CONTROL))
    pos -= wrldUp * velocity;

  if (maus.isPressed(GLFW_MOUSE_BUTTON_2)) {
    // maus.lockMouse(window);5
    rMouseDown = true;
  } else if (maus.justReleased(GLFW_MOUSE_BUTTON_2)) {
    // maus.unlockMouse(window);
    rMouseDown = false;
  }
}

void Camera::ProcessMouseScroll(float yoffset) {
  zoom -= (float)yoffset;
  if (zoom < 1.0f)
    zoom = 1.0f;
  if (zoom > 45.0f)
    zoom = 45.0f;
}

void Camera::updateCameraVectors() {

  glm::vec3 direction;
  direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  direction.y = sin(glm::radians(pitch));
  direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  frnt = glm::normalize(direction);

  rgt = glm::normalize(glm::cross(frnt, wrldUp));
  up = glm::normalize(glm::cross(rgt, frnt));
}

void Camera::updateProjAndView(Shader &shader) {
  shader.use();
  shader.set("projection", projection);
  shader.set("view", GetViewMatrix());
}
