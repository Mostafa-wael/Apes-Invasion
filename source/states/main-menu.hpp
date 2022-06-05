#pragma once
#include <fstream>
#include <iostream>
#include <json/json.hpp>

#include "GLFW/glfw3.h"
#include "glad/gl.h"
#include "imgui.h"
#include "glm/ext/vector_float3.hpp"
#include "input/mouse.hpp"
#include "mesh/mesh.hpp"
#include "shader/shader.hpp"
#include "texture/texture2d.hpp"
#include <application.hpp>
#include <asset-loader.hpp>
#include <string>
#include <texture/texture-utils.hpp>

class MainMenu : public our::State {
private:
    our::ShaderProgram* shader;
    our::Mesh* mesh;
    our::Texture2D* texture;
    std::string play_scene_path = "config/app.jsonc";

public:
    void onInitialize() override {

        auto& config = getApp()->getConfig()["scene"];

        shader = new our::ShaderProgram();
        shader->attach("assets/shaders/texture-test.vert", GL_VERTEX_SHADER);
        shader->attach("assets/shaders/texture-test.frag", GL_FRAGMENT_SHADER);
        shader->link();

        std::vector<our::Vertex> vertices = {
            { {-1, -1,  0}, {255, 255, 255, 255}, {0.00, 0.00}, {0, 0, 1} },
            { { 1, -1,  0}, {255, 255, 255, 255}, {1.00, 0.00}, {0, 0, 1} },
            { { 1,  1,  0}, {255, 255, 255, 255}, {1.00, 1.00}, {0, 0, 1} },
            { {-1,  1,  0}, {255, 255, 255, 255}, {0.00, 1.00}, {0, 0, 1} },
        };
        std::vector<unsigned int> elements = {
            0,
            1,
            2,
            2,
            3,
            0,
        };
        mesh = new our::Mesh(vertices, elements);
        if(config.contains("textures") && config["textures"].contains("start_menu")){
            if(getApp()->isGameOver) {
                 texture = our::texture_utils::loadImage(config["textures"]["game_over_menu"]);
            } else if(getApp()->isGameWon) {
                 texture = our::texture_utils::loadImage(config["textures"]["game_won"]);
            } else {
               texture = our::texture_utils::loadImage(config["textures"]["start_menu"]);
            }
        }
        getApp()->isGameOver = false;
        getApp()->isGameWon = false;
        getApp()->score = 0;
    }

    void onDraw(double deltaTime) override {
        glClear(GL_COLOR_BUFFER_BIT);
        shader->use();

        if(texture) {
            glActiveTexture(GL_TEXTURE0);
            texture->bind();

            shader->set("tex", 0);
            mesh->draw();
        }

        const auto keyboard = getApp()->getKeyboard();
        
        if (keyboard.isPressed(GLFW_KEY_ENTER)) {
            std::ifstream file_in(play_scene_path);
            if(!file_in) {
                std::cerr << "Couldn't open file: " << play_scene_path << std::endl;
                return;
            }
            nlohmann::json app_config = nlohmann::json::parse(file_in, nullptr, true, true);
            getApp()->setConfig(app_config);
            getApp()->changeState("main");
        }
    }
};