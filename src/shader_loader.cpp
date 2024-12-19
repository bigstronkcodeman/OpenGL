//
// Created by popbox on 12/15/24.
//

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include "shader_loader.h"

#include <glad/glad.h>


ShaderLoader::ShaderLoader(const char* vertexShaderPath, const char* fragmentShaderPath) {
    std::string vertexShaderCode;
    std::string fragmentShaderCode;
    std::ifstream vertexShaderFile;
    std::ifstream fragmentShaderFile;

    vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        vertexShaderFile.open(vertexShaderPath);
        fragmentShaderFile.open(fragmentShaderPath);
        std::stringstream vertexShaderStream, fragmentShaderStream;

        vertexShaderStream << vertexShaderFile.rdbuf();
        fragmentShaderStream << fragmentShaderFile.rdbuf();

        vertexShaderFile.close();
        fragmentShaderFile.close();

        vertexShaderCode = vertexShaderStream.str();
        fragmentShaderCode = fragmentShaderStream.str();
    } catch (std::ifstream::failure e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ\n"
              << "Path: " << vertexShaderPath << ", " << fragmentShaderPath << "\n"
              << "Error: " << e.what() << std::endl;
        throw;
    }

    const char* vertexShaderSource = vertexShaderCode.c_str();
    const char* fragmentShaderSource = fragmentShaderCode.c_str();

    unsigned int vertexShader, fragmentShader;
    int success;
    char infolog[1024];

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, sizeof(infolog), nullptr, infolog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infolog << std::endl;
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, sizeof(infolog), nullptr, infolog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infolog << std::endl;
    }

    this->ID = glCreateProgram();
    glAttachShader(this->ID, vertexShader);
    glAttachShader(this->ID, fragmentShader);
    glLinkProgram(this->ID);

    glGetProgramiv(this->ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(this->ID, sizeof(infolog), nullptr, infolog);
        std::cerr << "ERROR::SHADER::LINKING_FAILED\n" << infolog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void ShaderLoader::use() {
    glUseProgram(this->ID);
}

void ShaderLoader::setBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(this->ID, name.c_str()), (int)value);
}

void ShaderLoader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(this->ID, name.c_str()), value);
}

void ShaderLoader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(this->ID, name.c_str()), value);
}

void ShaderLoader::setDouble(const std::string& name, double value) const {
    glUniform1d(glGetUniformLocation(this->ID, name.c_str()), value);
}
