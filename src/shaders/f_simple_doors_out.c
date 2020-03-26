$input v_color0, v_normal0, v_position0

#include <bgfx_shader.sh>

uniform vec4 twh;
uniform vec4 doors[21];

void main()
{
  vec3 light_position = vec3(10.0, 10.0, -15.0);
  vec3 light_direction = normalize(light_position - v_position0);
  vec3 light_color = vec3(0.8, 0.8, 0.8);

  vec3 ambient = 0.3 * light_color;

  float diff = max(dot(v_normal0, light_direction), 0.0);
  vec3 diffuse = diff * light_color;

  vec4 color = vec4(v_color0.xyz * (ambient + diffuse), 0.0);

  int doors_count = (int)doors[0].x;
  if (doors_count > 0) {
    for (int i = 1; i < doors_count + 1; ++i) {
      if (abs(v_position0.x - doors[i].x) < 1.0 &&
          abs(v_position0.z - doors[i].z) < 1.0) {
        color.a = 0.7;
      }
    }
  }

  gl_FragColor = color;
}
