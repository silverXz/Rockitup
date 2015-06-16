#include "MyApp.h"

#include <windowsx.h>
#include <time.h>
#include "SilMesh.h"
#include "ShaderLib.h"
#include "Camera.h"
#include "Quad.h"
#include "FPSController.h"
#include "glutils.h"
#include "GLShader.h"

MyApp::MyApp(HINSTANCE hInstance, BOOL bShowConsole /*= TRUE*/) : OglApp(hInstance,bShowConsole)
{

}

MyApp::~MyApp()
{

}

void MyApp::InitScene()
{
	mpLightVirtualCamera = std::shared_ptr<iv::Camera>(new iv::Camera);

	mpCamera = std::shared_ptr<iv::Camera>(new iv::Camera);

	mpShaderLib = std::shared_ptr<ShaderLib>(new ShaderLib);
	mpShaderLib->CompileAndLinkAllShaderPrograms();
	
	mpQuad = std::shared_ptr<Quad>(new Quad);

	mpCameraController = std::shared_ptr<iv::FPSController>(new iv::FPSController);
	mpCameraController->Attach(mpCamera);

	mpMesh = std::shared_ptr<iv::SilMesh>(new iv::SilMesh);
	mpMesh->LoadMesh("crytek-sponza\\sponza.obj");

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glEnable(GL_DEPTH_TEST);

	std::srand(50);
	for (int i = 0; i < 16; i++)
	{
		Samples[i].x() = 2.0f * ((float)std::rand() / RAND_MAX) - 1.0f;
		Samples[i].y() = 2.0f * ((float)std::rand() / RAND_MAX) - 1.0f;
		Samples[i].z() = (float)std::rand() / RAND_MAX;
		Samples[i] = iv::normalize(Samples[i]);
		/*float scale = (float)i / 16;
		float t = scale * scale;
		scale = 0.1f * (1.0f - t) + 1.0f * t;
		Samples[i] *= scale;*/
	}

	InitBuffers();

	ShowCursor(FALSE);
}

void MyApp::Update(double deltaTime)
{
	mpCameraController->Update(deltaTime);
}

void MyApp::Render()
{
	//RenderForward();
	RenderLightDepth();
	//Render_MRT();
	//RenderSSAO();
}

