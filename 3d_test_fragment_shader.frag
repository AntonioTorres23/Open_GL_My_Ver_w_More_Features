#version 330 core
out vec4 color;

in vec2 texture_coordinates_to_fragment_shader;


float ambient = 0.50; 

uniform sampler2D tex; 

uniform sampler2D layer_tex;

void main()
{
	color = mix(texture(tex, texture_coordinates_to_fragment_shader), texture(layer_tex, texture_coordinates_to_fragment_shader), 0.2);
}