#include "texture-utils.hpp"
#include "glad/gl.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <iostream>

our::Texture2D* our::texture_utils::empty(GLenum format, glm::ivec2 size) {
    our::Texture2D* texture = new our::Texture2D();
    //TODO: (Req 10) Finish this function to create an empty texture with the given size and format
    texture->bind();
    if(format == GL_RGBA8) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size[0], size[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    } else if(format == GL_DEPTH_COMPONENT24)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, size[0], size[1], 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
    return texture;
}

our::Texture2D* our::texture_utils::loadImage(const std::string& filename, bool generate_mipmap) {
    glm::ivec2 size;
    int channels;
    //Since OpenGL puts the texture origin at the bottom left while images typically has the origin at the top left,
    //We need to till stb to flip images vertically after loading them
    stbi_set_flip_vertically_on_load(true);
    //Load image data and retrieve width, height and number of channels in the image
    //The last argument is the number of channels we want and it can have the following values:
    //- 0: Keep number of channels the same as in the image file
    //- 1: Grayscale only
    //- 2: Grayscale and Alpha
    //- 3: RGB
    //- 4: RGB and Alpha (RGBA)
    //Note: channels (the 4th argument) always returns the original number of channels in the file
    unsigned char* pixels = stbi_load(filename.c_str(), &size.x, &size.y, &channels, 4);
    if(pixels == nullptr) {
        std::cerr << "Failed to load image: " << filename << std::endl;
        return nullptr;
    }
    // Create a texture
    our::Texture2D* texture = new our::Texture2D();
    //Bind the texture such that we upload the image data to its storage
    //TODO: (Req 4) Finish this function to fill the texture with the data found in "pixels" and generate the mipmaps if requested
    texture->bind();
    glTexImage2D(GL_TEXTURE_2D,    //  target: Specifies the target texture. We only use the 2d Textures so it's hardcoded.
                 0,                //  level: Specifies the level-of-detail number.
                                   //         Level 0 is the base image level. Level n is the nth mipmap reduction image.
                 GL_RGBA8,         //  internalformat: Specifies the number of color components in the texture.
                 (GLsizei)size.x,  //  width: Specifies the width of the texture image.
                 (GLsizei)size.y,  //  height: Specifies the height of the texture image.
                 0,                //  border: This value must be 0.
                 GL_RGBA,          //  format: Specifies the format of the pixel data.
                 GL_UNSIGNED_BYTE, //  type: Specifies the data type of the pixel data.
                 pixels            //  data: Specifies a pointer to the image data in memory.
    );
    if(generate_mipmap) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(pixels); //Free image data after uploading to GPU
    return texture;
}