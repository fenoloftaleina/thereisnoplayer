$input a_position, a_color0, a_normal, a_color2, a_texcoord0
$output v_texcoord0

#include <bgfx_shader.sh>

void main()
{
  vec3 pos = a_color2;

	gl_Position = mul(u_modelViewProj, vec4(a_position + pos, 1.0));
	// gl_Position = vec4(a_position, 1.0);
  v_texcoord0 = a_texcoord0;
}
