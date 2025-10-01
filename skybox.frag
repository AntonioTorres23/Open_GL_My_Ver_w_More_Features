#version 330 core

in vec3 SBC; 

out vec4 skybox;

uniform samplerCube skybox_texture;

void main()
{
	skybox = texture(skybox_texture, SBC);
	
}