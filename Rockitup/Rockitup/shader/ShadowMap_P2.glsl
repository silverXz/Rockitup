--Vertex Shader--

#version 400

layout( location = 0 ) in vec3 VertexPosition;
layout( location = 1 ) in vec3 VertexNormal;
layout( location = 2 ) in vec2 VertexCoord;

out vec4 ShadowCoord;
out vec3 ePosition;
out vec2 Coord;
out vec3 eNormal;

uniform mat4 ShadowMatrix;
uniform mat4 MV;
uniform mat4 MVP;

void main()
{
	ShadowCoord = ShadowMatrix * vec4(VertexPosition,1.0);
	ePosition = vec3(MV * vec4(VertexPosition,1.0f));
	eNormal = mat3(MV) * VertexNormal;
	Coord = VertexCoord;
	gl_Position = MVP * vec4(VertexPosition,1.0f);
}

--Fragment Shader--

#version 400

layout (location = 0 ) out vec4 FragColor;

in vec4 ShadowCoord;
in vec3 ePosition;
in vec2 Coord;
in vec3 eNormal;

uniform sampler2DShadow texShadowMap;
uniform sampler2D texFuck;

struct LightInfo {
	vec3 Position;
	vec3 Intensity;
};

uniform LightInfo Light0;

struct MaterialInfo {
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	float Shininess;
};

uniform MaterialInfo material;

void main()
{
	if( !gl_FrontFacing )
		discard;

	float sum = 0.0f;
	sum += textureProjOffset(texShadowMap,ShadowCoord,ivec2(-1,-1));
	sum += textureProjOffset(texShadowMap,ShadowCoord,ivec2(-1,1));
	sum += textureProjOffset(texShadowMap,ShadowCoord,ivec2(1,1));
	sum += textureProjOffset(texShadowMap,ShadowCoord,ivec2(1,-1));
	sum *= 0.25f;

	vec3 n = normalize(eNormal);
	vec3 l = normalize(Light0.Position - ePosition);
	vec3 e = normalize( - ePosition );
	vec3 r = reflect( -l , n );
	vec3 ambient = material.Ka;
	vec3 diffuse = material.Kd * max(dot(n,l),0.0f);
	vec3 specular = material.Ks * pow(max(dot(r,e),1.0),material.Shininess);
	vec4 LightColor = vec4(Light0.Intensity * ambient + sum * Light0.Intensity * ( diffuse + specular ),1.0f);
	vec4 texColor = texture(texFuck,Coord);
	FragColor = LightColor * texColor;
}