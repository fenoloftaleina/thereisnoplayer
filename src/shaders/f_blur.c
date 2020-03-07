$input v_texcoord0

#include <bgfx_shader.sh>

SAMPLER2D(smplr, 0);
SAMPLER2D(smplr2, 1);
uniform vec4 twh;

void main() {
  float dx = 1.0 / twh.y;
  float dy = 1.0 / twh.z;
  gl_FragColor = vec4(
      texture2D(smplr, v_texcoord0 + vec2(-dx, dy)).xyz + texture2D(smplr, v_texcoord0 + vec2(0, dy)).xyz * 2 + texture2D(smplr, v_texcoord0 + vec2(dx, dy)).xyz +
      texture2D(smplr, v_texcoord0 + vec2(-dx, 0)).xyz * 2 + texture2D(smplr, v_texcoord0).xyz * 4 + texture2D(smplr, v_texcoord0 + vec2(dx, 0)).xyz * 2 +
      texture2D(smplr, v_texcoord0 + vec2(-dx, -dy)).xyz + texture2D(smplr, v_texcoord0 + vec2(0, -dy)).xyz * 2 + texture2D(smplr, v_texcoord0 + vec2(dx, -dy)).xyz
      , 1.0);
}
