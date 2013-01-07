#version 150

in vec2 f_texcoord;
in vec4 f_color;

uniform sampler2D tex;

void main()
{
    gl_FragColor = texture(tex, f_texcoord) * f_color;
}