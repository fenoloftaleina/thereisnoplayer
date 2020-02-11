$input v_texcoord0

#include <bgfx_shader.sh>

SAMPLER2D(smplr, 0);

void main()
{
	gl_FragColor = texture2D(smplr, v_texcoord0);
  // gl_FragColor = vec4(0.5, 0.7, 0.9, 1.0);
}
