#include "shader.hpp"

#include <cassert>
#include <iostream>
#include <fstream>
#include <string>

void our::ShaderProgram::create() {
    //Create Shader Program
    program = glCreateProgram();
}

void our::ShaderProgram::destroy() {
    //Delete Shader Program
    if(program != 0) glDeleteProgram(program);
    program = 0;
}

//Forward definition for error checking functions
std::string checkForShaderCompilationErrors(GLuint shader);
std::string checkForLinkingErrors(GLuint program);

bool our::ShaderProgram::attach(const std::string &filename, GLenum type) const {
    // Here, we open the file and read a string from it containing the GLSL code of our shader
    std::ifstream file(filename);
    if(!file){
        std::cerr << "ERROR: Couldn't open shader file: " << filename << std::endl;
        return false;
    }
    std::string sourceString = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    const char* sourceCStr = sourceString.c_str();
    file.close();

    GLuint shaderID = glCreateShader(type);

    //TODO: send the source code to the shader and compile it

    // The shader was created in the line above, using the returned shader ID
    // We assign it its source code which is from 1 file source and then compile it

    // @param1: shader ID
    // @param2: number of strings (files/source codes/ etc..)
    // @param3: the actual source string(s)
    // @param4: array of string lengths (not needed)

    glShaderSource(shaderID, 1, &sourceCStr, nullptr);
    glCompileShader(shaderID);
    
    // Here we check for compilation errors
    //TODO: Uncomment this if block
    
    // Here, we're simply checking if any errors are returned from compiling the shader
    // If there are any errors, print them and delete the shader

    if(std::string error = checkForShaderCompilationErrors(shaderID); !error.empty()){
        std::cerr << "ERROR IN " << filename << std::endl;
        std::cerr << error << std::endl;
        glDeleteShader(shaderID);
        return false;
    }

    
    //TODO: attach the shader to the program then delete the shader
    
    // shaders are attached using the glAttachShader(program, shader) function

    //@param1: Program
    //@param2: Shader

    // Shaders are deleted afterwards because they
    // are no longer needed after being attached to a program

    glAttachShader(this->program, shaderID);
    glDeleteShader(shaderID);

    //We return true since the compilation succeeded
    return true;
}



bool our::ShaderProgram::link() const {
    //TODO: call opengl to link the program identified by this->program 
    
    // Linking is done to link shader inputs/outputs to each other so that we can get shapes with colors
    // i.e. the output of a vertex shader can be the color that's set in the fragment shader

    glLinkProgram(this->program);

    // Here we check for linking errors
    //TODO: Uncomment this if block

    // Similar to the error block above

    if(auto error = checkForLinkingErrors(program); error.size() != 0){
        std::cerr << "LINKING ERROR" << std::endl;
        std::cerr << error << std::endl;
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////
// Function to check for compilation and linking error in shaders //
////////////////////////////////////////////////////////////////////

std::string checkForShaderCompilationErrors(GLuint shader){
     //Check and return any error in the compilation process
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char *logStr = new char[length];
        glGetShaderInfoLog(shader, length, nullptr, logStr);
        std::string errorLog(logStr);
        delete[] logStr;
        return errorLog;
    }
    return std::string();
}

std::string checkForLinkingErrors(GLuint program){
     //Check and return any error in the linking process
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (!status) {
        GLint length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        char *logStr = new char[length];
        glGetProgramInfoLog(program, length, nullptr, logStr);
        std::string error(logStr);
        delete[] logStr;
        return error;
    }
    return std::string();
}