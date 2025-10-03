#version 330 core

out vec4 apply_light;

// DirectionLighting Structure to hold all the data of the global direction light
// think of the directional light as "world light"; it mimics lighting such as the sun
struct DirectionalLighting
{
	vec3 light_direction;

	vec3 ambient_color;

	vec3 diffuse_color;

	vec3 specular_color;
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

	vec3 ambient_color;

	vec3 diffuse_color;

	vec3 specular_color; 
};

// preprocessor definition to specify the amount of lights we want within
#define AMOUNT_OF_POS_LIGHTS 3

in vec3 position_of_models_fragments;

in vec3 out_norm_pos_coordinates; 

in vec2 out_tex_pos_coordinates; 

uniform vec3 pos_of_cam; 

uniform PosLight pos_light_var[AMOUNT_OF_POS_LIGHTS];

uniform DirectionalLighting direction_lighting_var;

uniform sampler2D diffTex1;

uniform vec3 color_of_light;

vec3 PosLightCalculations (PosLight pos_light_struct_arg, vec3 normal_coordiantes_arg, vec3 position_of_models_fragments_arg, vec3 pos_of_camera_arg);

vec3 DirectionalLightingCalculations (DirectionalLighting direction_lighting_arg, vec3 normal_coordinates_arg, vec3 pos_of_camera_arg);

void main()
{

	vec3 normalized_normal_coordinates = normalize(out_norm_pos_coordinates);

	vec3 direction_of_camera_from_model = normalize(pos_of_cam - position_of_models_fragments);

	

	vec3 calc_res = DirectionalLightingCalculations(direction_lighting_var, normalized_normal_coordinates, direction_of_camera_from_model);
	
	for (int pos = 0; pos < AMOUNT_OF_POS_LIGHTS; pos++)
	{
		calc_res += PosLightCalculations(pos_light_var[pos], normalized_normal_coordinates, position_of_models_fragments, direction_of_camera_from_model);
	}
	
	//calc_res = vec3(0.8, 0.8, 0.8);

	apply_light = vec4(calc_res, 1.0);
	
}


vec3 DirectionalLightingCalculations (DirectionalLighting direction_lighting_arg, vec3 normal_coordinates_arg, vec3 pos_of_camera_arg)
{
	// we only negate the light direction since directional lighting affects everything in the environment
	vec3 light_direction = normalize(-direction_lighting_arg.light_direction);

	// ambient lighting, the darkest of the shading
	//float ambient_lighting = 0.3;

	// calculations for diffuse_lighting
	vec3 diffuse_lighting = vec3(max(dot(normal_coordinates_arg, light_direction), 0.0));


	// relection for specular_lighting
	vec3 direction_of_reflection = reflect(-light_direction, normal_coordinates_arg);
	
	// we only need the position of the camera and the direction of reflection since there is no "distance" between the light pos and the model/obj(s)
	float specular_lighting = pow(max(dot(pos_of_camera_arg, direction_of_reflection), 0.0), 32);

	vec3 ambient_return = direction_lighting_arg.ambient_color * vec3(texture(diffTex1, out_tex_pos_coordinates));

	vec3 diffuse_return = direction_lighting_arg.diffuse_color * diffuse_lighting * vec3(texture(diffTex1, out_tex_pos_coordinates));

	vec3 specular_return = direction_lighting_arg.specular_color * specular_lighting * vec3(texture(diffTex1, out_tex_pos_coordinates));

	//return (ambient_lighting + diffuse_lighting + specular_lighting);
	return (ambient_return + diffuse_return + specular_return);
}


vec3 PosLightCalculations (PosLight pos_light_struct_arg, vec3 normal_coordiantes_arg, vec3 position_of_models_fragments_arg, vec3 pos_of_camera_arg)
{

	//float ambient_lighting = 0.2;
	
	// grabs the direction the light is facing among the object and the lighting
	// we get this via subtracting the position of the lighting from the position of the model's fragments in world space
	// we also normalize this result which ensures the final vector has a length/magnitude of 1
	vec3 light_direction = normalize(pos_light_struct_arg.world_space_position - position_of_models_fragments_arg);

	// calculations for diffuse_lighting
	vec3 diffuse_lighting_calc = vec3(max(dot(normal_coordiantes_arg, light_direction), 0.0));

	// create a reflect direction vector that creates the highlight of specular lighting
	// we negate the direction of light source since the reflect built-in function expects a light direction that is pointing from the light source towards the position_of_models_fragments
	// however light_direction is actually pointing from the position_of_models_fragments to light_pos
	vec3 direction_of_reflection = reflect(-light_direction, normal_coordiantes_arg);

	// specify where the camera is from the fragment's position

	vec3 direction_of_camera_from_model = normalize(pos_of_camera_arg - position_of_models_fragments_arg);

	
	float specular_lighting_calc = pow(max(dot(direction_of_camera_from_model, direction_of_reflection), 0.0), 32);

	// calculate attenuation to impact lighting on object depending on distance/position of light
	// we use the built-in length function to find the length/distancce between the light source and the object
	// follows the same way we get the light direction 
	float dis_between_light_and_object = length(pos_light_struct_arg.world_space_position - position_of_models_fragments_arg);
	
	// apply this algebraic formula to calculate attenuation
	float atten = 1.0 / (pos_light_struct_arg.constant + pos_light_struct_arg.lin * dis_between_light_and_object + pos_light_struct_arg.quad * (dis_between_light_and_object * dis_between_light_and_object));

	/*
	// apply attenuation to ambient lighting
	ambient_lighting *= atten;
	// apply attenuation to diffuse lighting
	diffuse_lighting_calc *= atten;
	// apply attenuation to specular lighting
	specular_lighting_calc *= atten;
	*/

	vec3 ambient_return = pos_light_struct_arg.ambient_color * vec3(texture(diffTex1, out_tex_pos_coordinates));

	vec3 diffuse_return = pos_light_struct_arg.diffuse_color * diffuse_lighting_calc * vec3(texture(diffTex1, out_tex_pos_coordinates));

	vec3 specular_return = pos_light_struct_arg.specular_color * specular_lighting_calc * vec3(texture(diffTex1, out_tex_pos_coordinates));

	ambient_return *= atten;

	diffuse_return *= atten;

	specular_return *= atten;
	
	//return (ambient_lighting + diffuse_lighting_calc + specular_lighting_calc);
		
	return (ambient_return + diffuse_return + specular_return);


}




