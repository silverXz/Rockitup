--Vertex Shader--

#version 400

layout (location = 0 ) in vec3 VertexPosition;
layout ( location = 1 ) in vec3 VertexNormal;
layout ( location = 2 ) in vec2 TexCoord;

noperspective out vec2 Coord;

void main()
{
	Coord = TexCoord;
	gl_Position = vec4(VertexPosition,1.0f);
}

--Fragment Shader--

#version 400

layout ( location = 0 ) out vec3 BlurAO;

noperspective in vec2 Coord;

uniform sampler2D TexAO;

uniform int uBlurSize = 4;

void main()
{
	vec2 texelSize = 1.0f / vec2(textureSize(TexAO,0));
	float result = 0.0f;
	vec2 hlim = vec2(float(-uBlurSize)*0.5 + 0.5);
	for( int i = 0 ; i < uBlurSize ; i++ ) 
		for( int j = 0 ; j < uBlurSize ; j++ )
		{
			vec2 offset = (hlim + vec2(float(i),float(j))) * texelSize;
			result += texture(TexAO,Coord + offset).r;
		}
	BlurAO = vec3(result / float(uBlurSize * uBlurSize));
}