$input v_color0, v_color1, v_texcoord0, v_texcoord1, v_texcoord7, v_normal0, v_position0

#include <bgfx_shader.sh>

SAMPLER2D(smplr, 0);
uniform vec4 twh;

float random(vec2 st) {
  return fract(sin(dot(st.xy,
          vec2(12.9898,78.233)))*
      43758.5453123);
}

void main()
{
  vec3 color;

  color = vec3(0.7, 0.7, 0.7);

  vec2 st = gl_FragCoord.xy;
  st = st / twh.yz;

  float x = st.x * 1.8 - 0.9;
  float y = st.y * 1.8 - 0.9;
  color = color * (1.0 - x * x) * (1.0 - y * y);

	gl_FragColor = vec4(color.xyz - random(st) * 0.1, 1.0);
}
