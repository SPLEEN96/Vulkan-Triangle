#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D tex_sampler;

layout(location = 0) in vec3 frag_color;
layout(location = 1) in vec2 frag_texcoord;

layout(location = 0) out vec4 color_output;

void main() { color_output = texture(tex_sampler, frag_texcoord); }