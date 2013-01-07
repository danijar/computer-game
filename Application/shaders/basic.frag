#version 150

in vec3 f_normal;
in vec2 f_texcoord;

uniform sampler2D tex;

vec3 lightpos = vec3(0.5, 1.0, 1.5);
float amount = 0.6;

void main()
{
	vec3 diffuse = max(0.0, dot(normalize(f_normal), normalize(lightpos))) * amount + (1-amount);
    vec4 color = vec4(diffuse, 1.0);

    gl_FragColor = texture(tex, f_texcoord) * color;
}