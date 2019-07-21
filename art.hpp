#ifndef ART_H
#define ART_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <optional>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "game.hpp"
#include "graphics.hpp"

struct Mesh {
   std::vector<GLfloat> vertices, normals, uvs;
   std::vector<GLuint> indices;

   GLuint vao, vbo, nbo, ubo, ibo;
};

struct Texture {
   int width;
   int height;
   int channels;
   GLuint id;
};

struct Material {
   Texture texture;
   glm::vec4 region;
   glm::vec4 diffuse { glm::vec4(1, 1, 1, 1) };
};

struct Model {
   Model(Mesh* mesh, Material material={});
   Mesh* mesh;
   Material material;
};

struct Transform {
   glm::vec3 position {glm::vec3(0.0f)};
   glm::vec3 scale {glm::vec3(1.0f)};
   glm::quat orientation {glm::quat(1, 0, 0, 0)};
};

struct Framebuffer {
   GLuint fbo, tbo, dbo;
};

struct Camera {
   glm::quat orientation;
   glm::vec3 up {glm::vec3(0, 1, 0)};
   glm::vec3 position {glm::vec3(0, 0, 0)};

   void update(GLFWwindow* window);

   glm::mat4 get_view_matrix();

private:
   double last_m_x {0.0}, last_m_y {0.0};

   bool mouse_shown {false};
};

struct Skybox {
   GLuint id;
};

glm::mat4 get_transform_matrix(const Transform& t);

std::optional<Mesh*> load_mesh_file(const char* path);
std::optional<Texture> load_texture(const char* path);
std::optional<Skybox> load_skybox(const std::vector<std::string>& faces);

Framebuffer make_framebuffer(const int width, const int height);

struct Renderer {
   Renderer();

   GLuint cube_vao, cube_vbo;
   GLuint plane_vao, plane_vbo;

   ShaderProgram skybox_program;
};

void render_skybox(Renderer ren, Skybox skybox, glm::mat4 proj, glm::mat4 view);

#endif//ART_H
