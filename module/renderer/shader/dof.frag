#version 330

in vec2 coord;
out vec3 result;

uniform sampler2D colors;
uniform sampler2D blurs;
uniform sampler2D depths;

uniform float close = 5;

void main()
{
	vec3 color = texture2D(colors, coord).rgb;
	float depth = texture2D(depths, coord).r;
	float focus = texture2D(depths, vec2(0.5)).r;
	vec3 blur = texture2D(blurs, coord).rgb;

	// should be done by stencil
	if (depth > 0.9999) { result = color; return; }

	// restrict possible focus range
	depth = min(depth, 0.1);
	focus = min(focus, 0.1);

	//float distance = abs(depth - focus);
	float amount = 0.001 * abs(focus - depth) / (depth * focus);
	amount = clamp(close * amount, 0, 1);

	// visualize by red color
	// blur = vec3(1, 0, 0);

	result = mix(color, blur, amount);
}
