#pragma once
#include <iostream>
#include <fstream>
#include <json/json.hpp>

#include "GLFW/glfw3.h"
#include "glad/gl.h"
#include "input/mouse.hpp"
#include "mesh/mesh.hpp"
#include "shader/shader.hpp"
#include "texture/texture2d.hpp"
#include <texture/texture-utils.hpp>
#include <application.hpp>
#include <asset-loader.hpp>
#include <string>


class MainMenu: public our::State {
    private:
    double r = 0.0;
    double g = 0.0;
    double b = 0.0;
    double a = 0.0;
    our::ShaderProgram* shader;
    our::Mesh* mesh;
    our::Texture2D* texture;
    std::string play_scene_path = "config/physics.jsonc";

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
            0, 1, 2,
            2, 3, 0,
        };
        mesh = new our::Mesh(vertices, elements);
        texture = our::texture_utils::loadImage(config["textures"]["start_menu"]);

        const nlohmann::json& clear_color = config["clear-color"];
        std::cerr << clear_color <<std::endl;
        r =  clear_color.at("r");
        g =  clear_color.at("g");
        b =  clear_color.at("b");
        a =  clear_color.at("a");
        std::cerr << "r= " << r << " g= " << g << " b= " << b << " a= " << a <<std::endl;
    }


    void onDraw(double deltaTime) override {
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT);
        shader->use();


        glActiveTexture(GL_TEXTURE0);
        texture->bind();

        shader->set("tex", 0);
        mesh->draw();

        const auto keyboard = getApp()->getKeyboard();


        if (keyboard.isPressed(GLFW_KEY_ENTER)) {
            std::ifstream file_in(play_scene_path);
            if(!file_in){
                std::cerr << "Couldn't open file: " << play_scene_path << std::endl;
                return;
            }
            nlohmann::json app_config = nlohmann::json::parse(file_in, nullptr, true, true);
            std::cerr << "mouse is pressed " <<std::endl;
            getApp()->setConfig(app_config);
            getApp()->changeState("physics-test");
        }
    }
};