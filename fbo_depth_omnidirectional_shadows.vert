#version 330 core
// we only want to transform the vertices to the light's "POV" and not much else since this is just for grabbing the depth for the shadow mapping
layout (location = 0) in vec3 vertices;

uniform mat4 transformation_matrix;

void main()
{
	gl_Position = transformation_matrix * vec4(vertices, 1.0);
}


