#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 frag_color;
layout(location = 0) out vec4 color_output;

void main() {
  color_output = vec4(frag_color, 1.0);
}