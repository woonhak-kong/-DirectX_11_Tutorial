#pragma once
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

// GLOBALS //
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


class D3DClass;
class CameraClass;
class ModelClass;
//class ColorShaderClass;
//class TextureShaderClass;
class LightShaderClass;
class LightClass;

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass& other);
	~GraphicsClass();


	bool Initialize(int screenWidth, int screenHeight, HWND hwnd);
	void Shutdown();
	bool Frame();

private:
	bool Render(float rotation);

private:
	D3DClass* m_D3D = nullptr;
	CameraClass* m_Camera = nullptr;
	ModelClass* m_Model = nullptr;
	//TextureShaderClass* m_TextureShader = nullptr;
	//ColorShaderClass* m_ColorShader = nullptr;
	LightShaderClass* m_LightShader = nullptr;
	LightClass* m_Light = nullptr;
};

#endif // _GRAPHICSCLASS_H_