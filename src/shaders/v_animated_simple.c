$input a_position, a_color0, a_normal,  a_tangent, a_color1, a_bitangent,  a_color2, a_color3,  a_indices, a_weight
$output v_color0, v_normal0, v_position0

#include <bgfx_shader.sh>

uniform vec4 twh;

void main()
{
  vec3 a_position2 = a_tangent;
  vec3 a_normal2 = a_bitangent;

  vec3 pos1 = a_color2;
  vec3 pos2 = a_color3;

  vec3 from_vcp = a_indices;
  vec3 to_vcp = a_weight;

  vec3 position = mix(a_position, a_position2, smoothstep(from_vcp.x, to_vcp.x, twh.x));
  vec4 col = mix(a_color0, a_color1, smoothstep(from_vcp.y, to_vcp.y, twh.x));
  vec3 pos = mix(pos1, pos2, smoothstep(from_vcp.z, to_vcp.z, twh.x));

  vec3 end_pos = position + pos;
  end_pos.y = end_pos.y - (end_pos.x * end_pos.x * 0.01 + end_pos.z * end_pos.z * 0.01);

	gl_Position = mul(u_modelViewProj, vec4(end_pos, 1.0));
	v_color0 = col;
  v_normal0 = a_normal;
	v_position0 = mul(u_model[0], vec4(end_pos, 0.0)).xyz;
}
