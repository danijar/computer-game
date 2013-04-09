#version 330

in vec2 coord;
out vec4 image;

uniform sampler2D position_tex;
uniform sampler2D normal_tex;
uniform vec2 frameBufSize;

float depth(in vec2 offset)
{
	return texture2D(position_tex, coord + offset / frameBufSize).z;
}

void normal(out vec3 value, in vec2 offset)
{
	value = texture2D(normal_tex, coord + offset / frameBufSize).xyz;
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
	
	float dvertical   = /*abs*/(dc - ((dn + ds) / 2));
	float dhorizontal = /*abs*/(dc - ((de + dw) / 2));

	// normals

	vec3 nc, nn, ns, ne, nw;
	normal(nc, vec2( 0,  0));
	normal(nn, vec2( 0, +1));
	normal(ns, vec2( 0, -1));
	normal(ne, vec2(+1,  0));
	normal(nw, vec2(-1,  0));

	float nvertical   = /*abs*/(dot(vec3(1), nc - ((nn + ns) / 2.0)));
	float nhorizontal = /*abs*/(dot(vec3(1), nc - ((ne + nw) / 2.0)));

	// result

	float vertical   = clamp((dvertical   + nvertical  ) / 2, 0, 1);
	float horizontal = clamp((dhorizontal + nhorizontal) / 2, 0, 1);
	float amount = (vertical + horizontal) / 2;
	float edge = clamp(abs(amount - vertical) + abs(amount - horizontal), 0, 1);
	float aliasing = (amount > 0.2 ? 1 : 0) * (1 - edge);

	image = vec4(horizontal, vertical, aliasing, 1.0);
}
