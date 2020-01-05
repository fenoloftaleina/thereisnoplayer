$input a_position, a_color0, a_normal, a_color2, a_color3, a_indices, a_weight
$output v_color0, v_normal0, v_position0

#include <bgfx_shader.sh>

uniform vec4 twh;

void main()
{
  vec3 pos1 = a_color2;
  vec3 pos2 = a_color3;
  vec3 from_vcp = a_indices;
  vec3 to_vcp = a_weight;

  vec3 pos = mix(pos1, pos2, smoothstep(from_vcp.z, to_vcp.z, twh.x));

	gl_Position = mul(u_modelViewProj, vec4(a_position + pos, 1.0));
	v_color0 = a_color0;
  v_normal0 = a_normal;
	v_position0 = mul(u_model[0], vec4(a_position + pos, 0.0)).xyz;
}
