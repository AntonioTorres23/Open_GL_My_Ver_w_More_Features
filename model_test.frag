#version 330 core
out vec4 color_output;

in vec2 fragment_texture_coordinates;

uniform sampler2D diffTex1;

void main()
{
	color_output = texture(diffTex1, fragment_texture_coordinates); 
}