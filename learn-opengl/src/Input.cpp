#include "Input.h"
#include "Camera.h"

float  lastFrame = 0.0f, lastX = 250, lastY = 250, deltaTime;
bool   firstMouse = true;
Camera camera;

void processInput(GLFWwindow *window) { camera.ProcessKeyboard(window); }

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
  camera.ProcessMouseMovement(xposIn, yposIn);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  camera.ProcessMouseScroll((float)yoffset);
}