#include "Texture.h"
#include "glutils.h"
#include <assert.h>
#include <windows.h>

#include <FreeImage.h>


Texture::Texture() : mhTex(0),
mPath(""),
mInternalFormat(0),
mFormat(0),
mPixelType(0),
mWidth(0),
mHeight(0)
{
}

Texture::~Texture()
{
}

void Texture::CreateEmptyTexture(int width, int height, GLenum format)
{
	glGenTextures(1, &mhTex);
	glBindTexture(GL_TEXTURE_2D, mhTex);
	if (format == GL_RGBA || format == GL_BGRA)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
	else if (format == GL_RGB || format == GL_BGR)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);
	glGenSamplers(1, &mhSampler);

	mWidth = width;
	mHeight = height;
	mFormat = format;
}

void Texture::CreateFromFile(const char* filePath, bool bMipmap)
{
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP* dib(0);

	fif = FreeImage_GetFileType(filePath, 0);

	if (fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilename(filePath);

	if (fif == FIF_UNKNOWN)
	{
		MessageBox(NULL, L"Unknown Image Type!", L"Alert!", MB_OKCANCEL);
		return;
	}

	if (FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, filePath);
	if (!dib)
	{
		MessageBox(NULL, L"Failed to load image!", L"Alert!", MB_OKCANCEL);
		return;
	}
	BYTE* dataPtr = FreeImage_GetBits(dib);

	mWidth = FreeImage_GetWidth(dib);
	mHeight = FreeImage_GetHeight(dib);
	int iBPP = FreeImage_GetBPP(dib);

	if (mWidth == 0 || mHeight == 0 || iBPP == 0)
	{
		MessageBox(NULL, L"Wrong image dimensions!", L"Error!", MB_OKCANCEL);
		return;
	}

	glGenTextures(1, &mhTex);
	glBindTexture(GL_TEXTURE_2D, mhTex);

	int iFormat = 0;
	int internalFormat = 0;

	//printf("Texture byte per pixel: %d.\n", iBPP);

	if (iBPP == 32)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, dataPtr);
	else if (iBPP == 24)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWidth, mHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, dataPtr);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, mWidth, mHeight, 0, GL_RED, GL_UNSIGNED_BYTE, dataPtr);

	if (bMipmap)
		glGenerateMipmap(GL_TEXTURE_2D);

	glGenSamplers(1, &mhSampler);

	mPath = std::string(filePath);
}

void Texture::SetSamplerParameters(GLenum parameter, GLenum xvalue)
{
	glSamplerParameteri(mhSampler, parameter, xvalue);
	//glTexParameteri(GL_TEXTURE_2D, parameter, xvalue);
}

void Texture::SetSamplerParameters(GLenum parameter, GLfloat* data)
{
	glSamplerParameterfv(mhSampler, parameter, data);
}

void Texture::BindTexture(int index /*= 0*/)
{
	mSamplerID = index;
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, mhTex);
	glBindSampler(index, mhSampler);
}

void Texture::DeleteTexture()
{
	glDeleteTextures(1, &mhTex);
	glDeleteSamplers(1, &mhSampler);
}

void Texture::CreateFloatPointTexture(int width, int height, GLenum format)
{
	glGenTextures(1, &mhTex);
	glBindTexture(GL_TEXTURE_2D, mhTex);
	if (format == GL_RGBA32F)
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
	else if (format == GL_RGB32F)
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	mWidth = width;
	mHeight = height;
	glGenSamplers(1, &mhSampler);
	mFormat = format;
}

void Texture::LoadUpData(void* pData)
{
	GLUtils::checkForOpenGLError(__FILE__, __LINE__);
	glBindTexture(GL_TEXTURE_2D, mhTex);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mWidth, mHeight, GL_RGB, GL_FLOAT, pData);
	GLUtils::checkForOpenGLError(__FILE__, __LINE__);
}
