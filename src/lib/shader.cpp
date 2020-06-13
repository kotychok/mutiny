#include <fstream>
#include <sstream>
#include <iostream>

#include "shader.h"
#include "file.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath) {
  GLuint vertex = compileShader(vertexPath, GL_VERTEX_SHADER);
  GLuint fragment = compileShader(fragmentPath, GL_FRAGMENT_SHADER);
  if (geometryPath == nullptr) {
    compileAndLinkProgram(vertex, fragment);
  } else {
    GLuint geometry = compileShader(geometryPath, GL_GEOMETRY_SHADER);
    compileAndLinkProgram(vertex, fragment, geometry);
  }
}

void Shader::use() {
  glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setInt(const std::string &name, int value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setUnsignedInt(const std::string &name, unsigned int value) const {
  glUniform1ui(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
  glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setMat4(const std::string &name, glm::mat4 value) const {
  glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setVec3(const std::string &name, glm::vec3 value) const {
  glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setVec4(const std::string &name, glm::vec4 value) const {
  glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

GLuint Shader::compileShader(const char* path, GLenum type) {
  std::string codeString = File::read(path);
  const char* code = codeString.c_str();

  // 2. Compile shader code
  GLuint shader;

  int success;
  char infoLog[512];

  shader = glCreateShader(type);
  glShaderSource(shader, 1, &code, NULL);
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cerr << "ERROR: Shader compilation failed. Path: " << path << std::endl;
  }

  return shader;
}

void Shader::compileAndLinkProgram(GLuint vertex, GLuint fragment, GLuint geometry) {
  // Create Shader Program
  glAttachShader(ID, vertex);
  glAttachShader(ID, fragment);
  if (geometry != 0) {
    glAttachShader(ID, geometry);
  }
  glLinkProgram(ID);

  int success;
  char infoLog[512];

  glGetProgramiv(ID, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(ID, 512, NULL, infoLog);
    std::cerr << "ERROR: Program linking failed" << std::endl;
  }

  glDeleteShader(vertex);
  glDeleteShader(fragment);
  if (geometry != 0) {
    glDeleteShader(geometry);
  }
}
