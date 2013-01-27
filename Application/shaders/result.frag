#version 150

uniform sampler2D position_tex;
uniform sampler2D normal_tex;
uniform sampler2D albedo_tex;

in vec2 ftexcoord;

out vec4 frag_color;

void main()
{
    frag_color = vec4(texture(albedo_tex, ftexcoord).rgb, 0.6);
}
