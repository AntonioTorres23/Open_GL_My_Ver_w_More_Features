#version 330 core
out vec4 apply_light;

in vec4 position_of_models_fragments_from_light_perspective;

in vec3 position_of_models_fragments;

in vec3 out_norm_pos_coordinates; 

in vec2 out_tex_pos_coordinates; 

uniform vec3 pos_of_cam; 

uniform sampler2D diffTex1;

uniform sampler2D depth_map_texture;

uniform vec3 color_of_light;

uniform vec3 pos_of_light;

float CalcShadow(vec4 position_of_models_fragments_from_light_perspective_arg, vec3 normal_arg, vec3 direction_of_light_arg)
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

	// create a bias based on the dot product of the direction of the light and the normal coordinates
	// with the max bias being 0.05 and the minimum bias being 0.005
	// this helps combat shadow acne by generating an off set to prevent the depth map from sampling an already sampled depth value
	float shadow_acne_bias = max(0.05 * (1.0 - dot(normal_arg,  direction_of_light_arg)), 0.005);

	// check if current_depth_from_light is higher than closest_depth_from_light and if so, it is in shadow
	
	//float in_shadow = current_depth_from_light > closest_depth_from_light ? 1.0 : 0.0;
	//float in_shadow = current_depth_from_light - shadow_acne_bias > closest_depth_from_light ? 1.0 : 0.0;

	
	float in_shadow = 0.0;

	// use percentage closer filtering to smooth out the shadows

	// create a vec2 variable that returns the size of a single texel that we use to offset the texture coordinates
	// textureSize built-in function that returns the width and height of the given sampler texture, the second argument specifies which mipmap level you want to use. In this case we are using level 0.
	// dividing 1 over this value returns the size of a singgle texel that we use to offset the texture coordinates
	vec2 size_of_texel = 1.0 / textureSize(depth_map_texture, 0);

	// now we use a nested for loop to sample 9 values around the projected coordinate's x and y values, test for if they are in shadow

	for (int x_coord = -1; x_coord <= 1; ++x_coord)
	{
		for(int y_coord = -1; y_coord <= 1; ++y_coord)
		{
			// here is where we sample depth with PCF by adding the 2 x and y values that we are looping through currently with the original coordinates and then multiplying that by the size of a single texel for the depth map
			float percentage_closer_filtering_depth = texture(depth_map_texture, perspective_coords.xy + vec2(x_coord, y_coord) * size_of_texel).r;
			// now check if the current depth is in shadow or not with the PCF depth
			if (current_depth_from_light - shadow_acne_bias > percentage_closer_filtering_depth)
				// fragment is in shadow
				in_shadow += 1.0;
			else
				// fragment is not in shadow
				in_shadow += 0.0;
		
		}
	}

	// becuase of the amount of sample values we went (9) we need to set and divide the in_shadow variable by that amount
	in_shadow /= 9.0;

	return in_shadow;
}

void main()
{

	vec3 normalized_normal_coordinates = normalize(out_norm_pos_coordinates);

	vec3 direction_of_camera_from_model = normalize(pos_of_cam - position_of_models_fragments);
	
	vec3 direction_of_light_from_model = normalize(pos_of_light - position_of_models_fragments);

	vec3 col = texture(diffTex1, out_tex_pos_coordinates).rgb;

	//vec3 col = vec3(1.0, 0.0, 0.0);

	// calculate ambient lighting
	vec3 ambient_lighting = 0.3 * color_of_light;

	// calculate diffuse lighting
	vec3 diffuse_lighting = max(dot(direction_of_light_from_model, normalized_normal_coordinates), 0.0) * color_of_light;

	// get halfway direction for specular lighting
	vec3 halfway_direction = normalize(direction_of_light_from_model + direction_of_camera_from_model); 

	// calculate specular lighting
	vec3 specular_lighting = pow(max(dot(normalized_normal_coordinates, halfway_direction), 0.0), 64) * color_of_light;
	
	float shadows = CalcShadow(position_of_models_fragments_from_light_perspective, normalized_normal_coordinates, direction_of_light_from_model);

	//float shadows = 1.0;

	vec3 final_lighting = (ambient_lighting + (1.0 - shadows) * (diffuse_lighting + specular_lighting)) * col;

	apply_light = vec4(final_lighting, 1.0);

	//apply_light = vec4(1.0, 0.0, 0.0, 1.0);
}


