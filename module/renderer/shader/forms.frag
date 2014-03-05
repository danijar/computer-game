#version 330

in vec3 fposition;
in vec3 fnormal;
in vec2 fcoord;

layout(location = 0) out vec3 position;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec3 diffuse;
layout(location = 3) out vec3 specular;

uniform sampler2D diffusemap;
uniform sampler2D normalmap;
uniform sampler2D specularmap;

uniform bool hasnormalmap   = false;
uniform bool hasspecularmap = false;

uniform float shininess = 0.0;

void main()
{
	position = fposition;

	diffuse = texture2D(diffusemap, fcoord).rgb;

	if (hasnormalmap)   normal = normalize(vec3(fnormal.xy + texture2D(normalmap, fcoord).xy, fnormal.z));
	else                normal = normalize(fnormal);

	if (hasspecularmap) specular = shininess * texture2D(specularmap, fcoord).xyz;
	else                specular = vec3(shininess);

	gl_FragDepth = clamp(-position.z / 1000.0f, 0, 1);
}
