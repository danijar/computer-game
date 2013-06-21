#version 330

in vec2 coord;
out vec3 image;

uniform sampler2D depth_tex;
uniform sampler2D normal_tex;
uniform vec2 frame_size;

float depth(in vec2 offset)
{
	return 1500.0 * texture2D(depth_tex, coord + offset / frame_size).r;
}

vec3 normal(in vec2 offset)
{
	return texture2D(normal_tex, coord + offset / frame_size).xyz;
}

void main()
{
	// depth

	float dc, dn, ds, de, dw;
	dc = depth(vec2( 0,  0));
	dn = depth(vec2( 0, +1));
	ds = depth(vec2( 0, -1));
	de = depth(vec2(+1,  0));
	dw = depth(vec2(-1,  0));
	
	float dvertical   = abs(dc - ((dn + ds) / 2));
	float dhorizontal = abs(dc - ((de + dw) / 2));

	// normals

	vec3 nc, nn, ns, ne, nw;
	nc = normal(vec2( 0,  0));
	nn = normal(vec2( 0, +1));
	ns = normal(vec2( 0, -1));
	ne = normal(vec2(+1,  0));
	nw = normal(vec2(-1,  0));

	float nvertical   = abs(dot(vec3(1), nc - ((nn + ns) / 2.0)));
	float nhorizontal = abs(dot(vec3(1), nc - ((ne + nw) / 2.0)));

	// result

	float vertical   = clamp((dvertical   + nvertical  ) / 2, 0, 1);
	float horizontal = clamp((dhorizontal + nhorizontal) / 2, 0, 1);
	float amount = (vertical + horizontal) / 2;
	float edge = clamp(abs(amount - vertical) + abs(amount - horizontal), 0, 1);
	float aliasing = (amount > 0.2 ? 1 : 0) * (1 - edge);

	image = vec3(horizontal, vertical, aliasing);
	// image = vec3(dhorizontal, dvertical, 0); // from depth only
	// image = vec3(nhorizontal, nvertical, 0); // from normals only
}
