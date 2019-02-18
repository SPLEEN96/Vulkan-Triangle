#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D tex_sampler;

layout(location = 0) in vec3 frag_color;
layout(location = 1) in vec2 frag_texcoord;
layout(location = 2) in vec3 frag_normal;
layout(location = 3) in vec3 frag_pos;

layout(location = 0) out vec4 color_output;

void main() {
  // color_output = texture(tex_sampler, frag_texcoord);
  vec3 view_pos = vec3(0.0f, 0.f, 2.2f);
  vec3 obj_color = vec3(0.4f, 0.3f, 0.2f);
  vec3 light_color = vec3(1.f, 1.f, 1.f);
  vec3 light_pos = vec3(-3.f, 3.f, -3.f);

  vec3 normal = normalize(frag_normal);
  vec3 light_dir = normalize(light_pos - frag_pos);
  vec3 view_dir = normalize(view_pos - frag_pos);
  vec3 reflect_dir = reflect(-light_dir, normal);

  // float ambient_str = 0.3f;
  // vec3 ambient = ambient_str * light_color;

  // float diff = max(dot(normal, light_dir), 0);
  // vec3 diffuse = diff * light_color;

  // vec3 result = (ambient + diffuse) * obj_color;

  /* TOON */
  // Ambient
  float toon_ambient_str = 0.4f;
  vec3 toon_ambient = toon_ambient_str * vec3(0.24725, 0.1995, 0.0745);

  // Diffuse
  vec3 diff_color = vec3(0.75164, 0.60648, 0.22648);
  float diff = max(dot(normal, light_dir), 0);
  float toon_diff_intensity =
      smoothstep(0, 0.01, diff); // 1st arg: min, 2d arg: threshold, 3rd arg:
                                 // the value to be interpolated
  vec3 toon_diffuse = toon_diff_intensity * diff_color;

  // Specular
  vec3 spec_color = vec3(0.628281, 0.555802, 0.366065); // vec3(0.9f);
  float glossiness = 64;

  float spec = pow(dot(view_dir, reflect_dir), glossiness);
  float toon_spec_intensity = smoothstep(0.005f, 0.01f, spec);

  vec3 toon_specular = toon_spec_intensity * spec_color;

  // Rim Lighting
  float rim_amount = 0.716;
  float rim_light = 1 - dot(view_dir, normal);
  float rim_intensity =
      smoothstep(rim_amount - 0.01, rim_amount + 0.01, rim_light);
  // rim_light = rim_intensity * rim_light;

  vec3 result =
      (toon_ambient + toon_diffuse + toon_specular + rim_light) * obj_color;
  color_output = vec4(result, 1.f);
}