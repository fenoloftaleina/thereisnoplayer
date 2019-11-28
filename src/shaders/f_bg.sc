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

	gl_FragColor = vec4(vec3(1.0, 0.0, 0.0) * (ambient + diffuse), 1.0);


  vec2 st = gl_FragCoord.xy;
  st = st / twh.yz;

  vec4 no = vec4(0.3, 0.3, 0.3, 1.0);
  vec4 yes = vec4(0.25, 0.25, 0.25, 1.0);

  if (sin(st.x * 31.4) > 0.0) {
    gl_FragColor = no;
  } else {
    gl_FragColor = yes;
  }
}
