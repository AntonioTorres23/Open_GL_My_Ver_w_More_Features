#version 330 core
layout (location = 0) in vec3 vector_position_coordinates;

layout (location = 1) in vec3 normal_position_coordinates;

layout (location = 2) in vec2 texture_position_coordinates;

out vec2 fragment_texture_coordinates;

// GOURAUD SHADING: lighting done in the vertex shader to save on memory

// ambient lighting

out float ambient_lighting;

// diffuse lighting

out float diffuse_lighting;

// specular lighting

out float specular_lighting;

uniform mat4 transformation_matrix;

uniform mat4 view_matrix;

uniform mat4 perspective_matrix;

uniform vec3 light_pos;

uniform vec3 pos_of_camera;

uniform mat3 transformation_matrix_for_normal_coordinates;

// diffuse lighting calculations

// normalized normal coordinates so that the length/magnitude of the coordinates are always a length of 1
//vec3 normalized_normal_coordiantes = normalize(normal_position_coordinates);
vec3 normal_coordinates = transformation_matrix_for_normal_coordinates * normal_position_coordinates;

vec3 normalized_normal_coordniates = normalize(normal_coordinates);


// calculating the model's fragment position in world space by multiplying the original vector_position_coordinates by the transformation_matrix

vec3 position_of_models_fragments = vec3(transformation_matrix * vec4(vector_position_coordinates, 1.0)); 

// grabs the direction the light is facing among the object and the lighting
// we get this via subtracting the position of the lighting from the position of the model's fragments in world space
// we also normalize this result which ensures the final vector has a length/magnitude of 1

vec3 light_direction = normalize(light_pos - position_of_models_fragments);

// calculations for diffuse_lighting
// had to put it before the main function

float diffuse_lighting_calc = max(dot(light_direction, normalized_normal_coordniates), 0.0);

// have to specify a float value that will determine how strong/bright the specular highlight will be on the object

float strength_of_specular_lighting = 0.3f;

// specify where the camera is from the fragment's position

vec3 direction_of_camera_from_model = normalize(pos_of_camera - position_of_models_fragments);

// create a reflect direction vector that creates the highlight of specular lighting
// we negate the direction of light source since the reflect built-in function expects a light direction that is pointing from the light source towards the position_of_models_fragments
// however light_direction is actually pointing from the position_of_models_fragments to light_pos

vec3 direction_of_reflection = reflect(-light_direction, normalized_normal_coordniates);

// now we calculate the specular lighting
// we use the pow built in function to multiply our dot product by the power of whatever number we specify; there are some preset values that typcially most graphics developers choose from, each making the specular highlight more "concentrated"

float specular_lighting_calc = pow(max(dot(direction_of_camera_from_model, direction_of_reflection), 0.0), 128);




void main()
{
	fragment_texture_coordinates = texture_position_coordinates;

	
	// ambient_lighting is a constant float value that is the darkest lighting of the model/obj
	ambient_lighting = 0.3; 
	
	/* 
	diffuse_lighting is where we calculate a dot product between the direction of the light and the normal coordinates on the model which is multiplying the 2 
	3-coordiante vector to return a single scalar value that we use as a value to calulate how much the light is going to impact the model
	we use the max built in function so that the value can't go under 0
	*/
	diffuse_lighting = diffuse_lighting_calc;

	specular_lighting = strength_of_specular_lighting * specular_lighting_calc;


	gl_Position =   perspective_matrix * view_matrix * transformation_matrix * vec4(vector_position_coordinates, 1.0);


}