#version 330 core
out vec4 color_output;

in float directional_lighting;

in float pos_lighting;

in vec2 fragment_texture_coordinates;

uniform sampler2D diffTex1;

uniform vec3 color_of_light;

// we add the ambient_lighting and diffuse_lighting calculations together in which the sum of that is multiplied by the model's "color"

vec3 final_lighting = (directional_lighting + pos_lighting) * color_of_light;

vec3 tex_output = final_lighting * texture(diffTex1, fragment_texture_coordinates).xyz;

void main()
{
	color_output = vec4(tex_output, 1.0);
}