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
  vec2 st = gl_FragCoord.xy;
  st = st / twh.yz;

	/* gl_FragColor = vec4(v_color0.xyz - random(st) * 0.17, 1.0); */
	gl_FragColor = vec4(v_color0.xyz, 1.0);
}
