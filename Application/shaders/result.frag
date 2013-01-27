#version 150

uniform sampler2D position_tex;
uniform sampler2D normal_tex;
uniform sampler2D albedo_tex;

in vec2 ftexcoord;

out vec4 frag_color;

void main()
{
	float depth = texture(position_tex, ftexcoord).z;

	vec3 position = texture(position_tex, ftexcoord).xyz;
	vec3 normal = texture(normal_tex, ftexcoord).xyz;
	vec3 color = texture(albedo_tex, ftexcoord).rgb;

    frag_color = vec4(color, 1.0);
}
