$input v_color0, v_normal0, v_position0

#include <bgfx_shader.sh>

float random(vec2 st) {
  return fract(sin(dot(st.xy,
          vec2(12.9898,78.233)))*
      43758.5453123);
}

uniform vec4 twh;
uniform vec4 alpha;

void main()
{
  vec2 st = gl_FragCoord.xy;
  //st = st / twh.yz;

  vec3 light_position = vec3(10.0, 10.0, -15.0);
  vec3 light_direction = normalize(light_position - v_position0);
  vec3 light_color = vec3(0.8, 0.8, 0.8);

  vec3 ambient = 0.3 * light_color;

  float diff = max(dot(v_normal0, light_direction), 0.0);
  vec3 diffuse = diff * light_color;

  gl_FragColor = vec4(v_color0.xyz * (ambient + diffuse) + vec3(mod(st.x, 2.0)) * 0.06, 1.0);
}
