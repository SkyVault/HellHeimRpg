#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <filesystem>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

struct ShaderProgram {
  GLuint program_id, vertex_id, fragment_id;

  int get_loc(const char* name) const;
  void set_uniform(int loc, const float f) const;
  void set_uniform(int loc, const glm::vec2& f) const;
  void set_uniform(int loc, const glm::vec3& f) const;
  void set_uniform(int loc, const glm::vec4& f) const;
  void set_uniform(int loc, const glm::mat4& f) const;
};

GLuint load_shader_string(GLenum type, const char* code);
GLuint load_shader_file(GLenum type, const char* path);
ShaderProgram load_shader_program_vf(GLuint vertex, GLuint fragment);
ShaderProgram load_shader_program_vf_string(const char* v_code, const char* f_code);
ShaderProgram load_shader_program_vf_file(const char* v_path, const char* f_path);

#endif//GRAPHICS_H
