--Vertex Shader--

#version 400


layout( location = 0 ) in vec3 VertexPosition;
layout ( location = 1 ) in vec3 VertexColor;

out vec3 Color;
uniform mat4 MVP;

void main()
{
	Color = VertexColor;
	gl_Position = MVP * (vec4(VertexPosition,1.0f));
}


--Fragment Shader--

#version 400

layout ( location = 0 ) out vec4 FragColor;

in vec3 Color;


void main()
{
	FragColor = vec4(Color,1.0f);
}