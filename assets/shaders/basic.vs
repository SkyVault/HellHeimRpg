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
