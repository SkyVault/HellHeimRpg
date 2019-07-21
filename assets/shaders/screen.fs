#version 330 core

in vec2 Uvs;

uniform sampler2D screentexture;

uniform vec2 resolution = vec2(320, 180);
uniform float amount = 320;

void main() {
    float d = 1.0 / amount;
    float ar = resolution.x / resolution.y;
    float u = floor( Uvs.x / d ) * d;
    d = ar / amount;
    float v = floor( Uvs.y / d ) * d;
    gl_FragColor = texture2D( screentexture, vec2( u, v ) );
}