#pragma once
#include "glad/gl.h"
#include <iostream>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "../utils/stb/stb_image.h"

class Texture {

public:
  GLuint ID;

  Texture(std::string path, GLuint sourceFormat = GL_RGB) {
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    {
      // Wrap 2D textures in both x and y (s and t)
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

      // Mipmapping
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                      GL_LINEAR_MIPMAP_LINEAR);
      // Set min and mag filtering to point and bilinear repsectively
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      int width, height, nrChannels;
      unsigned char *data =
          stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

      if (data) {
        glTexImage2D(GL_TEXTURE_2D, // Texture target
                     0,             // Mipmap level to use. 0 for the base level
                     GL_RGB,        // Image format of the destination
                     width, height, // Obvious stuff
                     0,             // Always 0, legacy stuff
                     sourceFormat,  // Image format of the source
                     GL_UNSIGNED_BYTE, // Datatype of the source
                     data              // Texture data
        );

        // Generate mipmaps for the currently bound texture
        glGenerateMipmap(GL_TEXTURE_2D);
      } else {
        std::cout << "Failed to load at path " << path << std::endl;
      }

      stbi_image_free(data);
    }
  }

  // GL_TEXTURE0 for example
  void use(GLuint textureUnit) {
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, ID);
  }
};