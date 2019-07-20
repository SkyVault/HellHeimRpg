#include "graphics.hpp"

GLuint load_shader_string(GLenum type, const char* code) {
  const auto id = glCreateShader(type);
  
  glShaderSource(id, 1, &code, NULL);
  glCompileShader(id);

  GLint result;
  GLint length;
  glGetShaderiv(id, GL_COMPILE_STATUS, &result);
  glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

  if (!result){
    std::vector<char> message(length+1);
    glGetShaderInfoLog(id, length, NULL, &message[0]);
    printf("ERROR::%s:: %s\n",
	   (type == GL_VERTEX_SHADER?"VERTEX_SHADER":"FRAGMENT_SHADER"),
	   &message[0]);
  }

  return id;
}

GLuint load_shader_file(GLenum type, const char* path) {
  std::string code;
  std::ifstream stream(path, std::ios::in);

  if (!stream) {
    printf("ERROR::%s:: File: %s does not exist.\n",
	   (type == GL_VERTEX_SHADER?"VERTEX_SHADER":"FRAGMENT_SHADER"),
	   path);
    return 0;
  }

  std::string line;
  while (std::getline(stream, line)) {
    code += line + "\n";
  }

  return load_shader_string(type, code.c_str());
}

ShaderProgram load_shader_program_vf(GLuint vertex, GLuint fragment) {
  auto result = ShaderProgram{0, vertex, fragment};

  result.program_id = glCreateProgram();
  glAttachShader(result.program_id, vertex);
  glAttachShader(result.program_id, fragment);
  glLinkProgram(result.program_id);

  int succ;
  int length;
  glGetProgramiv(result.program_id, GL_LINK_STATUS, &succ);
  glGetProgramiv(result.program_id, GL_INFO_LOG_LENGTH, &length);

  if (!succ) {
    std::vector<char> message(length + 1);
    glGetProgramInfoLog(result.program_id, length, NULL, &message[0]);
    printf("ERROR::%s:: %s\n",
	   "PROGRAM_LINK",
	   &message[0]);
  }

  glDeleteShader(vertex);
  glDeleteShader(fragment);

  return result;
}

ShaderProgram load_shader_program_vf_string(const char* v_code, const char* f_code) {
  const auto vert_id = load_shader_string(GL_VERTEX_SHADER, v_code);
  const auto frag_id = load_shader_string(GL_FRAGMENT_SHADER, f_code);

  return load_shader_program_vf(vert_id, frag_id);
}

ShaderProgram load_shader_program_vf_file(const char* v_path, const char* f_path) {
  const auto vert_id = load_shader_file(GL_VERTEX_SHADER, v_path);
  const auto frag_id = load_shader_file(GL_FRAGMENT_SHADER, f_path);

  return load_shader_program_vf(vert_id, frag_id);
}

int ShaderProgram::get_loc(const char* name) const {
  return glGetUniformLocation(program_id, name);
}

void ShaderProgram::set_uniform(int loc, const float f) const {
    glUniform1f(loc, f);
}

void ShaderProgram::set_uniform(int loc, const glm::vec2& f) const {
  glUniform2f(loc, f.x, f.y);
}

void ShaderProgram::set_uniform(int loc, const glm::vec3& f) const {
  glUniform3f(loc, f.x, f.y, f.z);
}

void ShaderProgram::set_uniform(int loc, const glm::vec4& f) const {
  glUniform4f(loc, f.x, f.y, f.z, f.w);
}

void ShaderProgram::set_uniform(int loc, const glm::mat4& f) const {
  glUniformMatrix4fv(loc, 1, GL_FALSE, &f[0][0]);
}
