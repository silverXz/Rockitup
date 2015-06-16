--Vertex Shader--

#version 400

layout (location = 0 ) in vec3 VertexPosition;
layout ( location = 1 ) in vec3 VertexNormal;
layout ( location = 2 ) in vec2 TexCoord;

out vec3 CamDir;
out vec2 Coord;

uniform mat4 InverseProjectionMatrix;

void main()
{
	Coord = TexCoord;
	vec4 eyePos = InverseProjectionMatrix * vec4(VertexPosition,1.0f);
	CamDir = normalize(vec3(eyePos));
	gl_Position = vec4(VertexPosition,1.0f);
}

--Fragment Shader--

#version 400

layout ( location = 0 ) out vec4 FragColor;

in vec2 Coord;
in vec3 CamDir;

uniform sampler2D TexNormalDepth;
uniform sampler2D TexAmbient;
uniform sampler2D TexDiffuse;
uniform sampler2D TexSpecular;
uniform sampler2D fuck;

struct LightInfo
{
	vec3 Intensity;
	vec3 Position;
};

const float Shininess = 128.0f;

uniform LightInfo Light0;

vec3 phong(vec3 ePosition, vec3 eNormal, vec3 Ka, vec3 Kd, vec3 Ks )
{
	vec3 n = eNormal * 2.0f - vec3(1.0f);
	n = normalize(n);
	vec3 v = normalize(-ePosition);
	vec3 l = normalize(Light0.Position - ePosition);
	vec3 r = reflect(-l,n);
	//return Light0.Intensity * (Ks * pow( max(dot(r,v),0.0),Shininess ));
	return Light0.Intensity * (Ka + Kd * max( dot(l,n),0.0 ) +Ks * pow( max(dot(r,v),0.0),Shininess ));
}

void main()
{
	vec4 NormDepth = texture(TexNormalDepth,Coord);
	float eDepth = NormDepth.a;
	vec3 camDir = normalize(CamDir);
	camDir = camDir / camDir.z;
	vec3 ePosition = camDir * eDepth;
	vec3 Ka = vec3(texture(TexAmbient,Coord));
	vec3 Kd = vec3(texture(TexDiffuse,Coord));
	vec3 Ks = vec3(texture(TexSpecular,Coord));

	FragColor = texture(fuck,Coord) * vec4(phong(ePosition,NormDepth.xyz,Ka,Kd,Ks),1.0f);
}