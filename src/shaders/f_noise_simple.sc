$input v_color0, v_normal0

#include <bgfx_shader.sh>

float random(vec2 st) {
  return fract(sin(dot(st.xy,
          vec2(12.9898,78.233)))*
      43758.5453123);
}

uniform vec4 twh;

void main()
{
  vec2 st = gl_FragCoord.xy;
  st = st / twh.yz;
  st.x = st.x + twh.x * 0.0001;

	gl_FragColor = vec4(vec3(0.267, 0.20, 0.333) - vec3(pow(random(st), 35.0)), 1.0);
}
