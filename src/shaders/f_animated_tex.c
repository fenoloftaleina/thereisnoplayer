$input v_color0, v_color1, v_texcoord0, v_texcoord1, v_texcoord7, v_normal0, v_position0

#include <bgfx_shader.sh>

SAMPLER2D(smplr, 0);
uniform vec4 twh;

void main()
{
  vec3 color;

  if (v_texcoord0.x < 0) {
    color = mix(v_color0, v_color1, v_texcoord7.x);
  } else {
    color = mix(
        v_color0 + texture2D(smplr, v_texcoord0).xyz,
        v_color1 + texture2D(smplr, v_texcoord1).xyz,
        v_texcoord7.x
        );
  }

  vec3 light_position = vec3(10.0, 10.0, -15.0);
  vec3 light_direction = normalize(light_position - v_position0);
  vec3 light_color = vec3(0.8, 0.8, 0.8);

  vec3 ambient = 0.3 * light_color;

  float diff = max(dot(v_normal0, light_direction), 0.0);
  vec3 diffuse = diff * light_color;

	/* gl_FragColor = vec4(color.xyz * (ambient + diffuse), 1.0); */


  /* vec3 n = abs(v_normal0); */
  /* color = vec3(n.x * 0.8 + n.y * 0.9 + n.z * 0.7); */

	gl_FragColor = vec4(color.xyz, 1.0);
}
