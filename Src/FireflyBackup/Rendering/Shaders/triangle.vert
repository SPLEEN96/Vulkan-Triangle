#version 450

layout(location = 0) out vec4 frag_colors;

const vec2 vertices[] = {vec2(0.f, 0.5f), vec2(0.35f, -0.5f),
                         vec2(-0.35f, -0.5f)};

const vec3 colors[] = {vec3(1.f, 0.f, 0.f), vec3(0.f, 1.f, 0.f),
                       vec3(0.f, 0.f, 1.f)};

void main() {
  gl_Position = vec4(vertices[gl_VertexIndex], 0.f, 1.f);
  frag_colors = vec4(colors[gl_VertexIndex], 1.f);
}