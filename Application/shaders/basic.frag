#version 330

uniform sampler2D tex;

in vec3 fnormal;
in vec2 ftexcoord;

out vec3 color;

void main()
{
	color = texture2D(tex, ftexcoord).rgb;

	vec3 ambient_color = vec3(0.75, 0.65, 0.50);
	float ambient_amount = 0.2;
	color += ambient_color * ambient_amount;

	vec3 light_pos = vec3(0.5, 1.0, 1.5);
	float light_amount = 0.5;
	vec3 light = vec3(max(0.0, dot(normalize(fnormal), normalize(light_pos))));
	color *= light * light_amount + (1 - light_amount);
}