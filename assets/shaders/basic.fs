#version 330 core

in vec2 Uvs;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos = vec3(-1.2f, 1.0f, 2.0f);
uniform vec3 lightColor = vec3(1, 1, 1);

uniform vec3 diffuse = vec3(1.0, 1.0, 1.0);
uniform vec3 viewPos = vec3(0, 0, 0);

uniform sampler2D sampler;

void main() {
  // ambient
  float ambient_strength = 0.7;
  vec3 ambient_m = ambient_strength * lightColor;

  // diffuse
  vec3 norm = normalize(Normal);
  vec3 light_dir = normalize(lightPos - FragPos);
  float diff = max(dot(norm, light_dir), 0.0);
  vec3 diffuse_m = diff * lightColor;

  // specular
  float specular_strength = 0.5;
  vec3 view_dir = normalize(viewPos - FragPos);
  vec3 reflect_dir = reflect(-light_dir, norm);
  float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
  vec3 specular_m = specular_strength * spec * lightColor;

  vec4 texture_ = texture(sampler, Uvs);

  vec3 result = (ambient_m + diffuse_m + specular_m) * diffuse;

  gl_FragColor = vec4(result, 1.0) * texture_;
}

