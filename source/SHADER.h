//
// Created by Atlas on 8/10/2025.
//

#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h> // include glad to get all the required OpenGL headers

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "glm/fwd.hpp"

class Shader
{
public:
    // the program ID
    unsigned int ID;

    // constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath);
    // use/activate the shader
    void use();
    // utility uniform functions
    void setMat4fv(const std::string &name, glm::mat4 matrix) const;
    void setVec4(const std::string &name, glm::vec4 vector) const;
    void setVec4f(const std::string &name, float a, float b, float c, float d) const;
    void setVec3(const std::string &name, const glm::vec3 &vector) const;
    void setVec3f(const std::string &name, float a, float b, float c) const;
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
};

#endif