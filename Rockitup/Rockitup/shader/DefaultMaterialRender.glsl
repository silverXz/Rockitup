// For materials without textures.



--Vertex Shader--

#version 400

layout (location = 0) in vec3 VertexPosition;

uniform mat4 MVP;

void main()
{
	gl_Position = MVP * vec4(VertexPosition,1.0);
}

--Fragment Shader--

#version 400

void main()
{
	if(gl_FrontFacing)
		discard;
}