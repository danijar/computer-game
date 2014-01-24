#version 330

in vec3 fposition;
in vec3 fnormal;
in vec2 fcoord;

layout(location = 0) out vec3 position;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec3 albedo;
layout(location = 3) out vec3 specular;

uniform sampler2D mapdiffuse;
uniform sampler2D mapnormal;

uniform int hasmapnormal = 0;
uniform float shininess = 0.0;

void main()
{
	position = fposition;

	if (hasmapnormal == 1) {
		vec3 normalmap = texture2D(mapnormal, fcoord).xyz;
		normal = normalize(vec3(fnormal.xy + normalmap.xy, fnormal.z));
	} else {
		normal = normalize(fnormal);
	}

	albedo = texture2D(mapdiffuse, fcoord).rgb;

	specular = vec3(shininess);

	gl_FragDepth = clamp(-position.z / 1000.0f, 0, 1);
}
