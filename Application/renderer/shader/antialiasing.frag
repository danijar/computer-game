#version 330

in vec2 coord;
out vec3 result;

uniform sampler2D image_tex;
uniform sampler2D blur_tex;
uniform sampler2D edge_tex;

void main()
{
	vec3 edge  = texture2D(edge_tex,  coord).xyz;
	vec3 image = texture2D(image_tex, coord).rgb;
	vec3 blur  = texture2D(blur_tex,  coord).rgb;
	float presence = clamp(0.8 * (edge.x + edge.y + edge.z), 0, 1);

	result = mix(image, blur, presence);
	//result = vec4(vec3(presence), 1.0); // only aliasing
	//result = blur;                      // only blur
	result = image;                     // only image
}
