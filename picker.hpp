#ifndef PICKER_H
#define PICKER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <cstdlib>

#include "art.hpp"
#include "graphics.hpp"

struct Picker {
   Picker(const int width, const int height);
   Framebuffer frame;
   ShaderProgram program;

   GLuint diffuse_loc, projection_loc, view_loc, model_loc;
   GLubyte* buffer {nullptr};
};

struct Selectable {
   Selectable();

   bool selected { false };

   glm::vec4 unique_color;
};

void handle_picking(Picker& picker, entt::registry* reg, GLFWwindow* window, glm::mat4 proj, glm::mat4 view, const int width, const int height);

#endif//PICKER_H
