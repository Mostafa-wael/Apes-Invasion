#pragma once

#include "GLFW/glfw3.h"
#include "Utils.h"
#include "fly_cam/Camera.h"
#include "fly_cam/Renderer.h"
#include "glm/ext/vector_int2.hpp"
#include "glm/fwd.hpp"
#include <application.hpp>
#include <iostream>

class FlyCam : public our::State {
private:
  double r = 0.0;
  double g = 0.0;
  double b = 0.0;
  double a = 0.0;
  Renderer rend;

  float currentFrame, lastFrame;

  bool firstMouse;
  float lastX, lastY;
  Camera cam;

  glm::vec2 lastMousePos;
  glm::vec2 currMousePos;

  // clang-format off
  glm::vec3 cubePositions[10] = {
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

public:
  FlyCam()
      : firstMouse(true), lastX(-1), lastY(-1), cam(firstMouse, lastX, lastY){};

  // onInitialize() function is called once before the state starts
  void onInitialize() override {
    glm::ivec2 res = getApp()->getWindowSize();
    rend = Renderer(res.x, res.y, &cam);

    rend.AddTexture(Texture("assets/container.jpg", GL_RGB))
        .AddTexture(Texture("assets/awesomeface.png", GL_RGBA));
  }

  // onDraw(deltaTime) function is called every frame
  void onDraw(double deltaTime) override {

    rend.Prerender();
    float currentFrame = glfwGetTime();

    processInput(deltaTime);

    rend.BeginRender();

    for (int i = 0; i < 10; i++) {
      float angle = 20.0f * i;
      rend.RenderCube(cubePositions[i], glm::vec3(1.0f, 0.3f, 0.5f) * angle);
    }

    rend.EndRender(rend.window);
  }

  void onImmediateGui() override {
    ImGui::Text("Mouse deltas: %f, %f", getApp()->getMouse().getMouseDelta().x,
                getApp()->getMouse().getMouseDelta().y);
    ImGui::Text("Mouse pos: %f, %f", getApp()->getMouse().getMousePosition().x,
                getApp()->getMouse().getMousePosition().y);

    rend.onImGui();
  }

  void processInput(float dT) {
    auto keeb = getApp()->getKeyboard();
    auto maus = getApp()->getMouse();
    auto win = getApp()->getWindow();

    if (keeb.isPressed(GLFW_KEY_ESCAPE))
      glfwSetWindowShouldClose(win, GLFW_TRUE);

    rend.camera->ProcessButtonPress(dT, keeb, maus, win);

    auto delta = maus.getMouseDelta();
    delta.y *= -1;

    rend.camera->ProcessMouseMovement(delta);
  }

  void onDestroy() override {}
};