#version 330

in vec2 coord;
out vec3 image;

uniform sampler2D first;
uniform sampler2D second;
uniform sampler2D third;
uniform sampler2D fourth;
uniform float size = 0.1;

vec2 transform(vec2 coord, vec2 size, vec2 offset) {
	return (coord - offset) / size;
}

bool range(vec2 coord) {
	bool x = 0 < coord.x && coord.x < 1;
	bool y = 0 < coord.y && coord.y < 1;
	return x && y;
}

void main() {
	for(int i = 0; i < 4; ++i) {
		vec2 coordinate = transform(coord, vec2(size), vec2(i * size + 0.02 * (i + 1), 0.02));
		if(range(coordinate)) {
			     if(i == 0) image = texture2D(first,  coordinate).rgb;
			else if(i == 1) image = texture2D(second, coordinate).rgb;
			else if(i == 2) image = texture2D(third,  coordinate).rgb;
			else if(i == 3) image = texture2D(fourth, coordinate).rgb;
			return;
		}
	}
	discard;
}
