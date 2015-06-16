--Vertex Shader--

#version 400

layout ( location = 0 ) in vec3 VertexPosition;
layout ( location = 1 ) in vec3 VertexNormal;
layout ( location = 2 ) in vec2 TexCoord;

out vec3 ePosition;
out vec3 eNormal;
out vec2 Coord;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 MVP;

void main()
{
	mat4 MV = ViewMatrix * ModelMatrix;
	ePosition = vec3( MV * vec4(VertexPosition,1.0f));
	eNormal = mat3(MV)*VertexNormal;
	Coord = TexCoord;
	gl_Position = MVP * vec4(VertexPosition,1.0f);
}

--Fragment Shader--

#version 400

layout ( location = 0 ) out vec4 FragColor;

in vec3 ePosition;
in vec3 eNormal;
in vec2 Coord;

uniform sampler2D tex;

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
	vec3 n = normalize(eNormal);
	vec3 l = normalize(Light0.Position - ePosition);
	vec3 e = normalize( - ePosition );
	vec3 r = reflect( -l , n );
	vec3 ambient = material.Ka;
	vec3 diffuse = material.Kd * max(dot(n,l),0.0f);
	vec3 specular = material.Ks * pow(max(dot(r,e),1.0),material.Shininess);
	vec4 LightColor = vec4(Light0.Intensity * (ambient + diffuse + specular ),1.0f);
	vec4 texColor = texture(tex,Coord);
	FragColor = LightColor * texColor;//vec4(Coord.x,Coord.y,0.0f,1.0f);


//	float h11 = texture(tex,Coord).r;
//	float h01 = textureOffset(tex,Coord,ivec2(-1,0)).x;
//	float h21 = textureOffset(tex,Coord,ivec2(1,0)).x;
//	float h10 = textureOffset(tex,Coord,ivec2(0,-1)).x;
//	float h12 = textureOffset(tex,Coord,ivec2(0,1)).x;

//	vec3 va = normalize(vec3(2.0,0.0,h21-h01));
//	vec3 vb = normalize(vec3(0.0,2.0,h12-h10));
	
//	FragColor = vec4(cross(va,vb),h11);
}