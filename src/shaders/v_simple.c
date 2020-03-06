$input a_position, a_color0, a_normal, a_color2
$output v_color0, v_normal0, v_position0

#include <bgfx_shader.sh>

void main()
{
  vec3 pos = a_color2;

	gl_Position = mul(u_modelViewProj, vec4(a_position + pos, 1.0));
	v_color0 = a_color0;
  v_normal0 = a_normal;
	v_position0 = mul(u_model[0], vec4(a_position + pos, 0.0)).xyz;
}
