#pragma once

#include <application.hpp>
#include <iostream>
#include "Utils.h"

class ClearColorState: public our::State {
    private:
    double r = 0.0;
    double g = 0.0;
    double b = 0.0;
    double a = 0.0;

    public:
    // onInitialize() function is called once before the state starts
    void onInitialize() override {
        //TODO: Read the color from the configuration file and use it to set the clear color for the window
        //HINT: you can read the configuration for the whole application using "getApp()->getConfig()"
        //To see how the clear color is written in the json files, see "config/blue-screen.json"
        //To know how read data from a nlohmann::json object, 
        //look at the following documentation: https://json.nlohmann.me/features/element_access/
        const nlohmann::json& clear_color = getApp()->getConfig().at("scene").at("clear-color");
        std::cerr << clear_color <<std::endl;
        r =  clear_color.at("r");
        g =  clear_color.at("g");
        b =  clear_color.at("b");
        a =  clear_color.at("a");
        std::cout << our::FormatString("r= %.2f, g= %.2f, b= %.2f, a= %.2f\n", r,g,b,a);
    }

    // onDraw(deltaTime) function is called every frame 
    void onDraw(double deltaTime) override {
        //At the start of frame we want to clear the screen. Otherwise we would still see the results from the previous frame.

        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT);
    }
};