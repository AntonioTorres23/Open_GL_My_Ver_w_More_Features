#version 330 core
out vec4 color;

uniform vec3 color_of_light;

void main()
{
	// remember, the color of this does not effect the actual color of the light that is being casted to the object. It is simply just a visual representation of where the light source is in world space.
	color = vec4(color_of_light, 1.0);
}