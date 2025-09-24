#version 330 core
layout (location = 0) in vec3 vertex_coords; 

void main()
{
	gl_Position = vec4(vertex_coords.x, vertex_coords.y, vertex_coords.z, 1.0);
}
