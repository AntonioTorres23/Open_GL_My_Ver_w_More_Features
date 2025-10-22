#version 330 core
out vec4 color;

in vec2 frag_texture_coordinates;

uniform sampler2D floor_texture;

void main()
{
	
	color = texture(floor_texture, frag_texture_coordinates);
}