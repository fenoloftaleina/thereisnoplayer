$input v_color0, v_normal0, v_position0

#include <bgfx_shader.sh>

uniform vec4 twh;

void main()
{
	gl_FragColor = vec4(v_color0.xyz, 1.0);
}
