#pragma once
#include "Drawer.h"
#include "Shader.h"
#include "Texture.h"
#include "glad/gl.h"
#include <vector>

class CubeDrawer : public Drawer {
public:
  // clang-format off
  // Effectively draws 6 faces individually
  // Might be useful if we want hard edges later on since each vertex must have 
  // its own normal. Corner vertices will be actually 3 vertices with 3 normals.
  // Copy-pasted from learnopengl.com
  float cubeVertices[36 *5] = {
      -0.5f, -0.5f, -0.5f, +0.0f, +0.0f,
      +0.5f, -0.5f, -0.5f, +1.0f, +0.0f,
      +0.5f, +0.5f, -0.5f, +1.0f, +1.0f,
      +0.5f, +0.5f, -0.5f, +1.0f, +1.0f,
      -0.5f, +0.5f, -0.5f, +0.0f, +1.0f,
      -0.5f, -0.5f, -0.5f, +0.0f, +0.0f,

      -0.5f, -0.5f, +0.5f, +0.0f, +0.0f,
      +0.5f, -0.5f, +0.5f, +1.0f, +0.0f,
      +0.5f, +0.5f, +0.5f, +1.0f, +1.0f,
      +0.5f, +0.5f, +0.5f, +1.0f, +1.0f,
      -0.5f, +0.5f, +0.5f, +0.0f, +1.0f,
      -0.5f, -0.5f, +0.5f, +0.0f, +0.0f,

      -0.5f, +0.5f, +0.5f, +1.0f, +0.0f,
      -0.5f, +0.5f, -0.5f, +1.0f, +1.0f,
      -0.5f, -0.5f, -0.5f, +0.0f, +1.0f,
      -0.5f, -0.5f, -0.5f, +0.0f, +1.0f,
      -0.5f, -0.5f, +0.5f, +0.0f, +0.0f,
      -0.5f, +0.5f, +0.5f, +1.0f, +0.0f,

      +0.5f, +0.5f, +0.5f, +1.0f, +0.0f,
      +0.5f, +0.5f, -0.5f, +1.0f, +1.0f,
      +0.5f, -0.5f, -0.5f, +0.0f, +1.0f,
      +0.5f, -0.5f, -0.5f, +0.0f, +1.0f,
      +0.5f, -0.5f, +0.5f, +0.0f, +0.0f,
      +0.5f, +0.5f, +0.5f, +1.0f, +0.0f,

      -0.5f, -0.5f, -0.5f, +0.0f, +1.0f,
      +0.5f, -0.5f, -0.5f, +1.0f, +1.0f,
      +0.5f, -0.5f, +0.5f, +1.0f, +0.0f,
      +0.5f, -0.5f, +0.5f, +1.0f, +0.0f,
      -0.5f, -0.5f, +0.5f, +0.0f, +0.0f,
      -0.5f, -0.5f, -0.5f, +0.0f, +1.0f,

      -0.5f, +0.5f, -0.5f, +0.0f, +1.0f,
      +0.5f, +0.5f, -0.5f, +1.0f, +1.0f,
      +0.5f, +0.5f, +0.5f, +1.0f, +0.0f,
      +0.5f, +0.5f, +0.5f, +1.0f, +0.0f,
      -0.5f, +0.5f, +0.5f, +0.0f, +0.0f,
      -0.5f, +0.5f, -0.5f, +0.0f, +1.0f
  };
  // clang-format on

  // Check CubeEdgeDraw for details on `init()` and `bind()`
  virtual void init() override {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices,
                 GL_DYNAMIC_DRAW);

    // Vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    // Texture coordinates
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
  }
  
  virtual void bind(Shader &shader, std::vector<Texture> *textures) override {
    shader.use();
    glBindVertexArray(VAO);

    if (textures) {
      for (int i = 0; i < textures->size(); i++) {
        shader.set("texture" + std::to_string(i + 1), i);
      }
    }
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices,
                 GL_DYNAMIC_DRAW);
  }
};