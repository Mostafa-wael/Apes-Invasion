#include "Renderer.h"
#include "Texture.h"

#include "Camera.h"
#include "Input.h"

extern Camera camera;
extern float  deltaTime;
extern float  lastFrame;

// A better thing than having global/extern variables
// is to have a clear heirarchy
// engine -> input, renderer
// Which passes down shared data like the camera and delta time, etc..

int main(int argc, char **argv) {

  // clang-format off
  glm::vec3 cubePositions[] = {
      glm::vec3( 0.0f,  0.0f,  0.0f), 
      glm::vec3( 2.0f,  5.0f, -15.0f), 
      glm::vec3(-1.5f, -2.2f, -2.5f),  
      glm::vec3(-3.8f, -2.0f, -12.3f),  
      glm::vec3( 2.4f, -0.4f, -3.5f),  
      glm::vec3(-1.7f,  3.0f, -7.5f),  
      glm::vec3( 1.3f, -2.0f, -2.5f),  
      glm::vec3( 1.5f,  2.0f, -2.5f), 
      glm::vec3( 1.5f,  0.2f, -1.5f), 
      glm::vec3(-1.3f,  1.0f, -1.5f)  
  };
  // clang-format on

  Renderer rend(1280, 720, camera);

  glfwSetCursorPosCallback(rend.window, mouse_callback);
  glfwSetScrollCallback(rend.window, scroll_callback);

  rend.AddTexture(Texture("assets/container.jpg", GL_RGB))
      .AddTexture(Texture("assets/awesomeface.png", GL_RGBA));

  while (glfwWindowShouldClose(rend.window) == 0) {

    rend.Prerender();
    float currentFrame = glfwGetTime();
    deltaTime          = currentFrame - lastFrame;
    lastFrame          = currentFrame;

    processInput(rend.window);

    rend.BeginRender();

    for (int i = 0; i < 10; i++) {
      float angle = 20.0f * i;
      rend.RenderCube(cubePositions[i], glm::vec3(1.0f, 0.3f, 0.5f) * angle);
    }

    rend.EndRender(rend.window);
  }

  glfwDestroyWindow(rend.window);
  glfwTerminate();
  return 0;
}
