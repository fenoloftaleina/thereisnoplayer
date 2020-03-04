$input v_texcoord0

#include <bgfx_shader.sh>

SAMPLER2D(smplr, 0);
uniform vec4 twh;

void main() {
   gl_FragColor = vec4(texture2D(smplr, v_texcoord0).xyz, 1.0);
}
