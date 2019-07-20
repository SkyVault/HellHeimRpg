#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <magic_enum.hpp>

using namespace magic_enum::ops;

#include "graphics.hpp"
#include "game.hpp"
#include "art.hpp"
#include "picker.hpp"

std::unique_ptr<Game> Game::instance;
std::once_flag Game::onceFlag;

struct TestLayer : Layer {
  void act(LayerState state) override {
  }
};

int main() {
  if (!glfwInit()) return -1;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  constexpr float width = 1280.0f;
  constexpr float height = 720.0f;

  auto* window = glfwCreateWindow((int)width, (int)height, "DevWindow", nullptr, nullptr);

  if (!window) return -1;

  glfwMakeContextCurrent(window);

  if (glewInit() != GLEW_OK){
    printf("Error initializing glew\n");
  }

  // Set up opengl
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  //glDepthFunc(GL_ALWAYS);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  const auto FB = make_framebuffer(width, height);

  float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

  unsigned int quadVAO, quadVBO;
  glGenVertexArrays(1, &quadVAO);
  glGenBuffers(1, &quadVBO);
  glBindVertexArray(quadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

  const auto program = load_shader_program_vf_file(
			    "./assets/shaders/basic.vs",
			    "./assets/shaders/basic.fs");

  const auto screen_program = load_shader_program_vf_file(
			    "./assets/shaders/screen.vs",
			    "./assets/shaders/screen.fs");

  const auto projection = glm::perspective((float)(M_PI)/3.0f, width/height, 0.01f, 400.0f);
  const auto projection_loc = program.get_loc("projection");
  const auto view_loc = program.get_loc("view");
  const auto model_loc = program.get_loc("model");
  const auto diffuse_loc = program.get_loc("diffuse");

  Texture texture;

  if (auto texture_op = load_texture("assets/textures/lolwut.png")) {
    texture = *texture_op;
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }

  Camera* camera = new Camera();

  // Test entities
  {
    Mesh* mesh {nullptr};
    if (auto mesh_op = load_mesh_file("assets/meshes/test_scene.obj")) {
	mesh = *mesh_op;
    } else {
	std::cout << "Failed to load mesh" << std::endl;
    }

    Texture texture;
    if (auto texture_op = load_texture("assets/textures/texture_set_1.png")) {
	texture = *texture_op;
    } else {
	std::cout << "Failed to load texture" << std::endl;
    }

    auto* reg = Game::it()->get_registry();
    auto e = reg->create();
    reg->assign<Model>(e, mesh);
    reg->assign<Transform>(e);
    reg->assign<Material>(e);
    reg->assign<Selectable>(e);

    auto& trans = reg->get<Transform>(e);
    trans.position = glm::vec3(0, -1, -3);

    auto& material = reg->get<Material>(e);
    material.texture = texture;
  }

  {
    Mesh* mesh {nullptr};
    if (auto mesh_op = load_mesh_file("assets/meshes/house.obj")) {
	mesh = *mesh_op;
    } else {
	std::cout << "Failed to load mesh" << std::endl;
    }

    auto* reg = Game::it()->get_registry();
    auto e = reg->create();
    reg->assign<Model>(e, mesh);
    reg->assign<Transform>(e);
    reg->assign<Selectable>(e);

    auto& trans = reg->get<Transform>(e);
    trans.position = glm::vec3(0, -1, -3);
  }

  {
    Mesh* mesh {nullptr};
    if (auto mesh_op = load_mesh_file("assets/meshes/house.obj")) {
	mesh = *mesh_op;
    } else {
	std::cout << "Failed to load mesh" << std::endl;
    }

    auto* reg = Game::it()->get_registry();
    auto e = reg->create();
    reg->assign<Model>(e, mesh);
    reg->assign<Transform>(e);
    reg->assign<Selectable>(e);

    auto& trans = reg->get<Transform>(e);
    trans.position = glm::vec3(9, -1, 9);
  }

  // Picker / Editor
  Picker picker(width, height);

  Game::it()->pushLayer(new TestLayer());

  while (glfwWindowShouldClose(window) == false) {
    glfwPollEvents();

    Game::it()->update();
    camera->update(window);

    // Draw
    glClearColor(0.2, 0.2, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, FB.fbo);
    glClearColor(0.2, 0.2, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, width, height);

    glUseProgram(program.program_id);

    program.set_uniform(projection_loc, projection);
    program.set_uniform(view_loc, camera->get_view_matrix());

    {
	auto* reg = Game::it()->get_registry();

	reg->view<Model, Transform>().each([&](auto ent, auto& model, auto& trans) {
	  const auto& model_trans = get_transform_matrix(trans);
	  const auto* mesh = model.mesh;

	  if (reg->has<Material>(ent)) {
	    auto& material = reg->get<Material>(ent);
	    glBindTexture(GL_TEXTURE_2D, material.texture.id);
	  } else {
	    glBindTexture(GL_TEXTURE_2D, texture.id);
	  }
	  program.set_uniform(model_loc, model_trans);

	  program.set_uniform(diffuse_loc, glm::vec3(1, 1, 1));

	  glBindVertexArray(mesh->vao);
	  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);

	  if (reg->has<Selectable>(ent)) {
	    auto& select = reg->get<Selectable>(ent);
	    if (select.selected) {
	      auto v = (0.5 * cos(Game::it()->get_clock().timer * 10.0f)) + 1.0;
	      program.set_uniform(diffuse_loc, glm::vec3(1, v, v));
	    }
	  }

	  glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, (void*)0);

	  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	  glBindVertexArray(0);


	  glBindTexture(GL_TEXTURE_2D, 0);
	});
    }

    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    handle_picking(
	picker,
	Game::it()->get_registry(),
	window,
	projection,
	camera->get_view_matrix(),
	width,
	height);

    glUseProgram(screen_program.program_id);
    glBindTexture(GL_TEXTURE_2D, FB.tbo);
    //glBindTexture(GL_TEXTURE_2D, picker.frame.tbo);
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);

    glfwSwapBuffers(window);
  }
  
  glfwTerminate();
}
