#pragma once

#include "ecs/IImGuiDrawable.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/fwd.hpp"
#include "imgui.h"
#include "vertex.hpp"
#include <algorithm>
#include <cmath>
#include <glad/gl.h>
#include <string>
#include <utility>
#include <vector>

namespace our {

#define ATTRIB_LOC_POSITION 0
#define ATTRIB_LOC_COLOR 1
#define ATTRIB_LOC_TEXCOORD 2
#define ATTRIB_LOC_NORMAL 3

    class Mesh : public IImGuiDrawable {
        // Here, we store the object names of the 3 main components of a mesh:
        // A vertex array object, A vertex buffer and an element buffer
        unsigned int VBO, EBO;
        unsigned int VAO;
        // We need to remember the number of elements that will be draw by glDrawElements
        GLsizei elementCount;

    public:
        std::string path;
        bool enabled = true;

        std::vector<glm::vec3> verts;

        // The constructor takes two vectors:
        // - vertices which contain the vertex data.
        // - elements which contain the indices of the vertices out of which each rectangle will be constructed.
        // The mesh class does not keep these data on the RAM. Instead, it should create
        // a vertex buffer to store the vertex data on the VRAM,
        // an element buffer to store the element data on the VRAM,
        // a vertex array object to define how to read the vertex & element buffer during rendering
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& elements, std::string pth = "/path/unspecified") : path(pth) {

            for (auto&& vert : vertices) verts.push_back(vert.position);

            //TODO: (Req 1) Write this function
            // 1. Create a vertex array object
            glGenVertexArrays(1, &VAO); // Generate a VAO to define how to read the vertex & element buffer during rendering
            glBindVertexArray(VAO);     // Bind the VAO

            // 2. Create a vertex buffer object
            glGenBuffers(1, &VBO);                                                                                   // The VBO is a buffer that stores the vertex data to store the them on the VRAM
            glBindBuffer(GL_ARRAY_BUFFER, VBO);                                                                      // Bind the VBO
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), (void*)vertices.data(), GL_STATIC_DRAW); // Copy the vertex data to the VBO
            // remember to store the number of elements in "elementCount" since you will need it for drawing
            elementCount = (GLsizei)elements.size(); // Store the number of elements in "elementCount" since we will need it for drawing

            // 3. Define the vertix attributes
            // For the attribute locations, use the constants defined above: ATTRIB_LOC_POSITION, ATTRIB_LOC_COLOR, etc
            // void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *  offset of the first component );

            // 3.1 Position
            glEnableVertexAttribArray(ATTRIB_LOC_POSITION);                                           // Enable the attribute location for position
            glVertexAttribPointer(ATTRIB_LOC_POSITION, 3, GL_FLOAT, false, sizeof(Vertex), (void*)0); //  define an array of generic vertex attribute data

            // 3.2 Color
            glEnableVertexAttribArray(ATTRIB_LOC_COLOR);                                                                          // Enable the attribute location for color
            glVertexAttribPointer(ATTRIB_LOC_COLOR, 4, GL_UNSIGNED_BYTE, true, sizeof(Vertex), (void*)(offsetof(Vertex, color))); //  define an array of generic vertex attribute data

            // 3.3 Texture Coordinates
            glEnableVertexAttribArray(ATTRIB_LOC_TEXCOORD);                                                                       // Enable the attribute location for texture coordinates
            glVertexAttribPointer(ATTRIB_LOC_TEXCOORD, 2, GL_FLOAT, false, sizeof(Vertex), (void*)(offsetof(Vertex, tex_coord))); //  define an array of generic vertex attribute data

            // 3.4 Normal
            glEnableVertexAttribArray(ATTRIB_LOC_NORMAL);                                                                    // Enable the attribute location for normal
            glVertexAttribPointer(ATTRIB_LOC_NORMAL, 3, GL_FLOAT, false, sizeof(Vertex), (void*)(offsetof(Vertex, normal))); //  define an array of generic vertex attribute data

            // 4. Create an element buffer object
            glGenBuffers(1, &EBO);                                                                                                 // The EBO is a buffer that stores the element data on the VRAM
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);                                                                            // Bind the EBO
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(unsigned int), (void*)elements.data(), GL_STATIC_DRAW); // Copy the element data to the EBO
        }

        // this function should render the mesh
        void draw() {
            if(!enabled) return;
            //TODO: (Req 1) Write this function
            glBindVertexArray(VAO);                                                // Bind the VAO
            glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, (void*)0); // Draw the mesh
        }

        // this function should delete the vertex & element buffers and the vertex array object
        ~Mesh() {
            //TODO: (Req 1) Write this function
            glDeleteVertexArrays(1, &VAO); // Delete the VAO
            glDeleteBuffers(1, &VBO);      // Delete the VBO
            glDeleteBuffers(1, &EBO);      // Delete the EBO
        }

        void onImmediateGui() {
            ImGui::LabelText("", "Mesh path: %s", path.c_str());
            ImGui::Checkbox(("render##" + std::to_string((uint64_t)(this))).c_str(), &enabled);
        }

        std::pair<glm::vec3, glm::vec3> getAABB( glm::vec3 scale) {
            glm::vec3 min = glm::vec3(INFINITY);
            glm::vec3 max = glm::vec3(-INFINITY);

            std::vector<glm::vec3> vertsTransformed = verts;

            glm::mat4 transform = glm::scale(glm::mat4(1), scale);
            for (auto&& vert : vertsTransformed) vert = transform * glm::vec4(vert,1);

            for(auto&& v : vertsTransformed) {
                min.x = std::fmin(min.x, v.x);
                min.y = std::fmin(min.y, v.y);
                min.z = std::fmin(min.z, v.z);

                max.x = std::fmax(max.x, v.x);
                max.y = std::fmax(max.y, v.y);
                max.z = std::fmax(max.z, v.z);
            }
            glm::vec3 aabbCenter      = (min + max) / 2.0f;
            float minDist = 1;

            if(fabs(min.x - max.x) < minDist) {
                min.x = aabbCenter.x - minDist/2;
                max.x = aabbCenter.x + minDist/2;
            }

            if(fabs(min.y - max.y) < minDist) {
                min.y = aabbCenter.y - minDist/2;
                max.y = aabbCenter.y + minDist/2;
            }

            if(fabs(min.z - max.z) < minDist) {
                min.z = aabbCenter.z - minDist/2;
                max.z = aabbCenter.z + minDist/2;
            }

            glm::vec3 aabbHalfExtents = (max - min) / 2.0f;

            return {aabbCenter, aabbHalfExtents};
        }

        Mesh(Mesh const&)            = delete;
        Mesh& operator=(Mesh const&) = delete;
    };

} // namespace our