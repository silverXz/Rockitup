#include "ShaderLib.h"

#include "GLShader.h"

ShaderLib::ShaderLib()
{
	for(int i = 0 ; i < E_SHADER_NUMBER ; i++)
		m_pShaderLib[i] = NULL;
}

ShaderLib::~ShaderLib()
{
	for (int i = 0 ; i < E_SHADER_NUMBER ; i++)
	{
		if(m_pShaderLib[i])
			delete m_pShaderLib[i],m_pShaderLib[i] = NULL;
	}
}

void ShaderLib::CompileAndLinkAllShaderPrograms()
{
	m_pShaderLib[E_GENERAL_SHADER] = new GLShader;

	m_pShaderLib[E_GENERAL_SHADER]->Load("shader/DefaultMaterialRender.glsl");

	m_pShaderLib[E_MRT_SHADER] = new GLShader;


	m_pShaderLib[E_MRT_SHADER]->Load( "shader/MRT.glsl");

	m_pShaderLib[E_QUAD_SHADER] = new GLShader;

	m_pShaderLib[E_QUAD_SHADER]->Load( "shader/quad.glsl");

	m_pShaderLib[E_SSAOP1_SHADER] = new GLShader;

	m_pShaderLib[E_SSAOP1_SHADER]->Load( "shader/SSAOP1.glsl");

	m_pShaderLib[E_SSAOP2_SHADER] = new GLShader;

	m_pShaderLib[E_SSAOP2_SHADER]->Load("shader/SSAOP2.glsl");

	m_pShaderLib[E_SSAOP3_SHADER] = new GLShader;

	m_pShaderLib[E_SSAOP3_SHADER]->Load("shader/SSAOP3.glsl");

	m_pShaderLib[E_SSAOP4_SHADER] = new GLShader;

	m_pShaderLib[E_SSAOP4_SHADER]->Load("shader/SSAOP4.glsl");

	m_pShaderLib[E_TexMesh_Shader] = new GLShader;

	m_pShaderLib[E_TexMesh_Shader]->Load("shader/TexMesh.glsl");

	m_pShaderLib[E_DrawLine_Shader] = new GLShader;

	m_pShaderLib[E_DrawLine_Shader]->Load("shader/DrawLine.glsl");

	m_pShaderLib[E_AxisQuad_Shader] = new GLShader;

	m_pShaderLib[E_AxisQuad_Shader]->Load("shader/AxisQuad.glsl");

	m_pShaderLib[E_DrawATexture] = new GLShader;

	m_pShaderLib[E_DrawATexture]->Load("shader/DrawAQuadTexture.glsl");

	m_pShaderLib[E_ShadowMapP2] = new GLShader;

	m_pShaderLib[E_ShadowMapP2]->Load("shader/ShadowMap_P2.glsl");

	m_pShaderLib[E_Default] = new GLShader;

	m_pShaderLib[E_Default]->Load("shader/Default.glsl");
	
}

GLShader* ShaderLib::GetShaderProgram(E_SHADER_TYPE effect) const
{
	return m_pShaderLib[effect];
}