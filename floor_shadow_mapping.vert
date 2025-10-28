#version 330 core 

layout (location = 0) in vec3 vec_pos_coordinates; 

layout (location = 1) in vec3 norm_pos_coordinates;

layout (location = 2) in vec2 tex_pos_coordinates;

out vec3 out_norm_pos_coordinates;

out vec2 out_tex_pos_coordinates;

out vec3 position_of_models_fragments;

out vec4 position_of_models_fragments_from_light_perspective;

uniform mat4 transformation_matrix;

uniform mat4 view_matrix;

uniform mat4 perspective_matrix;

uniform mat4 light_matrix;

//uniform mat3 transformation_matrix_for_normal_coordinates;

void main()
{

	// calculating the model's fragment position in world space by multiplying the original vector_position_coordinates by the transformation_matrix

	position_of_models_fragments = vec3(transformation_matrix * vec4(vec_pos_coordinates, 1.0)); 
	
	out_norm_pos_coordinates = transpose(inverse(mat3(transformation_matrix))) * norm_pos_coordinates;

	out_tex_pos_coordinates = tex_pos_coordinates;

	position_of_models_fragments_from_light_perspective = light_matrix * vec4(position_of_models_fragments, 1.0); 

	gl_Position = perspective_matrix * view_matrix * transformation_matrix * vec4(vec_pos_coordinates, 1.0);
}


