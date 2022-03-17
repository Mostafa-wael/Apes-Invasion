#pragma once
#include "Shader.h"

#include "Camera.h"
#include "CubeDrawer.h"
#include "CubeEdgesDrawer.h"
#include "Texture.h"
#include "application.hpp"
#include "glad/gl.h"
#include "glm/detail/qualifier.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/quaternion_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float4.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/trigonometric.hpp"
#include "imgui.h"
#include <GL/gl.h>
#include <vector>

// clang-format off
float cubeVertices[] = {
    // Front
    -0.5f,    -0.5f,    +0.5f,
    +0.5f,    -0.5f,    +0.5f,
    +0.5f,    +0.5f,    +0.5f,
    -0.5f,    +0.5f,    +0.5f,

    // Back
    -0.5f,    -0.5f,    -0.5f,
    +0.5f,    -0.5f,    -0.5f,
    +0.5f,    +0.5f,    -0.5f,
    -0.5f,    +0.5f,    -0.5f,
};

GLuint cubeIndices[] = {0, 1, 1, 2, 2, 3, 3, 0, // Front
                        4, 5, 5, 6, 6, 7, 7, 4, // Back
                        0, 4, 1, 5, 2, 6, 3, 7};
// clang-format on

class Renderer {
public:
  GLuint cubeVAO;
  GLuint cubeVBO;
  std::vector<Texture> textures;
  Camera *camera;
  GLFWwindow *window;

  int resX, resY;

  CubeDrawer cubeDrawer;
  CubeEdgesDrawer cubeEdgesDrawer;

  glm::vec4 clearColor = glm::vec4(0.5, 0.2, 0.5, 1.0);
  glm::vec3 cubeEdgeColor = glm::vec3(0.3, 0.5, 0.7);

  glm::vec3 objectScale = glm::vec3(1.0f);

  // Renders triangles without indices, each 3 vertices sent to the gpu will be
  // treated as a triangle
  Shader trisShader;
  Shader linesShader;

  Renderer() = default;

  Renderer(int resX, int resY, Camera *camera)
      : camera(camera), resX(resX), resY(resY) {

    camera->UpdateProjectionMatrix(resX, resY);

    trisShader = Shader("assets/shaders/CubeFaces.vert",
                        "assets/shaders/CubeFaces.frag");
    linesShader = Shader("assets/shaders/CubeEdges.vert",
                         "assets/shaders/CubeEdges.frag");

    glEnable(GL_DEPTH_TEST);
    cubeDrawer.init();
    cubeEdgesDrawer.init();
  }

  Renderer &AddTexture(const Texture tex) {
    textures.push_back(tex);
    return *this;
  }

  void BeginRender() {

    // ImGui::Begin("KAK ENGINE", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    for (int i = 0; i < textures.size(); i++) {
      textures[i].use(GL_TEXTURE0 + i);
    }

    ClearScreen(clearColor);

    camera->updateProjAndView(trisShader);
    camera->updateProjAndView(linesShader);
  }

  void RenderCube(glm::vec3 translation = glm::vec3(0),
                  glm::vec3 rotation = glm::vec3(0),
                  glm::vec3 scale = glm::vec3(1.0f)) {

    cubeDrawer.bind(trisShader, &textures);

    glm::mat4 model = transform(translation, rotation, scale);
    linesShader.set("model", model);

    glDrawArrays(GL_TRIANGLES, 0, 36);
  }

  void RenderCubeEdges(glm::vec3 translation = glm::vec3(0),
                       glm::vec3 rotation = glm::vec3(0),
                       glm::vec3 scale = glm::vec3(1.0f),
                       glm::vec3 color = glm::vec3(1.0f)) {

    cubeEdgesDrawer.bind(linesShader, nullptr);

    // Color change can also be done through a uniform
    cubeEdgesDrawer.ChangeColors(color);

    glm::mat4 model = transform(translation, rotation, scale);
    linesShader.set("model", model);

    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
  }

  void onImGui() {
    ImGui::TextWrapped(
        "%s", "Press WASD to move\n"
              "SPACE to go up\n"
              "LCTRL to go down\n"
              "Hold the right mouse button to pan the camera\n"
              "(Yes you can interact with ImGui with the mouse hidden)\n");
    ImGui::ColorPicker4("Background clear color", (float *)&clearColor);
    ImGui::ColorPicker3("Cube edge colors (if you're rendering cube edges)",
                        (float *)&cubeEdgeColor);
    ImGui::SliderFloat("Cube edge scale", (float *)&objectScale.x, 0.1f, 5.0f);
    objectScale.y = objectScale.z = objectScale.x;
  }

  void EndRender(GLFWwindow *window) { glBindVertexArray(0); }

  void ClearScreen(glm::vec4 color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  ~Renderer() {}

  static glm::mat4 eulerRotation(glm::mat4 &model, glm::vec3 &rotation) {

    model = glm::rotate(model, glm::radians(rotation.x),
                        glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y),
                        glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z),
                        glm::vec3(0.0f, 0.0f, 1.0f));

    return model;
  }

  static glm::mat4 transform(glm::vec3 translation = glm::vec3(0),
                             glm::vec3 rotation = glm::vec3(0),
                             glm::vec3 scale = glm::vec3(1)) {

    glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);
    model = eulerRotation(model, rotation);
    return glm::scale(model, scale);
  }
};