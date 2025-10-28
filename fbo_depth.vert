#version 330 core
// we only want to transform the vertices to the light's "POV" and not much else since this is just for grabbing the depth for the shadow mapping
layout (location = 0) in vec3 vertices;

uniform mat4 transformation_matrix;

// send our light matrix to this shader via uniform
uniform mat4 light_matrix;

void main()
{
	gl_Position = light_matrix * transformation_matrix * vec4(vertices, 1.0);
}


