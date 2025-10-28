#version 330 core
// since we have no color buffer and disabled the draw and read buffers, the resulting fragments do not require any processing
// thus, we can just leave this shader empty
void main()
{
	// this is what is happening behind the scenes in the FBO depth buffer
	// gl_FragDepth = gl_FragCoord.z;
}