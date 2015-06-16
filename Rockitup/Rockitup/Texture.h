#pragma once

#include <gl/glew.h>
#include <string>

enum TexParamType {
	MagFilter = GL_TEXTURE_MAG_FILTER,
	MinFilter = GL_TEXTURE_MIN_FILTER,
	WrapS = GL_TEXTURE_WRAP_S,
	WrapT = GL_TEXTURE_WRAP_T
};

enum TexParamValue {
	Linear = GL_LINEAR,
	Nearest = GL_NEAREST,
	Clamp = GL_CLAMP,
	Repeat = GL_REPEAT,
	ClampToBoarder = GL_CLAMP_TO_BORDER
};

class Texture
{
public:
	Texture();
	~Texture();

public:

	void CreateEmptyTexture(int width, int height, GLenum format);

	void CreateFloatPointTexture(int width, int height, GLenum format);

	void CreateFromFile(const char* filePath,bool bMipmap);

	void LoadUpData(void* pData);

	void DeleteTexture();

	void SetSamplerParameters(GLenum parameter, GLenum value);

	void SetSamplerParameters(GLenum parameter, GLfloat* data);

	void BindTexture(int index = 0);

	std::string GetPath() const
	{
		return mPath;
	}

	GLuint GetHandle() const
	{
		return mhTex;
	}

	int GetSamplerID(void) const
	{
		return mSamplerID;
	}

private:
	GLuint mhTex;
	GLuint mhSampler;

	int mSamplerID;

	std::string mPath;

	int mInternalFormat;
	int mFormat;
	int mPixelType;
	int mWidth;
	int mHeight;

};