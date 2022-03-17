#pragma once
#include "Shader.h"

#include "Camera.h"
#include "Texture.h"
#include "application.hpp"
#include "glm/detail/qualifier.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/quaternion_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float4.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/trigonometric.hpp"
#include "imgui.h"
#include <vector>

// clang-format off
float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};
// clang-format on

class Renderer {
public:
  GLuint cubeVAO;
  GLuint cubeVBO;
  std::vector<Texture> textures;
  Shader shader;
  Camera *camera;
  GLFWwindow *window;

  int resX, resY;

  glm::vec4 clearColor = glm::vec4(0.5, 0.2, 0.5, 1.0);

  Renderer() = default;

  Renderer(int resX, int resY, Camera *camera)
      : camera(camera), resX(resX), resY(resY) {

    camera->UpdateProjectionMatrix(resX, resY);

    shader = Shader("assets/shaders/simple.vert", "assets/shaders/simple.frag");

    glEnable(GL_DEPTH_TEST);

    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);

    glGenBuffers(1, &cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    // Vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    // Texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
  }

  Renderer &AddTexture(const Texture tex) {
    textures.push_back(tex);
    return *this;
  }

  void Prerender() {
    shader.use();
    for (int i = 0; i < textures.size(); i++) {
      shader.set("texture" + std::to_string(i + 1), i);
    }
  }

  void BeginRender() {

    // ImGui::Begin("KAK ENGINE", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    for (int i = 0; i < textures.size(); i++) {
      textures[i].use(GL_TEXTURE0 + i);
    }

    glBindVertexArray(cubeVAO);

    ClearScreen(clearColor);

    shader.set("projection", camera->projection);
    shader.set("view", camera->GetViewMatrix());
  }

  void RenderCube(glm::vec3 translation = glm::vec3(0),
                  glm::vec3 rotation = glm::vec3(0),
                  glm::vec3 scale = glm::vec3(1.0f)) const {

    glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);

    model = glm::rotate(model, glm::radians(rotation.x),
                        glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y),
                        glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z),
                        glm::vec3(0.0f, 0.0f, 1.0f));

    model = glm::scale(model, scale);

    shader.set("model", model);

    glDrawArrays(GL_TRIANGLES, 0, 36);
  }

  void onImGui() {
    ImGui::TextWrapped(
        "%s", "Press WASD to move\n"
              "SPACE to go up\n" 
              "LCTRL to go down\n"
              "Hold the right mouse button to pan the camera\n"
              "(Yes you can interact with ImGui with the mouse hidden)\n");
    ImGui::ColorPicker4("Background clear color", (float *)&clearColor);
  }

  void EndRender(GLFWwindow *window) {
    // ImGui::End();
  }

  void ClearScreen(glm::vec4 color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

  ~Renderer() {
    // glDeleteBuffers(1, &cubeVAO);
    // glDeleteBuffers(1, &cubeVBO);
  }

  GLFWwindow *InitGLFW(int resX, int resY) {
    if (glfwInit() == 0) {
      std::cerr << "Failed to initialize GLFW" << std::endl;
      exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHintString(GLFW_FLOATING, "GLFW_TRUE");

    window = glfwCreateWindow(resX, resY, "Example 1", nullptr, nullptr);

    if (window == nullptr) {
      std::cerr << "Failed to create window" << std::endl;
      glfwTerminate();
      exit(-1);
    }

    glfwMakeContextCurrent(window);
    // glfwSetKeyCallback(window, key_callback);
    return window;
  }
};