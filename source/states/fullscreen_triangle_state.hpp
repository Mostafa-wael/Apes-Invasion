#pragma once

#include "Utils.h"
#include "glad/gl.h"
#include "glm/ext/vector_float2.hpp"
#include "imgui.h"
#include "input/keyboard.hpp"
#include "json/json.hpp"
#include <application.hpp>
#include <iostream>
#include <shader/shader.hpp>
#include <string>

// This function allows us to read glm vectors from json
namespace glm {
template <length_t L, typename T, qualifier Q>
void from_json(const nlohmann::json &j, vec<L, T, Q> &v) {
  for (length_t index = 0; index < L; ++index)
    v[index] = j[index].get<T>();
}
} // namespace glm

struct Line {
  float intercept = 0.0;
  float slope = 1.0;
};

struct Diamond {
  glm::vec2 center = glm::vec2(0, 0);
  float side_length = 0.0;
};

class FullscreenTriangleState : public our::State {

  our::ShaderProgram program;
  // TODO: Add a variable in which we will store the name (ID) for a vertex
  // array
  GLuint vertArrID;

  Line line;
  Diamond diamond;

  // onInitialize() function is called once before the state starts
  void onInitialize() override {

    // Here we get the json of the scene configuration
    const auto &config = getApp()->getConfig()["scene"];

    // Then we get the path for vertex and fragment shader
    // We used "assets/shaders/fullscreen.vert" and "assets/shaders/circle.frag"
    // as the defaults if the paths are not defined
    std::string vertex_shader_path =
        config.value("vertex-shader", "assets/shaders/fullscreen.vert");
    std::string fragment_shader_path =
        config.value("fragment-shader", "assets/shaders/circle.frag");

    // Here we create a shader, attach the vertex and fragment shader to it then
    // link
    program.create();
    program.attach(vertex_shader_path, GL_VERTEX_SHADER);
    program.attach(fragment_shader_path, GL_FRAGMENT_SHADER);
    program.link();

    // We call use() since we will send uniforms to this program
    program.use();

    // We loop over every uniform in the configuration and send to the program
    if (const auto &uniforms = config["uniforms"]; uniforms.is_object()) {
      for (auto &[name, uniform] : uniforms.items()) {
        std::string type = uniform.value("type", "");
        if (type == "float") {
          float value = uniform.value("value", 0.0f);
          program.set(name, value);
        } else if (type == "vec2") {
          glm::vec2 value = uniform.value("value", glm::vec2(0, 0));
          program.set(name, value);
        } else if (type == "vec4") {
          glm::vec4 value = uniform.value("value", glm::vec4(0, 0, 0, 0));
          program.set(name, value);
        }
      }

      // Line intitialzation
      std::string scenetitle = getApp()->getConfig()["window"]["title"];
      auto &sceneUniformList = getApp()->getConfig()["scene"]["uniforms"];
      if (scenetitle == "Line") {
        line.slope = getApp()
                         ->getConfig()["scene"]["uniforms"]["slope"]["value"]
                         .get<float>();
        line.intercept =
            getApp()
                ->getConfig()["scene"]["uniforms"]["intercept"]["value"]
                .get<float>();
      } else if (scenetitle == "Diamond") {
        diamond.center =
            sceneUniformList["center"].value("value", glm::vec2(0, 0));
        diamond.side_length =
            sceneUniformList["side_length"].value("value", 0.0f);
      }
    }

    // TODO: Create a vertex Array
    glGenVertexArrays(1, &vertArrID);

    // We set the clear color to be black
    glClearColor(0.0, 0.0, 0.0, 1.0);
  }

  // onDraw(deltaTime) function is called every frame
  void onDraw(double deltaTime) override {
    UpdateUniforms();

    // At the start of frame we want to clear the screen. Otherwise we would
    // still see the results from the previous frame.
    glClear(GL_COLOR_BUFFER_BIT);

    // TODO: Draw a triangle using the vertex array and the program
    glBindVertexArray(vertArrID);

    glDrawArrays(GL_TRIANGLES, 0, 3);
  }

  // onInitialize() function is called once after the state ends
  void onDestroy() override {
    // TODO: Delete the vertex Array
    glDeleteVertexArrays(1, &vertArrID);
    vertArrID = -1;
  }

  // Keyboard input example
  // Confirmed working, just left it to help if you need an example
  void CheckInputs(float deltaTime) {
    const auto keyboard = getApp()->getKeyboard();
    std::string scenetitle = getApp()->getConfig()["window"]["title"];
    if (scenetitle == "Line") {
      if (keyboard.isPressed(GLFW_KEY_W)) {
        if (line.slope < 1.0f)
          line.slope += 0.1f * deltaTime;
      } else if (keyboard.isPressed(GLFW_KEY_S)) {
        if (line.slope > 0.0f)
          line.slope -= 0.1f * deltaTime;
      } else if (keyboard.isPressed(GLFW_KEY_A)) {
        line.intercept -= 10.0f * deltaTime;
      } else if (keyboard.isPressed(GLFW_KEY_D)) {
        line.intercept += 10.0f * deltaTime;
      }
    } else if (scenetitle == "Diamond") {
      if (keyboard.isPressed(GLFW_KEY_W)) {

      } else if (keyboard.isPressed(GLFW_KEY_S)) {

      } else if (keyboard.isPressed(GLFW_KEY_A)) {

      } else if (keyboard.isPressed(GLFW_KEY_D)) {
      }
    }
  }

  // Updates uniforms
  void UpdateUniforms() {
    std::string scenetitle = getApp()->getConfig()["window"]["title"];
    if (scenetitle == "Line") {
      program.set("slope", line.slope);
      program.set("intercept", line.intercept);
    } else {
      program.set("center", diamond.center);
      program.set("side_length", diamond.side_length);
    }
  }

  // Sets up ImGui layout and updates parameters from it.
  void onImmediateGui() override {
    std::string scenetitle = getApp()->getConfig()["window"]["title"];
    if (scenetitle == "Line") {
      ImGui::SetWindowSize(ImVec2(200, 100));
      ImGui::SliderFloat("Intercept", &line.intercept,
                         -getApp()->getWindowSize().x,
                         getApp()->getWindowSize().x);
      ImGui::SliderFloat("Slope", &line.slope, -1, 1);
    } else if (scenetitle == "Diamond") {
      ImGui::SetWindowSize(ImVec2(200, 100));
      ImGui::SliderFloat("Center X", &diamond.center.x,
                         -getApp()->getWindowSize().x,
                         getApp()->getWindowSize().x);
      ImGui::SliderFloat("Center Y", &diamond.center.y,
                         -getApp()->getWindowSize().y,
                         getApp()->getWindowSize().y);
      ImGui::SliderFloat("Side Length", &diamond.side_length, 0, 512);
    }
  }
};