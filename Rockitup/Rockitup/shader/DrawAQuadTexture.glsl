--Vertex Shader--

#version 400

layout (location = 0 ) in vec3 VertexPosition;
layout ( location = 2 ) in vec2 VertexCoord;

out vec2 Coord;

void main()
{
	Coord = VertexCoord;
	gl_Position = vec4(VertexPosition,1.0f);
}




--Fragment Shader--

#version 400

layout ( location = 0 ) out vec4 FragColor;

in vec2 Coord;

uniform sampler2D fuck;

void main()
{
	FragColor = texture(fuck,Coord);
}