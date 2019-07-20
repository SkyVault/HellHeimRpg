#version 330 core

layout (location = 0) in vec2 aVertex;
layout (location = 1) in vec2 aUvs;

out vec2 Uvs;

void main() {
    Uvs = aUvs;
    gl_Position = vec4(aVertex.x, aVertex.y, 0, 1);
}