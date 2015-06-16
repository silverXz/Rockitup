#pragma once

#include "OglApp.h"

#include <memory>
#include "Texture.h"
#include "SiMath.h"

class ShaderLib;
class Quad;


namespace iv {
	class SilMesh;
	class Camera;
	class FPSController;
}


class MyApp : public OglApp
{
public:
	MyApp(HINSTANCE hInstance, BOOL bShowConsole = TRUE);
	~MyApp();

	void Render_MRT();
	void RenderSSAO();
	void RenderLightDepth();
	void RenderForward();


	void Update(double deltaTime);
	void Render();
	void InitScene();
	LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);

	void InitBuffers();

private:

	// For shadow map
	iv::ivec2 mShadowMapResolution;


	Texture mShadowMap;
	Texture	mTexture;
	Texture mTexNormal;
	Texture mTexAmbient;
	Texture mTexDiffuse;
	Texture mTexSpecular;
	Texture mTexAO;
	Texture mTexRandom;
	Texture mTexBlur;
	Texture mTexColor;

	GLuint m_hShadowMapFbo;
	GLuint m_hFBO;
	GLuint m_hAO;
	GLuint m_hBlur;
	GLuint m_hRandomTex;

	std::shared_ptr<iv::Camera>				mpLightVirtualCamera;
	std::shared_ptr<iv::Camera>				mpCamera;
	std::shared_ptr<iv::SilMesh>			mpMesh;
	std::shared_ptr<ShaderLib>				mpShaderLib;
	std::shared_ptr<Quad>					mpQuad;
	std::shared_ptr<iv::FPSController>		mpCameraController;

	iv::vec3 Samples[16];
};