void MyApp::InitBuffers()
{
	mShadowMapResolution = iv::ivec2(1 * mWidth, 1 * mHeight);

	glGenFramebuffers(1, &m_hShadowMapFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_hShadowMapFbo);

	GLfloat border[] = { 1.0f, 0.0f, 0.0f, 1.0f };

	mShadowMap.CreateEmptyTexture(mShadowMapResolution.x(), mShadowMapResolution.y(), GL_DEPTH_COMPONENT);
	mShadowMap.SetSamplerParameters(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	mShadowMap.SetSamplerParameters(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	mShadowMap.SetSamplerParameters(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	mShadowMap.SetSamplerParameters(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	mShadowMap.SetSamplerParameters(GL_TEXTURE_BORDER_COLOR, border);
	mShadowMap.SetSamplerParameters(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	mShadowMap.SetSamplerParameters(GL_TEXTURE_COMPARE_FUNC, GL_LESS);
	GLUtils::checkForOpenGLError(__FILE__, __LINE__);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mShadowMap.GetHandle(), 0);
	GLUtils::checkForOpenGLError(__FILE__, __LINE__);
	GLenum fucks[] = { GL_NONE };

	glDrawBuffers(1, fucks);

	GLenum fuckers = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fuckers != GL_FRAMEBUFFER_COMPLETE)
	{
		switch (fuckers)
		{
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		{
			printf("Failed! InComplete Attachment!\n");
			break;
		}
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
		{
			printf("Failed! Incompelete Dimension!\n");
			break;
		}
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		{
			printf("Failed! Incomplete Missing Attachment!\n");
			break;
		}
		case GL_FRAMEBUFFER_UNSUPPORTED:
		{
			printf("Failed! Framebuffer Unsupported!\n");
			break;
		}
		default:
			break;
		}
	}
	else
		printf("Framebuffer Complete!\n");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	glGenFramebuffers(1, &m_hFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_hFBO);

	mTexNormal.CreateFloatPointTexture(mWidth, mHeight, GL_RGBA32F);
	mTexNormal.SetSamplerParameters(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	mTexNormal.SetSamplerParameters(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	mTexNormal.SetSamplerParameters(GL_TEXTURE_WRAP_S, GL_CLAMP);
	mTexNormal.SetSamplerParameters(GL_TEXTURE_WRAP_T, GL_CLAMP);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexNormal.GetHandle(), 0);

	mTexAmbient.CreateEmptyTexture(mWidth, mHeight, GL_RGB);
	mTexAmbient.SetSamplerParameters(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	mTexAmbient.SetSamplerParameters(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	mTexAmbient.SetSamplerParameters(GL_TEXTURE_WRAP_S, GL_CLAMP);
	mTexAmbient.SetSamplerParameters(GL_TEXTURE_WRAP_T, GL_CLAMP);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mTexAmbient.GetHandle(), 0);


	GLUtils::checkForOpenGLError(__FILE__, __LINE__);

	mTexDiffuse.CreateEmptyTexture(mWidth, mHeight, GL_RGB);
	mTexDiffuse.SetSamplerParameters(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	mTexDiffuse.SetSamplerParameters(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	mTexDiffuse.SetSamplerParameters(GL_TEXTURE_WRAP_S, GL_CLAMP);
	mTexDiffuse.SetSamplerParameters(GL_TEXTURE_WRAP_T, GL_CLAMP);


	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, mTexDiffuse.GetHandle(), 0);

	mTexSpecular.CreateEmptyTexture(mWidth, mHeight, GL_RGB);
	mTexSpecular.SetSamplerParameters(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	mTexSpecular.SetSamplerParameters(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	mTexSpecular.SetSamplerParameters(GL_TEXTURE_WRAP_S, GL_CLAMP);
	mTexSpecular.SetSamplerParameters(GL_TEXTURE_WRAP_T, GL_CLAMP);


	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, mTexSpecular.GetHandle(), 0);

	mTexColor.CreateEmptyTexture(mWidth, mHeight, GL_RGB);
	mTexColor.SetSamplerParameters(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	mTexColor.SetSamplerParameters(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	mTexColor.SetSamplerParameters(GL_TEXTURE_WRAP_S, GL_CLAMP);
	mTexColor.SetSamplerParameters(GL_TEXTURE_WRAP_T, GL_CLAMP);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, mTexColor.GetHandle(), 0);

	GLUtils::checkForOpenGLError(__FILE__, __LINE__);

	// Render buffer for depth test.
	GLuint depthRenderBuffer;
	glGenRenderbuffers(1, &depthRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, mWidth, mHeight);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);

	GLenum drawbuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };

	glDrawBuffers(5, drawbuffers);

	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (result != GL_FRAMEBUFFER_COMPLETE)
	{
		switch (result)
		{
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		{
			printf("Failed! InComplete Attachment!\n");
			break;
		}
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
		{
			printf("Failed! Incompelete Dimension!\n");
			break;
		}
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		{
			printf("Failed! Incomplete Missing Attachment!\n");
			break;
		}
		case GL_FRAMEBUFFER_UNSUPPORTED:
		{
			printf("Failed! Framebuffer Unsupported!\n");
			break;
		}
		default:
			break;
		}
	}
	else
		printf("Framebuffer Complete!\n");

	GLUtils::checkForOpenGLError(__FILE__, __LINE__);

	glGenFramebuffers(1, &m_hAO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_hAO);


	mTexAO.CreateFloatPointTexture(mWidth, mHeight, GL_RGB32F);
	mTexAO.SetSamplerParameters(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	mTexAO.SetSamplerParameters(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	mTexAO.SetSamplerParameters(GL_TEXTURE_WRAP_S, GL_CLAMP);
	mTexAO.SetSamplerParameters(GL_TEXTURE_WRAP_T, GL_CLAMP);
	GLUtils::checkForOpenGLError(__FILE__, __LINE__);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexAO.GetHandle(), 0);

	GLenum drawbuffers2[] = { GL_COLOR_ATTACHMENT0 };

	glDrawBuffers(1, drawbuffers2);

	result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (result != GL_FRAMEBUFFER_COMPLETE)
	{
		switch (result)
		{
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		{
			printf("Failed! InComplete Attachment!\n");
			break;
		}
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
		{
			printf("Failed! Incompelete Dimension!\n");
			break;
		}
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		{
			printf("Failed! Incomplete Missing Attachment!\n");
			break;
		}
		case GL_FRAMEBUFFER_UNSUPPORTED:
		{
			printf("Failed! Framebuffer Unsupported!\n");
			break;
		}
		default:
			break;
		}
	}
	else
		printf("Framebuffer Complete!\n");

	GLUtils::checkForOpenGLError(__FILE__, __LINE__);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	mTexRandom.CreateFloatPointTexture(4, 4, GL_RGB32F);
	mTexRandom.SetSamplerParameters(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	mTexRandom.SetSamplerParameters(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	mTexRandom.SetSamplerParameters(GL_TEXTURE_WRAP_S, GL_REPEAT);
	mTexRandom.SetSamplerParameters(GL_TEXTURE_WRAP_T, GL_REPEAT);

	std::srand(100);
	iv::vec3 rVecs[4 * 4];
	for (int i = 0; i < 4 * 4; ++i)
	{
		rVecs[i].x() = 2.0f * ((float)std::rand() / RAND_MAX) - 1.0f;
		rVecs[i].y() = 2.0f * ((float)std::rand() / RAND_MAX) - 1.0f;
		rVecs[i].z() = 0.0f;
		rVecs[i] = iv::normalize(rVecs[i]);
	}
	mTexRandom.LoadUpData(rVecs);

	GLUtils::checkForOpenGLError(__FILE__, __LINE__);
	glGenFramebuffers(1, &m_hBlur);
	glBindFramebuffer(GL_FRAMEBUFFER, m_hBlur);

	mTexBlur.CreateFloatPointTexture(mWidth, mHeight, GL_RGB32F);
	mTexBlur.SetSamplerParameters(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	mTexBlur.SetSamplerParameters(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	mTexBlur.SetSamplerParameters(GL_TEXTURE_WRAP_S, GL_CLAMP);
	mTexBlur.SetSamplerParameters(GL_TEXTURE_WRAP_T, GL_CLAMP);

	GLUtils::checkForOpenGLError(__FILE__, __LINE__);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexBlur.GetHandle(), 0);

	GLenum drawbuffers3[] = { GL_COLOR_ATTACHMENT0 };

	glDrawBuffers(1, drawbuffers3);

	result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (result != GL_FRAMEBUFFER_COMPLETE)
	{
		switch (result)
		{
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		{
			printf("Failed! InComplete Attachment!\n");
			break;
		}
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
		{
			printf("Failed! Incompelete Dimension!\n");
			break;
		}
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		{
			printf("Failed! Incomplete Missing Attachment!\n");
			break;
		}
		case GL_FRAMEBUFFER_UNSUPPORTED:
		{
			printf("Failed! Framebuffer Unsupported!\n");
			break;
		}
		default:
			break;
		}
	}
	else
		printf("Framebuffer Complete!\n");

	GLUtils::checkForOpenGLError(__FILE__, __LINE__);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

LRESULT MyApp::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	//LRESULT lres;
	switch (msg)														// Evaluate Window Message
	{
	case WM_SYSCOMMAND:												// Intercept System Commands
	{
		switch (wParam)												// Check System Calls
		{
		case SC_SCREENSAVE:										// Screensaver Trying To Start?
			std::cout << "Fuck!\n";
			return 0;
		case SC_MONITORPOWER:									// Monitor Trying To Enter Powersave?
			return 0;												// Prevent From Happening
		}
		break;														// Exit
	}
	return 0;														// Return

	case WM_CLOSE:													// Closing The Window
		Terminate();								// Terminate The Application
		return 0;														// Return

	case WM_SIZE:													// Size Action Has Taken Place
		switch (wParam)												// Evaluate Size Action
		{
		case SIZE_MINIMIZED:									// Was Window Minimized?
			mbVisible = FALSE;							// Set isVisible To False
			return 0;												// Return

		case SIZE_MAXIMIZED:									// Was Window Maximized?
			mbVisible = TRUE;							// Set isVisible To True
			Resize(LOWORD(lParam), HIWORD(lParam));		// Reshape Window - LoWord=Width, HiWord=Height
			return 0;												// Return

		case SIZE_RESTORED:										// Was Window Restored?
			mbVisible = TRUE;							// Set isVisible To True
			Resize(LOWORD(lParam), HIWORD(lParam));		// Reshape Window - LoWord=Width, HiWord=Height
			return 0;												// Return
		}
		break;															// Break

	case WM_KEYDOWN:												// Update Keyboard Buffers For Keys Pressed
	{
		if ((wParam >= 0) && (wParam <= 255))						// Is Key (wParam) In A Valid Range?
		{
			mKeys.keyDown[wParam] = TRUE;					// Set The Selected Key (wParam) To True
			iv::KeyboardEvent eKey;
			eKey.mState = iv::Pressed;
			switch (wParam)
			{
			case 'W':
				eKey.mKey = iv::W;
				break;
			case 'S':
				eKey.mKey = iv::S;
				break;
			case 'A':
				eKey.mKey = iv::A;
				break;
			case 'D':
				eKey.mKey = iv::D;
				break;
			default:
				break;
			}
			mpCameraController->HandleKeyboardEvent(eKey);
			return 0;												// Return
		}
		break;															// Break
	}

	case WM_LBUTTONUP:									//Mouse Left button up
	{
		// 		m_bRightButtonDown = FALSE;
		// 		int xPos = GET_X_LPARAM(lParam);
		// 		int yPos = GET_Y_LPARAM(lParam);
		// 		m_pScene->Release(xPos, yPos);
		// 		m_pScene->GetPinched() = false;
		return 0;
		break;
	}

	case WM_MOUSEMOVE:								//
	{
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);
		iv::MouseEvent mouseEvent(iv::Mb_None, iv::Bs_None, xPos, yPos);
		mpCameraController->HandleMouseEvent(mouseEvent, mhWnd);
		return 0;
	}
	break;

	case WM_MOUSEWHEEL:								//
	{
		// 		if (m_pScene == NULL)
		// 			return 0;
		// 		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		// 		if (zDelta > 0)
		// 			m_pScene->ZoomOut();
		// 		else if (zDelta < 0)
		// 			m_pScene->ZoomIn();
		return 0;
	}
	break;

	case WM_KEYUP:													// Update Keyboard Buffers For Keys Released
		if ((wParam >= 0) && (wParam <= 255))						// Is Key (wParam) In A Valid Range?
		{
			mKeys.keyDown[wParam] = FALSE;					// Set The Selected Key (wParam) To False
			iv::KeyboardEvent eKey;
			eKey.mState = iv::Released;
			switch (wParam)
			{
			case 'W':
				eKey.mKey = iv::W;
				break;
			case 'S':
				eKey.mKey = iv::S;
				break;
			case 'A':
				eKey.mKey = iv::A;
				break;
			case 'D':
				eKey.mKey = iv::D;
				break;
			case VK_ESCAPE:
				Terminate();
				break;
			default:
				break;
			}
			mpCameraController->HandleKeyboardEvent(eKey);
			return 0;												// Return
		}
		break;															// Break

	case WM_TOGGLEFULLSCREEN:										// Toggle FullScreen Mode On/Off
		mbFullScreen = (mbFullScreen == TRUE) ? FALSE : TRUE;
		PostMessage(mhWnd, WM_QUIT, 0, 0);
		break;															// Break
	}

	return DefWindowProc(mhWnd, msg, wParam, lParam);					// Pass Unhandled Messages To DefWindowProc
}

void MyApp::Render_MRT()
{
	if (!mpMesh)
		return;

	GLShader* pShader = mpShaderLib->GetShaderProgram(E_MRT_SHADER);
	pShader->Use();
	GLUtils::checkForOpenGLError(__FILE__, __LINE__);
	glBindFramebuffer(GL_FRAMEBUFFER, m_hFBO);  // Render to FBO

	glViewport(0, 0, mWidth, mHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mpCamera->SetFrustrum(75.0f, static_cast<float>(mWidth) / static_cast<float>(mHeight), 1.0f, 5000.0f);

	iv::mat4 modelMatrix(1.0f);
	iv::mat4 viewMatrix = mpCamera->GetViewMatrix();
	iv::mat4 projectionMatrix = mpCamera->GetProjectionMatrix();
	iv::mat4 MVP = projectionMatrix * viewMatrix * modelMatrix;
	GLUtils::checkForOpenGLError(__FILE__, __LINE__);

	pShader->SetUniform("ModelMatrix", modelMatrix);
	pShader->SetUniform("ViewMatrix", viewMatrix);
	pShader->SetUniform("MVP", MVP);
	pShader->SetUniform("fuck", 0);

	pShader->SetUniform("material.Ka", iv::vec3(0.7f));
	pShader->SetUniform("material.Kd", iv::vec3(0.4f));
	pShader->SetUniform("material.Ks", iv::vec3(0.3f));
	GLUtils::checkForOpenGLError(__FILE__, __LINE__);
	mpMesh->Render();

	GLUtils::checkForOpenGLError(__FILE__, __LINE__);
#if 1
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // Render to back buffer
	glViewport(0, 0, mWidth, mHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLUtils::checkForOpenGLError(__FILE__, __LINE__);
	pShader = mpShaderLib->GetShaderProgram(E_QUAD_SHADER);
	pShader->Use();
	GLUtils::checkForOpenGLError(__FILE__, __LINE__);
	iv::vec4 LightWorldPosition(100.0f, 100.0f, 100.0f, 1.0f);
	iv::vec3 LightIntensity(0.75f);
	pShader->SetUniform("Light0.Position", iv::vec3(viewMatrix * LightWorldPosition));

	pShader->SetUniform("Light0.Intensity", LightIntensity);
	GLUtils::checkForOpenGLError(__FILE__, __LINE__);
	mTexNormal.BindTexture(0);
	mTexAmbient.BindTexture(1);
	mTexDiffuse.BindTexture(2);
	mTexSpecular.BindTexture(3);
	mTexColor.BindTexture(4);
	pShader->SetUniform("TexNormalDepth", mTexNormal.GetSamplerID());		// FragNormal And Depth
	pShader->SetUniform("TexAmbient", mTexAmbient.GetSamplerID());		// FragDepth
	pShader->SetUniform("TexDiffuse", mTexDiffuse.GetSamplerID());
	pShader->SetUniform("TexSpecular", mTexSpecular.GetSamplerID());
	pShader->SetUniform("fuck", mTexColor.GetSamplerID());
	GLUtils::checkForOpenGLError(__FILE__, __LINE__);

	pShader->SetUniform("InverseProjectionMatrix", iv::inverse(projectionMatrix));

	mpQuad->render();

	GLUtils::checkForOpenGLError(__FILE__, __LINE__);
#endif
}

void MyApp::RenderSSAO()
{
	if (!mpMesh)
		return;

	GLShader* pShader = mpShaderLib->GetShaderProgram(E_SSAOP1_SHADER);
	pShader->Use();

	GLUtils::checkForOpenGLError(__FILE__, __LINE__);


	glBindFramebuffer(GL_FRAMEBUFFER, m_hFBO);  // Render to FBO

	glViewport(0, 0, mWidth, mHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mpCamera->SetFrustrum(75.0f, static_cast<float>(mWidth) / mHeight, 1.0f, 5000.0f);

	iv::mat4 modelMat(1.0f);
	iv::mat4 vMat = mpCamera->GetViewMatrix();
	iv::mat4 projMat = mpCamera->GetProjectionMatrix();
	iv::mat4 MVP = projMat * vMat * modelMat;

	pShader->SetUniform("ModelMatrix", modelMat);
	pShader->SetUniform("ViewMatrix", vMat);
	pShader->SetUniform("MVP", MVP);
	pShader->SetUniform("fuck", 0);

	pShader->SetUniform("material.Ka", iv::vec3(0.7f));
	pShader->SetUniform("material.Kd", iv::vec3(0.4f));
	pShader->SetUniform("material.Ks", iv::vec3(0.3f));
	mpMesh->Render();



	GLUtils::checkForOpenGLError(__FILE__, __LINE__);


	glBindFramebuffer(GL_FRAMEBUFFER, m_hAO); // Render to AO Buffer
	glViewport(0, 0, mWidth, mHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	pShader = mpShaderLib->GetShaderProgram(E_SSAOP2_SHADER);
	pShader->Use();

	pShader->SetUniform("power", 1.0f);

	iv::mat4 inverProjMat = iv::inverse(projMat);
	int loc = glGetUniformLocation(pShader->GetHandle(), "InverseProjectionMatrix");
	if (loc >= 0)
		glUniformMatrix4fv(loc, 1, GL_FALSE, inverProjMat.v);

	//glm::mat4 invP = glm::inverse(projectionMatrix);
	//pShader->SetUniform("InverseProjectionMatrix",invP);
	mTexNormal.BindTexture(0);
	pShader->SetUniform("TexNormalDepth", mTexNormal.GetSamplerID());		// FragColor
	mTexRandom.BindTexture(1);
	pShader->SetUniform("TexRandom", mTexRandom.GetSamplerID());
	pShader->SetUniform("ProjectionMatrix", projMat);
	pShader->SetUniform("NoiseScale", iv::vec2(mWidth / 4.0f, mHeight / 4.0f));
	pShader->SetUniform("Radius", 5.5f);

	char UniName[20];
	for (int i = 0; i < 16; i++)
	{
		sprintf_s(UniName, 20, "Samples[%d]", i);
		pShader->SetUniform(UniName, Samples[i]);
	}

	int max_Texel_Offset = 0;
	glGetIntegerv(GL_MIN_PROGRAM_TEXEL_OFFSET, &max_Texel_Offset);

	mpQuad->render();

	pShader = mpShaderLib->GetShaderProgram(E_SSAOP3_SHADER);
	pShader->Use();
	glBindFramebuffer(GL_FRAMEBUFFER, m_hBlur);
	glViewport(0, 0, mWidth, mHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mTexAO.BindTexture(3);
	pShader->SetUniform("TexAO", mTexAO.GetSamplerID());

	mpQuad->render();

	// Pass 4 Render to Screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, mWidth, mHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	pShader = mpShaderLib->GetShaderProgram(E_SSAOP4_SHADER);
	pShader->Use();

	//Vertex Shader
	pShader->SetUniform("InverseProjectionMatrix", iv::inverse(projMat));

	mTexAmbient.BindTexture(4);
	mTexDiffuse.BindTexture(5);
	mTexSpecular.BindTexture(6);
	mTexNormal.BindTexture(0);
	mTexBlur.BindTexture(7);
	mTexColor.BindTexture(10);

	pShader->SetUniform("TexAmbient", mTexAmbient.GetSamplerID());
	pShader->SetUniform("TexDiffuse", mTexDiffuse.GetSamplerID());
	pShader->SetUniform("TexSpecular", mTexSpecular.GetSamplerID());
	pShader->SetUniform("TexNormalDepth", mTexNormal.GetSamplerID());
	pShader->SetUniform("TexBlur", mTexBlur.GetSamplerID());
	pShader->SetUniform("TexColor", mTexColor.GetSamplerID());
	iv::vec4 LightWorldPosition(100.0f, 100.0f, 100.0f, 1.0f);
	iv::vec3 LightIntensity(0.75f);
	pShader->SetUniform("Light0.Position", iv::vec3(vMat * LightWorldPosition));
	pShader->SetUniform("Light0.Intensity", LightIntensity);


	mpQuad->render();
}

void MyApp::RenderLightDepth()
{
	if (!mpMesh)
		return;

	GLShader* pShader = mpShaderLib->GetShaderProgram(E_GENERAL_SHADER);
	pShader->Use();



	glBindFramebuffer(GL_FRAMEBUFFER, m_hShadowMapFbo);  // Render to back buffer
	//glViewport(0, 0, mWidth, mHeight);
	glViewport(0, 0, mShadowMapResolution.x(), mShadowMapResolution.y());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	iv::vec3 lightPosition(-1922.61f, 2143.87f, -32.5507f);

	mpLightVirtualCamera->SetPosition(lightPosition);
	mpLightVirtualCamera->SetFaceOrientation(iv::vec3(0.756301, -0.654213f, 0.003664f));
	mpLightVirtualCamera->SetFrustrum(90.0f, static_cast<float>(mWidth) / static_cast<float>(mHeight), 1.0f, 5000.0f);

	iv::mat4 modelMatrix(1.0f);
	iv::mat4 viewMatrix = mpLightVirtualCamera->GetViewMatrix();
	iv::mat4 projectionMatrix = mpLightVirtualCamera->GetProjectionMatrix();
	iv::mat4 MVP = projectionMatrix * viewMatrix * modelMatrix;

	pShader->SetUniform("MVP", MVP);

	mpMesh->Render();

#if 1
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, mWidth, mHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mpCamera->SetFrustrum(90.0f, static_cast<float>(mWidth) / static_cast<float>(mHeight), 1.0f, 5000.0f);
	pShader = mpShaderLib->GetShaderProgram(E_ShadowMapP2);;
	pShader->Use();
	// Vertex Shader Uniforms
	iv::mat4 camViewMat = mpCamera->GetViewMatrix();
	iv::mat4 camProjMat = mpCamera->GetProjectionMatrix();
	iv::mat4 MV = camViewMat * iv::mat4(1.0f);
	iv::mat4 MVPx = camProjMat * MV;
	pShader->SetUniform("MV", MV);
	pShader->SetUniform("MVP", MVPx);

	iv::mat4 biasMat = iv::translate(iv::vec3(0.5f)) * iv::scale(0.5f);
	iv::mat4 shadowMatrix = biasMat * mpLightVirtualCamera->GetProjectionMatrix() * mpLightVirtualCamera->GetViewMatrix();

	pShader->SetUniform("ShadowMatrix", shadowMatrix);

	mShadowMap.BindTexture(10);
	pShader->SetUniform("texShadowMap", 10);
	pShader->SetUniform("texFuck", 0);

	pShader->SetUniform("material.Ka", iv::vec3(0.7f));
	pShader->SetUniform("material.Kd", iv::vec3(0.4f));
	pShader->SetUniform("material.Ks", iv::vec3(0.3f));
	pShader->SetUniform("material.Shininess", 128.0f);

	iv::vec3 LightIntensity(0.75f);
	pShader->SetUniform("Light0.Position", iv::vec3(camViewMat * iv::vec4(lightPosition,1.0f)));
	pShader->SetUniform("Light0.Intensity", LightIntensity);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(-2.0f, -0.5f);

	mpMesh->Render();
#endif // 0


}

void MyApp::RenderForward()
{
	if (!mpMesh)
		return;

	GLShader* pShader = mpShaderLib->GetShaderProgram(E_Default);
	pShader->Use();


	glBindFramebuffer(GL_FRAMEBUFFER, 0);  // Render to back buffer
	glViewport(0, 0, mWidth, mHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mpCamera->SetFrustrum(90.0f, static_cast<float>(mWidth) / static_cast<float>(mHeight), 1.0f, 10000.0f);

	iv::mat4 modelMatrix(1.0f);
	iv::mat4 viewMatrix = mpCamera->GetViewMatrix();
	iv::mat4 projectionMatrix = mpCamera->GetProjectionMatrix();
	iv::mat4 MVP = projectionMatrix * viewMatrix * modelMatrix;

	pShader->SetUniform("ModelMatrix", modelMatrix);
	pShader->SetUniform("ViewMatrix", viewMatrix);
	pShader->SetUniform("MVP", MVP);
	pShader->SetUniform("tex", 0);


	iv::vec4 LightWorldPosition(100.0f, 100.0f, 100.0f, 1.0f);
	iv::vec3 LightIntensity(1.0f);
	pShader->SetUniform("Light0.Position", iv::vec3(viewMatrix * LightWorldPosition));
	pShader->SetUniform("Light0.Intensity", LightIntensity);

	pShader->SetUniform("material.Ka", iv::vec3(0.4f));
	pShader->SetUniform("material.Kd", iv::vec3(0.6f));
	pShader->SetUniform("material.Ks", iv::vec3(0.3f));

	pShader->SetUniform("material.Shininess", 128.0f);
	//mTexture.Bind();
	mpMesh->Render();
}


