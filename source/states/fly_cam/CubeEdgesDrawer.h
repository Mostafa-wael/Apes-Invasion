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
  // The eight essential vertices for a cube
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

  // The order we draw edges between the vertices
  GLuint indices[24] = {
    0, 1, 1, 2, 2, 3, 3, 0, // Front
    4, 5, 5, 6, 6, 7, 7, 4, // Back
    0, 4, 1, 5, 2, 6, 3, 7
  };
  // clang-format on

  virtual void init() override {
    // Create and bind the (settings) VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // VBO to contain cube verts
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices,
                 GL_DYNAMIC_DRAW);

    // EBO to contain the order we draw the edges in
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 24 * sizeof(GLuint), indices,
                 GL_STATIC_DRAW);

    //                 v-----------------------------v Attrib 1 stride
    // v-----------------------------v Attrib 0 stride
    // P1x, P1y, P1z, C1x, C1y, C1z, P2x, P2y, P2z, C2x, C2y, C2z
    //                ^ Attrib 1 start
    // ^ Attrib 0 start     
    // Vertex positions
    glVertexAttribPointer(
        0,        // Attribute position in the shader `layout (location= X)`
        3,        // How many elements we will send
        GL_FLOAT, // The type of elements we will send
        GL_FALSE, // Normalize? mostly for if we want to send uints instead of
                  // floats
        6 * sizeof(float), // The stride between each attribute element
        (void *)0          // Initial offset from the buffer's start
    );
    glEnableVertexAttribArray(0);

    // Colors
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
  }

  // Uses the shader, binds the VAO, then updates the data before drawing
  virtual void bind(Shader &shader, std::vector<Texture> *textures) override {
    shader.use();
    glBindVertexArray(VAO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices,
                 GL_DYNAMIC_DRAW);
  }

  // Changes the color portion of the vertex data then uploads it to the shader
  void ChangeColors(glm::vec3 &color) {
    for (int i = 1; i < 16; i += 2) {
      cubeVertices[i] = color;
    }
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices,
                 GL_DYNAMIC_DRAW);
  }
};