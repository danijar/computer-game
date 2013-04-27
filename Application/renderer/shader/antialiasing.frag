#version 330

in vec2 coord;
out vec4 result;

uniform sampler2D image_tex;
uniform sampler2D blur_tex;
uniform sampler2D edge_tex;

void main()
{
	vec4 edge  = texture2D(edge_tex,  coord);
	vec4 image = texture2D(image_tex, coord);
	vec4 blur  = texture2D(blur_tex,  coord);
	float presence = clamp(0.8 * (edge.x + edge.y + edge.z), 0, 1);

	result = mix(image, blur, presence);
	//result = vec4(vec3(presence), 1.0); // only aliasing
	//result = blur;                      // only blur
	//result = image;                     // only image
}
