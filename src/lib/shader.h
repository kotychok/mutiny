#pragma once

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
  public:
    // The Program ID
    unsigned int ID { glCreateProgram() };

    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);

    void use();

    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setUnsignedInt(const std::string &name, unsigned int value) const;
    void setFloat(const std::string &name, float value) const;
    void setMat4(const std::string &name, glm::mat4 value) const;
    void setVec3(const std::string &name, glm::vec3 value) const;
    void setVec4(const std::string &name, glm::vec4 value) const;

  private:
    GLuint compileShader(const char* path, GLenum type);
    void compileAndLinkProgram(GLuint vertex, GLuint fragment, GLuint geometry = 0);
};
