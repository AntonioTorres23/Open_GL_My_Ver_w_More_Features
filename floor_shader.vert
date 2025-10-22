#version 330 core
layout (location = 0) in vec3 vertex_coordinates;
layout (location = 1) in vec3 normal_coordiantes;
layout (location = 2) in vec2 texture_coordinates;

out vec2 frag_texture_coordinates;

uniform mat4 transformation_matrix;

uniform mat4 view_matrix;

uniform mat4 perspective_matrix;

void main()
{
	frag_texture_coordinates = texture_coordinates;
	gl_Position =  perspective_matrix * view_matrix * transformation_matrix * vec4(vertex_coordinates, 1.0f);
}
