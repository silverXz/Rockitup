#pragma once
#include <string>
#include "SiMath.h"
#include <gl/glew.h>
#include "ShaderFileParser.h"

namespace GLSLShader 
{
	enum GLSLShaderType 
	{
		VERTEX, FRAGMENT, GEOMETRY,
		TESS_CONTROL, TESS_EVALUATION
	};
};

class GLShader
{
public:
	GLShader(void);
	~GLShader(void);

public:


	// Vertex, Geometry, Fragment Shader
	bool Load(	const char * path0, GLSLShader::GLSLShaderType type0,
				const char * path1, GLSLShader::GLSLShaderType type1,
				const char * path2, GLSLShader::GLSLShaderType type2 );

	// Vertex, Fragment Shader
	bool Load(	const char * path0, GLSLShader::GLSLShaderType type0,
				const char * path1, GLSLShader::GLSLShaderType type1);
	bool Load(const char* filePath);


	void Use();

	void   SetUniform( const char *name, float x, float y, float z);
	void   SetUniform(const char* name, const iv::vec2& v);
	void   SetUniform( const char* name, const iv::vec3& v);
	void   SetUniform( const char *name, const iv::vec4& v);
	void   SetUniform( const char *name, const iv::mat4& m);
	void   SetUniform( const char *name, const iv::mat3& m);
	void   SetUniform( const char *name, float val );
	void   SetUniform( const char *name, int val );
	void   SetUniform( const char *name, bool val );
	int    GetHandle();

private:

	int  GetUniformLocation(const char * name );
	bool FileExists( const std::string & fileName );

	bool   CompileShaderFromFile( const char * fileName, GLSLShader::GLSLShaderType type );
	bool   CompileShaderFromString( const std::string & source, GLSLShader::GLSLShaderType type );
	bool   Link();
	bool   Validate();
	
	std::string GetLog();


	bool   IsLinked();

	void   BindAttribLocation( GLuint location, const char * name);
	void   BindFragDataLocation( GLuint location, const char * name );

	

private:
	int		m_hProgram;
	bool	m_bLinked;
	std::string	m_sLog;

};

