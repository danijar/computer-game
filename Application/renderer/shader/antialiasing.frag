#version 330

in vec2 coord;
out vec4 result;

uniform sampler2D image_tex;
uniform sampler2D blur_u_tex;
uniform sampler2D blur_v_tex;
uniform sampler2D edge_tex;
uniform vec2 frameBufSize;

vec3 edge(in float u, in float v)
{
	return texture2D(edge_tex, coord + vec2(u, v) / frameBufSize).xyz;
}

void main()
{
	float edge_u = (edge(0, 0).y + 0.5 * edge(0, 1).y + 0.5 * edge(0,-1).y + 0.25 * edge(0, 2).y + 0.25 * edge(0,-2).y) / 2.5;
	float edge_v = (edge(0, 0).x + 0.5 * edge(1, 0).x + 0.5 * edge(-1,0).x + 0.25 * edge(2, 0).x + 0.25 * edge(-2,0).x) / 2.5;

	vec4 image = texture2D(image_tex, coord);
	vec4 blur = mix(texture2D(blur_v_tex, coord), texture2D(blur_u_tex, coord), 0.5 + edge_u/2 - edge_v/2);
	
	result = mix(image, blur, clamp(4 * edge_v + edge_u, 0, 1));
	//result = vec4(edge_u, edge_v, 0.0, 1.0); // aliasing only
	//result = texture2D(image_tex, coord);    // image only
}
