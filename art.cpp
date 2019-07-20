#include "art.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Model::Model(Mesh* mesh, Material material):
   mesh(mesh),
   material(material)
{
   
}

glm::mat4 get_transform_matrix(const Transform& t) {
   glm::mat4 result = glm::translate(glm::mat4(1), t.position);
   result = result * glm::mat4_cast(t.orientation);
   result = glm::scale(result, t.scale);
   return result;
}

std::optional<Mesh*> load_mesh_file(const char* path) {
   Assimp::Importer importer;

   const auto* scene = importer.ReadFile(
      path, 
      aiProcess_Triangulate | aiProcess_FlipUVs);
   
   if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE
       || !scene->mRootNode) {
      return std::nullopt;
   }

   auto* mesh = new Mesh();

   const auto* node = scene->mRootNode;

   // This only handles one mesh
   // View this for multi mesh importing
   // https://learnopengl.com/Model-Loading/Model

   if (node->mNumChildren == 0) {
      std::cout << "No meshes" << std::endl;
      return std::nullopt;
   }

   node = node->mChildren[0];

   const auto* aimesh = scene->mMeshes[node->mMeshes[0]];

   for (unsigned int i = 0; i < aimesh->mNumVertices; i++) {
      mesh->vertices.emplace_back(aimesh->mVertices[i].x);
      mesh->vertices.emplace_back(aimesh->mVertices[i].y);
      mesh->vertices.emplace_back(aimesh->mVertices[i].z);

      mesh->normals.emplace_back(aimesh->mNormals[i].x);
      mesh->normals.emplace_back(aimesh->mNormals[i].y);
      mesh->normals.emplace_back(aimesh->mNormals[i].z);

      if (aimesh->mTextureCoords[0]) {
	// Only handle 1 texture
	mesh->uvs.emplace_back(aimesh->mTextureCoords[0][i].x);
	mesh->uvs.emplace_back(aimesh->mTextureCoords[0][i].y);
      } else {
	mesh->uvs.emplace_back(0.0f);
	mesh->uvs.emplace_back(0.0f);
      }
   }

   for (unsigned int i = 0; i < aimesh->mNumFaces; i++) {
      const auto face = aimesh->mFaces[i];

      for (unsigned int j = 0; j < face.mNumIndices; j++) {
	 mesh->indices.emplace_back(face.mIndices[j]);
      }
   }

   // OpenGL buffers and stuff
   glGenVertexArrays(1, &mesh->vao);
   glBindVertexArray(mesh->vao);

   // Vertices
   glGenBuffers(1, &mesh->vbo);
   glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*mesh->vertices.size(), mesh->vertices.data(), GL_STATIC_DRAW);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
   glBindBuffer(GL_ARRAY_BUFFER, 0);

   // Normals
   glGenBuffers(1, &mesh->nbo);
   glBindBuffer(GL_ARRAY_BUFFER, mesh->nbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*mesh->normals.size(), mesh->normals.data(), GL_STATIC_DRAW);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
   glBindBuffer(GL_ARRAY_BUFFER, 0);

   // Uvs
   glGenBuffers(1, &mesh->ubo);
   glBindBuffer(GL_ARRAY_BUFFER, mesh->ubo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*mesh->uvs.size(), mesh->uvs.data(), GL_STATIC_DRAW);
   glEnableVertexAttribArray(2);
   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
   glBindBuffer(GL_ARRAY_BUFFER, 0);

   // Elements
   glGenBuffers(1, &mesh->ibo);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*mesh->indices.size(), mesh->indices.data(), GL_STATIC_DRAW);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

   glBindVertexArray(0);

   return mesh;
}

std::optional<Texture> load_texture(const char* path) {
   Texture result;
   auto* data = stbi_load(path, &result.width, &result.height, &result.channels, STBI_default);

   if (!data) return std::nullopt;

   glGenTextures(1, &result.id);
   glBindTexture(GL_TEXTURE_2D, result.id);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

   std::cout << "-> " << result.channels << std::endl;

   switch (result.channels) {
      case STBI_grey: {
	 glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, result.width, result.height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
	 break;
      }

      case STBI_grey_alpha: {
	 glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, result.width, result.height, 0, GL_RG, GL_UNSIGNED_BYTE, data);
	 break;
      }

      case STBI_rgb: {
	 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, result.width, result.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	 break;
      }

      case STBI_rgb_alpha: {
	 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, result.width, result.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	 break;
      }
   }

   glBindTexture(GL_TEXTURE_2D, 0);

   stbi_image_free(data);

   return result;
}

void Camera::update(GLFWwindow* window) {
   double mx, my;
   glfwGetCursorPos(window, &mx, &my);

   if (glfwGetKey(window, GLFW_KEY_LEFT_ALT)) {
     glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
     last_m_x = mx;
     last_m_y = my;
     return;
   } else {
     glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
   }

   const float dt = Game::it()->get_clock().dt;

   constexpr auto sensitivity {0.01};

   const auto delta_m_x = (mx - last_m_x) * sensitivity;
   const auto delta_m_y = (my - last_m_y) * sensitivity;

   last_m_x = mx;
   last_m_y = my;

   const auto key_quat = glm::quat(glm::vec3(delta_m_y, delta_m_x, 0));

   const auto yaw_key_quat = glm::angleAxis((float)delta_m_x, glm::vec3(0, 1, 0));
   const auto pitch_key_quat = glm::angleAxis((float)delta_m_y, glm::vec3(1, 0, 0));

   orientation = pitch_key_quat * orientation * yaw_key_quat;

   orientation = glm::normalize(orientation);

   const auto forward = glm::vec3(0, 0, 1) * orientation;
   const auto right = glm::vec3(1, 0, 0) * orientation;

   const auto speed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)?3.f:1.f;

   if (glfwGetKey(window, GLFW_KEY_W)) {
      position += speed * dt * forward;
   }

   if (glfwGetKey(window, GLFW_KEY_S)) {
      position -= speed * dt * forward;
   }

   if (glfwGetKey(window, GLFW_KEY_A)) {
      position += speed * dt * right;
   }

   if (glfwGetKey(window, GLFW_KEY_D)) {
      position -= speed * dt * right;
   }
}

glm::mat4 Camera::get_view_matrix() {
   glm::mat4 trans = glm::translate(glm::mat4(1), position);
   glm::mat4 rotation = glm::mat4_cast(orientation);

   return rotation * trans;
}

Framebuffer make_framebuffer(const int width, const int height) {
   GLuint fbo, tbo;

   glGenFramebuffers(1, &fbo);
   glBindFramebuffer(GL_FRAMEBUFFER, fbo);

   glGenTextures(1, &tbo);
   glBindTexture(GL_TEXTURE_2D,tbo);

   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

   // The depth buffer
   GLuint dbo;
   glGenRenderbuffers(1, &dbo);
   glBindRenderbuffer(GL_RENDERBUFFER,dbo);
   glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
   glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,dbo);

    // Set "renderedTexture" as our colour attachement #0
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,tbo, 0);

    // Set the list of draw buffers.
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return Framebuffer{fbo, tbo, dbo};
}
