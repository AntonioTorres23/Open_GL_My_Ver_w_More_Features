#version 330 core
out vec4 color_output;

in float directional_lighting;

in vec3 lighting;

in vec2 fragment_texture_coordinates;

//uniform sampler2D diffTex1;

uniform vec3 color_of_light;

// we add the ambient_lighting and diffuse_lighting calculations together in which the sum of that is multiplied by the model's "color"

//vec3 final_lighting = (directional_lighting + pos_lighting) * color_of_light;

//vec3 final_lighting = directional_lighting * color_of_light;

//vec3 tex_output = final_lighting * texture(diffTex1, fragment_texture_coordinates).xyz;

void main()
{
	//vec3 final_lighting = directional_lighting * color_of_light;

	//final_lighting += pos_lighting * color_of_light;

	//vec3 first_light = directional_lighting * color_of_light;

	//vec3 second_light = pos_lighting * color_of_light;
 
	//vec3 tex_output = final_lighting * texture(diffTex1, fragment_texture_coordinates).xyz;
	
	//vec3 tex_output = (first_light + second_light) * texture(diffTex1, fragment_texture_coordinates).xyz;


	color_output = vec4(lighting, 1.0);
}