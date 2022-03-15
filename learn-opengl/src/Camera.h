#pragma once

// clang-format off
#include "glad/gl.h"
// clang-format on

#include "GLFW/glfw3.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/glm.hpp"
#include <GL/gl.h>
#include <cstdlib>
#include <iostream>

// Default camera values
const float YAW         = -90.0f;
const float PITCH       = 0.0f;
const float SPEED       = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM        = 45.0f;

class Camera {
public:
  glm::vec3 pos;
  glm::vec3 frnt;
  glm::vec3 up;
  glm::vec3 rgt;
  glm::vec3 wrldUp;

  float yaw = -90.0f, pitch = 0.0f;
  float moveSpeed;
  float mouseSens;
  float zoom;

  float dT;

  bool rMouseDown = false;

  glm::mat4 projection;

  Camera(glm::vec3 pos = glm::vec3(0.0f),
         glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW,
         float pitch = PITCH);

  // constructor with scalar values
  Camera(float posX, float posY, float posZ, float upX, float upY, float upZ,
         float yaw, float pitch);

  glm::mat4 GetViewMatrix() { return glm::lookAt(pos, pos + frnt, up); }

  void ProcessMouseMovement(float xposIn, float yposIn,
                            GLboolean constrainPitch = true);

  void ProcessMouseScroll(float yoffset);

  void ProcessKeyboard(GLFWwindow *window);

  void UpdateProjectionMatrix(float resX, float resY) {
    projection =
        glm::perspective(glm::radians(zoom), resX / resY, 0.1f, 100.0f);
  }

private:
  void updateCameraVectors();
};