#version 330 core
layout (location = 0) in vec3 skybox_local_positional_coordiantes;

out vec3 SBC;

// we don't use the transformation matrix becuase the local positional coordinates are at the furthest points 1.0 and -1.0
// the coordinates should stay within the origin position of (0,0,0), as well as we want the skybox to appear constantly far away

uniform mat4 view_matrix;

uniform mat4 perspective_matrix;

void main()
{
	SBC = skybox_local_positional_coordiantes;

	// store output data in seperate 4 coordinate vector
	vec4 vert_out = perspective_matrix * view_matrix * vec4(skybox_local_positional_coordiantes, 1.0);

	// use gl_Position as our output since its built in by OpenGL to do perspective division as well as other things
	// we use the homogenous w coordinate as our z coordinate becuase gl_Position does perspective division with the z and w coordinate
	// if we set the z coordinate to the same value as the w coordinate it will always equal 1 (kind of similar to ortho projection)
	gl_Position = vert_out.xyww;
}


