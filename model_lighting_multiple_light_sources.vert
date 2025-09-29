#version 330 core
layout (location = 0) in vec3 vector_position_coordinates;

layout (location = 1) in vec3 normal_position_coordinates;

layout (location = 2) in vec2 texture_position_coordinates;

out vec2 fragment_texture_coordinates;

// DirectionLighting Structure to hold all the data of the global direction light
// think of the directional light as "world light"; it mimics lighting such as the sun
struct DirectionalLighting
{
	vec3 light_direction;
};

// PosLight Structure to hold all the data of lights that are positional based
struct PosLight
{
	// position of light in world space
	vec3 world_space_position;
	// constant value to help calculate attenuation depending on how far away the light is from the object
	float constant;
	// linear value to help calculate attenuation depending on how far away the light is from the object
	float lin; 
	// quadratic value to help calculate attenuation depending on how far away the light is from the object
	float quad; 
};

// preprocessor definition to specify the amount of lights we want within
#define AMMOUNT_OF_POS_LIGHTS 3

// GOURAUD SHADING: lighting done in the vertex shader to save on memory

out float pos_lighting;

out float directional_lighting;

uniform mat4 transformation_matrix;

uniform mat4 view_matrix;

uniform mat4 perspective_matrix;

uniform vec3 light_pos;

uniform vec3 pos_of_camera;

uniform mat3 transformation_matrix_for_normal_coordinates;

uniform DirectionalLighting direction_lighting_var;

// we can make this a uniform value with the data type of our previously defined structure
uniform PosLight pos_light_var[AMMOUNT_OF_POS_LIGHTS];

float DirectionalLightingCalculations (DirectionalLighting direction_lighting_arg, vec3 normal_coordinates_arg, vec3 pos_of_camera_arg)
{
	// we only negate the light direction since directional lighting affects everything in the environment
	vec3 light_direction = normalize(-direction_lighting_arg.light_direction);

	// ambient lighting, the darkest of the shading
	float ambient_lighting = 0.2;

	// calculations for diffuse_lighting
	float diffuse_lighting = max(dot(normal_coordinates_arg, light_direction), 0.0);

	// relection for specular_lighting
	vec3 direction_of_reflection = reflect(-light_direction, normal_coordinates_arg);
	
	// we only need the position of the camera and the direction of reflection since there is no "distance" between the light pos and the model/obj(s)
	float specular_lighting = pow(max(dot(pos_of_camera_arg, direction_of_reflection), 0.0), 128);


	return (ambient_lighting + diffuse_lighting + specular_lighting);
}


float PosLightCalculations (PosLight pos_light_struct_arg, vec3 normal_coordiantes_arg, vec3 position_of_models_fragments_arg, vec3 pos_of_camera_arg)
{

	float ambient_lighting = 0.3;
	
	// grabs the direction the light is facing among the object and the lighting
	// we get this via subtracting the position of the lighting from the position of the model's fragments in world space
	// we also normalize this result which ensures the final vector has a length/magnitude of 1
	vec3 light_direction = normalize(pos_light_struct_arg.world_space_position - position_of_models_fragments_arg);

	// calculations for diffuse_lighting
	float diffuse_lighting_calc = max(dot(light_direction, normal_coordiantes_arg), 0.0);

	// create a reflect direction vector that creates the highlight of specular lighting
	// we negate the direction of light source since the reflect built-in function expects a light direction that is pointing from the light source towards the position_of_models_fragments
	// however light_direction is actually pointing from the position_of_models_fragments to light_pos
	vec3 direction_of_reflection = reflect(-light_direction, normal_coordiantes_arg);

	// specify where the camera is from the fragment's position

	vec3 direction_of_camera_from_model = normalize(pos_of_camera_arg - position_of_models_fragments_arg);

	
	float specular_lighting_calc = pow(max(dot(direction_of_camera_from_model, direction_of_reflection), 0.0), 128);

	// calculate attenuation to impact lighting on object depending on distance/position of light
	// we use the built-in length function to find the length/distancce between the light source and the object
	// follows the same way we get the light direction 
	float dis_between_light_and_object = length(pos_light_struct_arg.world_space_position - position_of_models_fragments_arg);
	
	// apply this algebraic formula to calculate attenuation
	//float atten = 1.0 / (pos_light_struct_arg.constant + pos_light_struct_arg.lin * dis_between_light_and_object + pos_light_struct_arg.quad * (dis_between_light_and_object * dis_between_light_and_object));

	// apply attenuation to ambient lighting
	//ambient_lighting *= atten;
	// apply attenuation to diffuse lighting
	//diffuse_lighting_calc *= atten;
	// apply attenuation to specular lighting
	//specular_lighting_calc *= atten;

	return (ambient_lighting + diffuse_lighting_calc + specular_lighting_calc);
	

}

// diffuse lighting calculations

// normalized normal coordinates so that the length/magnitude of the coordinates are always a length of 1
// vec3 normalized_normal_coordiantes = normalize(normal_position_coordinates);
vec3 normal_coordinates = transformation_matrix_for_normal_coordinates * normal_position_coordinates;

vec3 normalized_normal_coordniates = normalize(normal_coordinates);


// calculating the model's fragment position in world space by multiplying the original vector_position_coordinates by the transformation_matrix

vec3 position_of_models_fragments = vec3(transformation_matrix * vec4(vector_position_coordinates, 1.0)); 


vec3 direction_of_camera_from_model = normalize(pos_of_camera - position_of_models_fragments);



void main()
{
	fragment_texture_coordinates = texture_position_coordinates;


	gl_Position =   perspective_matrix * view_matrix * transformation_matrix * vec4(vector_position_coordinates, 1.0);

	directional_lighting = DirectionalLightingCalculations(direction_lighting_var, normalized_normal_coordniates, direction_of_camera_from_model);

	for (int pos = 0; pos < AMMOUNT_OF_POS_LIGHTS; pos++)
		pos_lighting = PosLightCalculations(pos_light_var[pos], normalized_normal_coordniates, position_of_models_fragments, direction_of_camera_from_model);


}