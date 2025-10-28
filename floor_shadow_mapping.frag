#version 330 core
out vec4 apply_light;

in vec4 position_of_models_fragments_from_light_perspective;

in vec3 position_of_models_fragments;

in vec3 out_norm_pos_coordinates; 

in vec2 out_tex_pos_coordinates; 

uniform vec3 pos_of_cam; 

uniform sampler2D floor_texture;

uniform sampler2D depth_map_texture;

uniform vec3 color_of_light;

uniform vec3 pos_of_light;

float CalcShadow(vec4 position_of_models_fragments_from_light_perspective_arg)
{
	// perform perspective division
	// orthographic division doesn't really need to do this since the w component is not affected by perspective divison but makes this shader compatible with perspective division
	vec3 perspective_coords = position_of_models_fragments_from_light_perspective_arg.xyz / position_of_models_fragments_from_light_perspective_arg.w;

	// use this formula to transform the now Normalized Device Coordinates (perspective_coords) which are in range of [-1, 1] to only a range of [0, 1] since that is the depth map/ shadow map range
	// the x and y coords also gives us the closest depth from the light's pov
	perspective_coords = perspective_coords * 0.5 + 0.5;

	// use the x and y coords from perspective_coords to get the closest depth from the light's pov
	// see how we only take one color component since there is no picture/color from the texture
	float closest_depth_from_light = texture(depth_map_texture, perspective_coords.xy).r;

	// get the perspective_coords z coordinate whichc equals the depth of the fragment from the light's pov.
	float current_depth_from_light = perspective_coords.z;

	// check if current_depth_from_light is higher than closest_depth_from_light and if so, it is in shadow
	float in_shadow;

	if (current_depth_from_light > closest_depth_from_light)
		// fragment is in shadow
		in_shadow = 1.0;
	else
		// fragment is not in shadow
		in_shadow = 0.0;

	return in_shadow;
}



void main()
{

	vec3 normalized_normal_coordinates = normalize(out_norm_pos_coordinates);

	vec3 direction_of_camera_from_model = normalize(pos_of_cam - position_of_models_fragments);
	
	vec3 direction_of_light_from_model = normalize(pos_of_light - position_of_models_fragments);

	vec3 col = texture(floor_texture, out_tex_pos_coordinates).rgb;

	//vec3 col = vec3(1.0, 0.0, 0.0);

	// calculate ambient lighting
	vec3 ambient_lighting = 0.3 * color_of_light;

	// calculate diffuse lighting
	vec3 diffuse_lighting = max(dot(direction_of_light_from_model, normalized_normal_coordinates), 0.0) * color_of_light;

	// get halfway direction for specular lighting
	vec3 halfway_direction = normalize(direction_of_light_from_model + direction_of_camera_from_model); 

	// calculate specular lighting
	vec3 specular_lighting = pow(max(dot(normalized_normal_coordinates, halfway_direction), 0.0), 64) * color_of_light;
	
	float shadows = CalcShadow(position_of_models_fragments_from_light_perspective);

	//float shadows = 1.0;

	vec3 final_lighting = (ambient_lighting + (1.0 - shadows) * (diffuse_lighting + specular_lighting)) * col;

	apply_light = vec4(final_lighting, 1.0);
	//apply_light = vec4(1.0, 0.0, 0.0, 1.0);
}