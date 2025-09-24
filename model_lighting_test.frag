#version 330 core
out vec4 color_output;

in vec2 fragment_texture_coordinates;

in float ambient_lighting;

in float diffuse_lighting;

in float specular_lighting;

uniform sampler2D diffTex1;

uniform vec3 color_of_light;

// we add the ambient_lighting and diffuse_lighting calculations together in which the sum of that is multiplied by the model's "color"

vec3 final_ambient_lighting = ambient_lighting * color_of_light;

vec3 final_diffuse_lighting = diffuse_lighting * color_of_light;

vec3 final_specular_lighting = specular_lighting * color_of_light;

vec3 tex_output = (final_ambient_lighting + final_diffuse_lighting + final_specular_lighting) * texture(diffTex1, fragment_texture_coordinates).xyz;

void main()
{
	color_output = vec4(tex_output, 1.0);
}