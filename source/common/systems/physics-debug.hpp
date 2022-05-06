#pragma once

#include "LinearMath/btIDebugDraw.h"
#include "LinearMath/btVector3.h"
#include "glad/gl.h"
#include "shader/shader.hpp"
#include <cstdio>
namespace our {
    // https://pybullet.org/Bullet/phpBB3/viewtopic.php?t=11517
    class PhysDebugDraw : public btIDebugDraw {
    public:
        // Helper class; draws the world as seen by Bullet.
        // This is very handy to see it Bullet's world matches yours
        // How to use this class :
        // Declare an instance of the class :
        //
        // dynamicsWorld->setDebugDrawer(&mydebugdrawer);
        // Each frame, call it :
        // mydebugdrawer.SetMatrices(ViewMatrix, ProjectionMatrix);
        // dynamicsWorld->debugDrawWorld();
        GLuint VBO, VAO;
        ShaderProgram lineShader;

    public:
        PhysDebugDraw() {
            lineShader.attach("assets/shaders/physics-debug.vert", GL_VERTEX_SHADER);
            lineShader.attach("assets/shaders/physics-debug.frag", GL_FRAGMENT_SHADER);
            lineShader.link();
            
            glGenBuffers(1, &VBO);
            glGenVertexArrays(1, &VAO);
        }


        void SetMatrices(glm::mat4 pViewMatrix, glm::mat4 pProjectionMatrix) {
            lineShader.use();
            lineShader.set("projection", pProjectionMatrix);
            lineShader.set("view", pViewMatrix);
        }

        virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
            // Vertex data
            GLfloat points[12];

            points[0] = from.x();
            points[1] = from.y();
            points[2] = from.z();
            points[3] = color.x()/255.0f;
            points[4] = color.y()/255.0f;
            points[5] = color.z()/255.0f;

            points[6]  = to.x();
            points[7]  = to.y();
            points[8]  = to.z();
            points[9]  = color.x()/255.0f;
            points[10] = color.y()/255.0f;
            points[11] = color.z()/255.0f;

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);

            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

            glBindVertexArray(VAO);
            
            glDrawArrays(GL_LINES, 0, 2);
            glBindVertexArray(0);
        }
        virtual void drawContactPoint(const btVector3&, const btVector3&, btScalar, int, const btVector3&) {}
        virtual void reportErrorWarning(const char*) {}
        virtual void draw3dText(const btVector3&, const char*) {}
        virtual void setDebugMode(int p) {
            m = p;
        }
        int getDebugMode(void) const { return 3; }
        int m;
    };
} // namespace our