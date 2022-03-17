#pragma once
#include "Drawer.h"
#include "Shader.h"
#include "Texture.h"
#include "glad/gl.h"
#include "glm/ext/vector_float3.hpp"
#include <vector>

class CubeEdgesDrawer : public Drawer {
public:
  // clang-format off
// Effectively draws 6 faces individually
// Might be useful if we want hard edges later on since each vertex must have 
// its own normal. Corner vertices will be actually 3 vertices with 3 normals.
  glm::vec3 cubeVertices[8 * 2] = {
    // Front                        // Colors
    glm::vec3(-0.5f, -0.5f, +0.5f), glm::vec3(0.5f, 0.0f, 0.5f),
    glm::vec3(+0.5f, -0.5f, +0.5f), glm::vec3(0.5f, 0.0f, 0.5f),
    glm::vec3(+0.5f, +0.5f, +0.5f), glm::vec3(0.5f, 0.0f, 0.5f),
    glm::vec3(-0.5f, +0.5f, +0.5f), glm::vec3(0.5f, 0.0f, 0.5f),

    // Back                         // Colors
    glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.0f, 0.5f),
    glm::vec3(+0.5f, -0.5f, -0.5f), glm::vec3(0.5f, 0.0f, 0.5f),
    glm::vec3(+0.5f, +0.5f, -0.5f), glm::vec3(0.5f, 0.0f, 0.5f),
    glm::vec3(-0.5f, +0.5f, -0.5f), glm::vec3(0.5f, 0.0f, 0.5f)
  };

  GLuint indices[24] = {
    0, 1, 1, 2, 2, 3, 3, 0, // Front
    4, 5, 5, 6, 6, 7, 7, 4, // Back
    0, 4, 1, 5, 2, 6, 3, 7
  };

  // clang-format on
  virtual void init() override {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices,
                 GL_DYNAMIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 24 * sizeof(GLuint), indices,
                 GL_STATIC_DRAW);

    // Vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    // Colors
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
  }
  virtual void bind(Shader &shader, std::vector<Texture> *textures) override {
    shader.use();
    glBindVertexArray(VAO);
    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    // glEnableVertexAttribArray(0);
    // glEnableVertexAttribArray(1);

    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices,
                 GL_DYNAMIC_DRAW);
  }

  void ChangeColors(glm::vec3 &color) {
    for (int i = 1; i < 16; i += 2) {
      cubeVertices[i] = color;
    }
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices,
                 GL_DYNAMIC_DRAW);
  }
};