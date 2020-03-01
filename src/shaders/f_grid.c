$input v_color0, v_normal0, v_position0

#include <bgfx_shader.sh>

uniform vec4 twh;

void main()
{
  vec3 light_position = vec3(10.0, 10.0, -15.0);
  vec3 light_direction = normalize(light_position - v_position0);
  vec3 light_color = vec3(0.8, 0.8, 0.8);

  vec3 ambient = 0.3 * light_color;

  float diff = max(dot(v_normal0, light_direction), 0.0);
  vec3 diffuse = diff * light_color;

  vec3 col = vec3(1, 1, 1);

  // vec2 uv = gl_FragCoord.xy / twh.yz;
  // vec2 uv = v_texcoord7;
  vec2 uv = v_position0.xz;

  float size = 2.0;   // size of the tile
  float edge = size/25.0; // size of the edge
  uv = (mod(uv, size) - mod(uv - edge, size) - edge) * 1.0/size;
  col = vec3(0.9 - length(uv) * 0.5);

  // col = vec3(v_position0.x, 0.5, 0.5);
  // col = vec3(1.0, uv.y, 1.0);


  // col = vec3(mod(v_position0.xz, 2.0) * 0.5, 0.0);

	// gl_FragColor = vec4(col * (ambient + diffuse), 1.0);
  gl_FragColor = vec4(col, 1.0);
}
