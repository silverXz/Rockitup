--Vertex Shader--

#version 400

layout ( location = 0 ) in vec3 VertexPosition;
layout ( location = 1 ) in vec3 VertexNormal;
layout ( location = 2 ) in vec2 VertexCoord;

out vec4 eNormalDepth;
out vec2 Coord;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 MVP;


void main()
{
	Coord = VertexCoord;
	mat4 MV = ViewMatrix * ModelMatrix;
	vec3 ePosition = vec3(MV * vec4(VertexPosition,1.0f));
	vec3 eNormal = vec3(MV*vec4(VertexNormal,0.0f));
	eNormalDepth = vec4(eNormal,ePosition.z);
	gl_Position = MVP * vec4(VertexPosition,1.0f);
}

--Fragment Shader--

#version 400

in vec4 eNormalDepth;
in vec2 Coord;

layout ( location = 0 ) out vec4 FragNormalDepth;
layout ( location = 1 ) out vec3 FragAmbient;
layout ( location = 2 ) out vec3 FragDiffuse;
layout ( location = 3 ) out vec3 FragSpecular;
layout ( location = 4 ) out vec3 FragColor;

uniform sampler2D fuck;

struct MaterialInfo
{
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
};

uniform MaterialInfo material;


void main()
{
	if( !gl_FrontFacing )
	{
		discard;
	}
	else
		FragNormalDepth = eNormalDepth;
	FragAmbient = material.Ka;
	FragDiffuse = material.Kd;
	FragSpecular = material.Ks;
	FragColor = texture(fuck,Coord).xyz;
}