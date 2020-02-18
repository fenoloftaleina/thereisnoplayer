$input v_texcoord0

#include <bgfx_shader.sh>

SAMPLER2D(smplr, 0);

void main()
{
  float e = 0.005;

	gl_FragColor =
  texture2D(smplr, v_texcoord0 + vec2(-e, e)) / 16.0 + texture2D(smplr, v_texcoord0 + vec2(0, e)) / 8.0 + texture2D(smplr, v_texcoord0 + vec2(e, e)) / 16.0 +
  texture2D(smplr, v_texcoord0 + vec2(-e, 0)) / 8.0 + texture2D(smplr, v_texcoord0) / 4.0 + texture2D(smplr, v_texcoord0 + vec2(e, 0)) / 8.0 +
  texture2D(smplr, v_texcoord0 + vec2(-e, -e)) / 16.0 + texture2D(smplr, v_texcoord0 + vec2(0, -e)) / 8.0 + texture2D(smplr, v_texcoord0 + vec2(e, -e)) / 16.0;

  // gl_FragColor = vec4(0.5, 0.7, 0.9, 1.0);
}
