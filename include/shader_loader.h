//
// Created by popbox on 12/15/24.
//

#ifndef SHADER_LOADER_H
#define SHADER_LOADER_H

struct ShaderLoader {
    unsigned int ID;
    ShaderLoader(const char* vertex_shader_path, const char* fragment_shader_path);
    void use();
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setDouble(const std::string& name, double value) const;
};

#endif //SHADER_LOADER_H
