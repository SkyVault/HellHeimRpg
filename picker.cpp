#include "picker.hpp"

Selectable::Selectable() {
   auto rand_float = []() {
     return (float)rand() / (float)RAND_MAX;
   };

   unique_color = glm::vec4(
      rand_float(),
      rand_float(),
      rand_float(),
      1.0
   );
}

Picker::Picker(const int width, const int height) {
    frame = make_framebuffer(width, height);

    program = load_shader_program_vf_string(
       R"VERTEX(
#version 330 core
layout (location = 0) in vec3 aVertices;
layout (location = 1) in vec3 aNormals;
layout (location = 2) in vec2 aUvs;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

out vec2 Uvs;
out vec3 FragPos;
out vec3 Normal;

void main() {
  Uvs = aUvs;
  FragPos = vec3(model * vec4(aVertices, 1.0));
  Normal = mat3(transpose(inverse(model))) * aNormals;
  gl_Position = projection * view * vec4(FragPos, 1.0);
}
       )VERTEX",

       R"FRAGMENT(
#version 330 core
in vec2 Uvs;
in vec3 FragPos;
in vec3 Normal;
uniform vec4 diffuse;
void main() {
    gl_FragColor = diffuse;
}
       )FRAGMENT"
    );

    diffuse_loc = program.get_loc("diffuse");
    projection_loc = program.get_loc("projection");
    view_loc = program.get_loc("view");
    model_loc = program.get_loc("model");
}

void handle_picking(Picker& picker, entt::registry* reg, GLFWwindow* window, glm::mat4 proj, glm::mat4 view, const int width, const int height) {
  glBindFramebuffer(GL_FRAMEBUFFER, picker.frame.fbo);

  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glViewport(0, 0, width, height);

  glUseProgram(picker.program.program_id);

  picker.program.set_uniform(picker.projection_loc, proj);
  picker.program.set_uniform(picker.view_loc, view);

  reg->view<Model, Transform, Selectable>().each([&](
    auto ent, auto& model, auto& trans, auto& selectable) {

    const auto& model_trans = get_transform_matrix(trans);
    const auto* mesh = model.mesh;

    picker.program.set_uniform(picker.model_loc, model_trans);
    picker.program.set_uniform(picker.diffuse_loc, selectable.unique_color);
    glBindVertexArray(mesh->vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);
    glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, (void*)0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  });

  glUseProgram(0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

#if 0
void glGetTexImage (GLenum      target,
                    GLint       level,
                    GLenum      format, // GL will convert to this format
                    GLenum      type,   // Using this data type per-pixel
                    GLvoid *    img);
#endif

  double _mx, _my;
  glfwGetCursorPos(window, &_mx, &_my);
  int mx = floor(_mx), my = floor(_my);

  glBindTexture(GL_TEXTURE_2D, picker.frame.tbo);

  if (picker.buffer == nullptr) {
    picker.buffer = new GLubyte[width*height*3];
  }

  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, picker.buffer);
  
  glBindTexture(GL_TEXTURE_2D, 0);

  int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

  if (state == GLFW_PRESS) {
     if (!(mx < 0 || mx >= width || my < 0 || my >= height)){
	int index = mx + ((height - 1) - my) * width;

	unsigned char r = picker.buffer[index*3+0];
	unsigned char g = picker.buffer[index*3+1];
	unsigned char b = picker.buffer[index*3+2];

	auto view = reg->view<Selectable>();

	for (auto e : view) {
	   auto& select = reg->get<Selectable>(e);

	   const auto _r = (unsigned char)(select.unique_color.x * 255);
	   const auto _g = (unsigned char)(select.unique_color.y * 255);
	   const auto _b = (unsigned char)(select.unique_color.z * 255);

	   if (std::abs(_r - r) <= 1 && std::abs(_g - g) <= 1 && std::abs(_b - b) <= 1) {
	      select.selected = true;
	   } else {
	      select.selected = false;
	   }
	}
     }
  }
}
