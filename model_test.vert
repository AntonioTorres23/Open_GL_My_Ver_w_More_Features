#version 330 core
layout (location = 0) in vec3 vector_position_coordinates;

layout (location = 1) in vec3 normal_position_coordinates;

layout (location = 2) in vec2 texture_position_coordinates;

out vec2 fragment_texture_coordinates;

uniform mat4 transformation_matrix;

uniform mat4 view_matrix;

uniform mat4 perspective_matrix;

void main()
{
	fragment_texture_coordinates = texture_position_coordinates;

	gl_Position =   perspective_matrix * view_matrix * transformation_matrix * vec4(vector_position_coordinates, 1.0);


}