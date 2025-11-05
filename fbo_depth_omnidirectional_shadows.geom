#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 light_matrices[6];

out vec4 omnidirectional_fragment_positions; 

void main()
{
	for(int cube_map_face = 0; cube_map_face < 6; ++cube_map_face)
	{
		gl_Layer = cube_map_face; // built-in var that specifies to which face we render
		for (int vertices = 0; vertices < 3; ++vertices)
		{
			omnidirectional_fragment_positions = gl_in[vertices].gl_Position;
			gl_Position = light_matrices[cube_map_face] * omnidirectional_fragment_positions;
			EmitVertex();
		}
		EndPrimitive();
	}
}