//
// Created by popbox on 12/27/24.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <compute_shader.h>

ComputeShader::ComputeShader(const char* computeShaderPath) {
    std::ifstream computeShaderFile;
    computeShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    std::string computeShaderCode;
    try {
        computeShaderFile.open(computeShaderPath);
        std::stringstream computeShaderCodeStream;

        computeShaderCodeStream << computeShaderFile.rdbuf();

        computeShaderFile.close();

        computeShaderCode = computeShaderCodeStream.str();
    } catch (std::ifstream::failure& e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ\n"
              << "Path: " << computeShaderPath << "\n"
              << "Error: " << e.what() << std::endl;
        throw;
    }

    const GLchar* shaderCode = computeShaderCode.c_str();
    GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);

    glShaderSource(computeShader, 1, &shaderCode, nullptr);
    glCompileShader(computeShader);
    checkCompileErrors(computeShader, "COMPUTE_SHADER");

    this->id = glCreateProgram();
    glAttachShader(this->id, computeShader);
    glLinkProgram(this->id);
    checkCompileErrors(this->id, "PROGRAM");
}

void ComputeShader::checkCompileErrors(GLuint shader, const std::string& type) {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                    << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                    << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }

void ComputeShader::use() const {
    glUseProgram(this->id);
}

void ComputeShader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(this->id, name.c_str()), (int)value);
}

void ComputeShader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(this->id, name.c_str()), value);
}

void ComputeShader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(this->id, name.c_str()), value);
}

void ComputeShader::setVec2(const std::string &name, const glm::vec2 &value) const {
    glUniform2fv(glGetUniformLocation(this->id, name.c_str()), 1, &value[0]);
}

void ComputeShader::setVec2(const std::string &name, float x, float y) const {
    glUniform2f(glGetUniformLocation(this->id, name.c_str()), x, y);
}

void ComputeShader::setVec3(const std::string &name, const glm::vec3 &value) const {
    glUniform3fv(glGetUniformLocation(this->id, name.c_str()), 1, &value[0]);
}

void ComputeShader::setVec3(const std::string &name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(this->id, name.c_str()), x, y, z);
}

void ComputeShader::setVec4(const std::string &name, const glm::vec4 &value) const {
    glUniform4fv(glGetUniformLocation(this->id, name.c_str()), 1, &value[0]);
}

void ComputeShader::setVec4(const std::string &name, float x, float y, float z, float w) const {
    glUniform4f(glGetUniformLocation(this->id, name.c_str()), x, y, z, w);
}

void ComputeShader::setMat2(const std::string &name, const glm::mat2 &mat) const {
    glUniformMatrix2fv(glGetUniformLocation(this->id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void ComputeShader::setMat3(const std::string &name, const glm::mat3 &mat) const {
    glUniformMatrix3fv(glGetUniformLocation(this->id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void ComputeShader::setMat4(const std::string &name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(this->id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
