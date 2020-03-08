$input v_texcoord0

#include <bgfx_shader.sh>

SAMPLER2D(smplr, 0);
SAMPLER2D(smplr2, 1);
uniform vec4 twh;

void main() {
  float dx = 1.0 / twh.y;
  float dy = 1.0 / twh.z;
  float minus = -1.0;
  gl_FragColor = vec4(
      texture2D(smplr, v_texcoord0 + vec2(-dx, dy)).xyz * minus + texture2D(smplr, v_texcoord0 + vec2(0, dy)).xyz * minus + texture2D(smplr, v_texcoord0 + vec2(dx, dy)).xyz * minus +
      texture2D(smplr, v_texcoord0 + vec2(-dx, 0)).xyz * minus + texture2D(smplr, v_texcoord0).xyz * 8 + texture2D(smplr, v_texcoord0 + vec2(dx, 0)).xyz * minus +
      texture2D(smplr, v_texcoord0 + vec2(-dx, -dy)).xyz * minus + texture2D(smplr, v_texcoord0 + vec2(0, -dy)).xyz * minus + texture2D(smplr, v_texcoord0 + vec2(dx, -dy)).xyz * minus
      , 1.0);
}
