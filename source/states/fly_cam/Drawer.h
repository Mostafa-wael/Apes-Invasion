#pragma once
#include "Shader.h"
#include "Texture.h"
#include "glad/gl.h"
#include <vector>
class Drawer {
public:
  GLuint VAO = 0;
  GLuint VBO = 0;
  GLuint EBO = 0;

  virtual void init() = 0;
  virtual void bind(Shader& shader, std::vector<Texture>* textures) = 0;
};