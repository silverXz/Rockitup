--Vertex Shader--

#version 400

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexCoord;

out float eDepth;
out vec3 eNormal;
out vec2 Coord;


uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 MVP;

void main()
{
	Coord = VertexCoord;
	mat4 MV = ViewMatrix * ModelMatrix;
	vec3 ePosition = vec3(MV * vec4(VertexPosition,1.0));
	eDepth = ePosition.z;
	eNormal = normalize( vec3(MV * vec4(VertexNormal,0.0)));
	gl_Position = MVP * vec4(VertexPosition,1.0);
}

--Fragment Shader--

#version 400

in float eDepth;
in vec3 eNormal;
in vec2 Coord;

uniform sampler2D fuck;

layout (location = 0) out vec4 FragNormalDepth;
layout (location = 1) out vec3 FragAmbient;
layout (location = 2) out vec3 FragDiffuse;
layout (location = 3) out vec3 FragSpecular;
layout (location = 4) out vec3 FragTexColor;

struct MaterialInfo
{
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
};

uniform MaterialInfo material;



void main()
{
	FragAmbient = material.Ka;
	FragDiffuse = material.Kd;
	FragSpecular = material.Ks;
	FragNormalDepth = vec4(eNormal * 0.5f + vec3(0.5f),eDepth);
	FragTexColor = texture(fuck,Coord).xyz;
	//FragNormalDepth = vec4(material.Kd,eDepth);
}