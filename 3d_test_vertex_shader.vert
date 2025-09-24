#version 330 core

layout (location = 0) in vec3 vertex_coordinates;

layout (location = 1) in vec2 texture_coordinates;

uniform mat4 transformation_matrix;

uniform mat4 view_matrix;

uniform mat4 perspective_matrix;

out vec2 texture_coordinates_to_fragment_shader;

void main()
{
	
	gl_Position =  perspective_matrix * view_matrix * transformation_matrix * vec4(vertex_coordinates, 1.0f);
	
	texture_coordinates_to_fragment_shader = texture_coordinates;
}
