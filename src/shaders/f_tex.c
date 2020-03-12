$input v_color0, v_normal0, v_position0, v_texcoord0

#include <bgfx_shader.sh>

SAMPLER2D(smplr, 0);
uniform vec4 twh;

void main()
{
  vec3 light_position = vec3(10.0, 10.0, -15.0);
  vec3 light_direction = normalize(light_position - v_position0);
  vec3 light_color = vec3(0.8, 0.8, 0.8);

  vec3 ambient = 0.3 * light_color;

  float diff = max(dot(v_normal0, light_direction), 0.0);
  vec3 diffuse = diff * light_color;

  vec3 color;

  if (v_texcoord0.x < 0) {
    color = v_color0.xyz;
  } else {
    color = v_color0.xyz + texture2D(smplr, v_texcoord0).xyz;
  }

	gl_FragColor = vec4(color * (ambient + diffuse), 1.0);
	/* gl_FragColor = vec4(vec3(v_texcoord0.xy, 0.0), 1.0); */
}
