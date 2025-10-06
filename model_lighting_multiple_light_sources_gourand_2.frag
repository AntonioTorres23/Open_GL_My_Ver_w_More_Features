#version 330 core
out vec4 color_output;

in float directional_lighting;

in vec3 lighting;

in vec2 fragment_texture_coordinates;

uniform vec3 color_of_light;

void main()
{
	color_output = vec4(lighting, 1.0);
}