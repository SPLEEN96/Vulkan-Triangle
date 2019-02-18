#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform uniform_buffer_obj {
  mat4 model;
  mat4 view;
  mat4 proj;
}
ubo;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_color;
layout(location = 2) in vec2 in_texcoord;
layout(location = 3) in vec3 in_normals;

layout(location = 0) out vec3 frag_color;
layout(location = 1) out vec2 frag_texcoord;
layout(location = 2) out vec3 frag_normal;
layout(location = 3) out vec3 frag_pos;

void main() {
  gl_Position = ubo.proj * ubo.view * ubo.model * vec4(in_position, 1.0);
  frag_color = in_color;
  frag_texcoord = in_texcoord;

  frag_normal = mat3(transpose(inverse(ubo.model)))*in_normals;
  frag_pos = vec3(ubo.model * vec4(in_position, 1.f));
}