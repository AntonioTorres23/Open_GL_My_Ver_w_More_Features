#version 330 core
layout (location = 0) in vec3 vertex_coordinates;

uniform mat4 transformation_matrix;

uniform mat4 view_matrix;

uniform mat4 perspective_matrix;

void main()
{
	gl_Position =  perspective_matrix * view_matrix * transformation_matrix * vec4(vertex_coordinates, 1.0f);
}
