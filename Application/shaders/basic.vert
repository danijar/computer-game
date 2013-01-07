#version 150

in vec3 vertex;
in vec3 normal;
in vec2 texcoord;

out vec3 f_normal;
out vec2 f_texcoord;

uniform mat4 vertex_mat;
uniform mat4 normal_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;

void main()
{
	f_normal   = vec3(normal_mat * vec4(normal, 1.0));
    f_texcoord = texcoord;

    gl_Position = proj_mat * view_mat * vertex_mat * vec4(vertex, 1.0);
}