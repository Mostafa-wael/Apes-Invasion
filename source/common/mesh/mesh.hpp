#pragma once

#include <vector>
#include <glad/gl.h>
#include <string>
#include "ecs/IImGuiDrawable.h"
#include "imgui.h"
#include "vertex.hpp"

namespace our {

    #define ATTRIB_LOC_POSITION 0
    #define ATTRIB_LOC_COLOR    1
    #define ATTRIB_LOC_TEXCOORD 2
    #define ATTRIB_LOC_NORMAL   3

    class Mesh : public IImGuiDrawable {
        // Here, we store the object names of the 3 main components of a mesh:
        // A vertex array object, A vertex buffer and an element buffer
        unsigned int VBO, EBO;
        unsigned int VAO;
        // We need to remember the number of elements that will be draw by glDrawElements 
        GLsizei elementCount;
    public:
        std::string path;

        // The constructor takes two vectors:
        // - vertices which contain the vertex data.
        // - elements which contain the indices of the vertices out of which each rectangle will be constructed.
        // The mesh class does not keep these data on the RAM. Instead, it should create
        // a vertex buffer to store the vertex data on the VRAM,
        // an element buffer to store the element data on the VRAM,
        // a vertex array object to define how to read the vertex & element buffer during rendering 
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& elements, std::string pth = "/path/unspecified") : path(pth)
        {
            //TODO: (Req 1) Write this function
            // 1. Create a vertex array object
            glGenVertexArrays(1, &VAO); // Generate a VAO to define how to read the vertex & element buffer during rendering 
            glBindVertexArray(VAO); // Bind the VAO
            
            // 2. Create a vertex buffer object
            glGenBuffers(1, &VBO); // The VBO is a buffer that stores the vertex data to store the them on the VRAM
            glBindBuffer(GL_ARRAY_BUFFER , VBO); // Bind the VBO
            glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex) ,(void *)vertices.data() , GL_STATIC_DRAW); // Copy the vertex data to the VBO
            // remember to store the number of elements in "elementCount" since you will need it for drawing
            elementCount = (GLsizei) elements.size(); // Store the number of elements in "elementCount" since we will need it for drawing

            // 3. Define the vertix attributes
            // For the attribute locations, use the constants defined above: ATTRIB_LOC_POSITION, ATTRIB_LOC_COLOR, etc
            // void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *  offset of the first component );
           
            // 3.1 Position
            glEnableVertexAttribArray(ATTRIB_LOC_POSITION); // Enable the attribute location for position
            glVertexAttribPointer(ATTRIB_LOC_POSITION, 3, GL_FLOAT, false, sizeof(Vertex) ,(void*)0); //  define an array of generic vertex attribute data

            // 3.2 Color
            glEnableVertexAttribArray(ATTRIB_LOC_COLOR); // Enable the attribute location for color
            glVertexAttribPointer(ATTRIB_LOC_COLOR, 4, GL_UNSIGNED_BYTE, true, sizeof(Vertex) ,(void*)(offsetof(Vertex,color))); //  define an array of generic vertex attribute data

            // 3.3 Texture Coordinates
            glEnableVertexAttribArray(ATTRIB_LOC_TEXCOORD); // Enable the attribute location for texture coordinates
            glVertexAttribPointer(ATTRIB_LOC_TEXCOORD, 2, GL_FLOAT, false, sizeof(Vertex) ,(void*)(offsetof(Vertex,tex_coord))); //  define an array of generic vertex attribute data

            // 3.4 Normal
            glEnableVertexAttribArray(ATTRIB_LOC_NORMAL); // Enable the attribute location for normal
            glVertexAttribPointer(ATTRIB_LOC_NORMAL, 3, GL_FLOAT, false, sizeof(Vertex) ,(void*)(offsetof(Vertex,normal))); //  define an array of generic vertex attribute data

            // 4. Create an element buffer object
            glGenBuffers(1, &EBO); // The EBO is a buffer that stores the element data on the VRAM
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // Bind the EBO
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size()*sizeof(unsigned int), (void *)elements.data(), GL_STATIC_DRAW); // Copy the element data to the EBO
            
        }

        // this function should render the mesh
        void draw() 
        {
            //TODO: (Req 1) Write this function
            glBindVertexArray(VAO); // Bind the VAO
            glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT,(void*)0); // Draw the mesh  

        }

        // this function should delete the vertex & element buffers and the vertex array object
        ~Mesh(){
            //TODO: (Req 1) Write this function
            glDeleteVertexArrays(1, &VAO); // Delete the VAO
            glDeleteBuffers(1 ,&VBO); // Delete the VBO
            glDeleteBuffers(1 ,&EBO); // Delete the EBO
        }

        void onImmediateGui(){
            ImGui::LabelText("", "Mesh path: %s", path.c_str());
        }

        Mesh(Mesh const &) = delete;
        Mesh &operator=(Mesh const &) = delete;
    };

}