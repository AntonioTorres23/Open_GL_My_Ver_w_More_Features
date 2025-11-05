#version 330 core

in vec4 omnidirectional_fragment_positions;

uniform vec3 pos_of_light;

uniform float perspective_far_plane;

void main()
{
	float distance_of_light = length(omnidirectional_fragment_positions.xyz - pos_of_light);

	// map to a range of [0,1] range by dividing by the far plane of our light perspective matrix
	distance_of_light = distance_of_light / perspective_far_plane;

	// write this as the modified depth value
	gl_FragDepth = distance_of_light;
}