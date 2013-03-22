#version 330

in vec2 coord;
out vec4 image;

uniform sampler2D image_tex;
uniform sampler2D position_tex;
uniform sampler2D normal_tex;
uniform sampler2D noise_tex;
// uniform vec2 frameBufSize;


void main()
{
    image = texture2D(image_tex, coord);
}
