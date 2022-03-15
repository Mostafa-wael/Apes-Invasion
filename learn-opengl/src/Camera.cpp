#include "Camera.h"
#include "GLFW/glfw3.h"

extern Camera camera;
extern float  deltaTime;
extern float  lastFrame;
extern float  lastX;
extern float  lastY;
extern bool   firstMouse;

// Defines several possible options for camera movement. Used as abstraction to
// stay away from window-system specific input methods
enum Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };

Camera::Camera(glm::vec3 pos, glm::vec3 up, float yaw, float pitch)
    : frnt(glm::vec3(0.0f, 0.0f, -1.0f))
    , moveSpeed(SPEED)
    , mouseSens(SENSITIVITY)
    , zoom(ZOOM) {

  this->pos   = pos;
  wrldUp      = up;
  this->yaw   = yaw;
  this->pitch = pitch;

  updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY,
               float upZ, float yaw, float pitch)
    : frnt(glm::vec3(0.0f, 0.0f, -1.0f))
    , moveSpeed(SPEED)
    , mouseSens(SENSITIVITY)
    , zoom(ZOOM) {
  pos         = glm::vec3(posX, posY, posZ);
  wrldUp      = glm::vec3(upX, upY, upZ);
  this->yaw   = yaw;
  this->pitch = pitch;
  updateCameraVectors();
}

void Camera::ProcessMouseMovement(float xposIn, float yposIn,
                                  GLboolean constrainPitch) {

  if (!rMouseDown)
    return;
    
  float xpos = (float)xposIn;
  float ypos = (float)yposIn;

  if (firstMouse) {
    lastX      = xpos;
    lastY      = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  // reversed since y-coordinates go from bottom to top
  float yoffset = lastY - ypos;

  lastX = xpos;
  lastY = ypos;

  xoffset *= mouseSens;
  yoffset *= mouseSens;

  yaw += xoffset;
  pitch += yoffset;

  if (constrainPitch) {
    if (pitch > 89.0f)
      pitch = 89.0f;
    if (pitch < -89.0f)
      pitch = -89.0f;
  }

  updateCameraVectors();
}

void Camera::ProcessKeyboard(GLFWwindow *window) {

  this->dT       = deltaTime;
  float velocity = moveSpeed * deltaTime;

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    pos += frnt * velocity;
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    pos -= frnt * velocity;
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    pos -= rgt * velocity;
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    pos += rgt * velocity;
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    pos += wrldUp * velocity;
  if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    pos -= wrldUp * velocity;

  if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    rMouseDown = true;
  } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_RELEASE) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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
  frnt        = glm::normalize(direction);

  rgt = glm::normalize(glm::cross(frnt, wrldUp));
  up  = glm::normalize(glm::cross(rgt, frnt));
}