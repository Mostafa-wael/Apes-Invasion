#pragma once
#include "Shader.h"
#include "Texture.h"
#include "glad/gl.h"
#include <vector>

// Defines things common between drawing methods
class Drawer {
public:
  // Common buffers
  GLuint VAO = 0; // Saves bound buffers and attributes
  GLuint VBO = 0; // Points to a vertex buffer
  GLuint EBO = 0; // Points to an index buffer

  // Should generate, bind, and fill the needed buffers
  // Should also define vertex attributes (and enable?) them
  virtual void init() = 0;

  // Should use the shader, bind the VAO, and prepare textures (if needed)
  virtual void bind(Shader& shader, std::vector<Texture>* textures) = 0;
};