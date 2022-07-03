#include "stdafx.h"
#include "graphicsclass.h"

#include "BitmapClass.h"
#include "D3dclass.h"
#include "CameraClass.h"
#include "ModelClass.h"
#include "LightShaderClass.h"
#include "LightClass.h"
#include "TextureShaderClass.h"

GraphicsClass::GraphicsClass()
{
}

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	// Direct3D ��ü ����
	m_D3D = new D3DClass;
	if (!m_D3D)
	{
		return false;
	}

	// Direct3D ��ü�� �ʱ�ȭ �Ѵ�.
	if (!m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}


	// m_Camera ��ü ����
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	// ī�޶� ������ ����
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	//// m_Model ��ü ����
	//m_Model = new ModelClass;
	//if (!m_Model)
	//{
	//	return false;
	//}

	//// m_Model �ʱ�ȭ
	//if (!m_Model->Initialize(m_D3D->GetDevice(), "./cube.txt", L"./Textures/WoodCrate01.dds"))
	//{
	//	MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
	//}

	// �ؽ��� ���̴� ��ü ����
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
	{
		return false;
	}

	//// �ؽ��� ���̴� ���� �ʱ�ȭ
	if (!m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize texture shader object.", L"Error", MB_OK);
		return false;
	}

	//// LightShaderClass ��ü ����
	//m_LightShader = new LightShaderClass;
	//if (!m_LightShader)
	//{
	//	return false;
	//}

	//// LightShader ��ü�� �ʱ�ȭ�Ѵ�.
	//if (!m_LightShader->Initialize(m_D3D->GetDevice(), hwnd))
	//{
	//	MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
	//	return false;
	//}

	//// LightClass ��ü ����
	//m_Light = new LightClass;
	//if (!m_Light)
	//{
	//	return false;
	//}

	//// Light ��ü �ʱ�ȭ
	//m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	//m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	//m_Light->SetDirection(1.0f, 0.0f, 1.0f);
	//m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	//m_Light->SetSpecularPower(32.0f);

	// ��Ʈ�� ��ü ����
	m_Bitmap = new BitmapClass;
	if (!m_Bitmap)
	{
		return false;
	}

	// ��Ʈ�� ��ü �ʱ�ȭ
	if (!m_Bitmap->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, L"./Textures/WoodCrate01.dds",
		512, 512))
	{
		MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void GraphicsClass::Shutdown()
{
	// ��Ʈ�� ��ü ����
	if (m_Bitmap)
	{
		m_Bitmap->Shutdown();
		delete m_Bitmap;
		m_Bitmap = nullptr;
	}
	//// light ��ü ����
	//if (m_Light)
	//{
	//	delete m_Light;
	//	m_Light = nullptr;
	//}

	//// LightShader ��ü ����
	//if (m_LightShader)
	//{
	//	m_LightShader->Shutdown();
	//	delete m_LightShader;
	//	m_LightShader = nullptr;
	//}

	// m_TextureShader ��ü ��ȯ
	//if (m_TextureShader)
	//{
	//	m_TextureShader->Shutdown();
	//	delete m_TextureShader;
	//	m_TextureShader = nullptr;
	//}

	//// m_Model ��ü ��ȯ
	//if (m_Model)
	//{
	//	m_Model->Shutdown();
	//	delete m_Model;
	//	m_Model = nullptr;
	//}

	// m_Camera ��ü ��ȯ
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = nullptr;
	}

	// D3D ��ü�� ��ȯ�մϴ�.
	if (m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = nullptr;
	}
}

bool GraphicsClass::Frame()
{
	static float rotation = 0.0f;

	// �� ������ ���� rotation �������� ������Ʈ �Ѵ�.
	rotation += XM_PI * 0.005f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}
	//�׷��� �������� �����մϴ�.
	if (!Render(rotation))
	{
		
		return false;
	}
	return true;
}

bool GraphicsClass::Render(float rotation)
{
	// �� �׸��⸦ �����ϱ� ���� ������ ������ ����ϴ�.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ����� �����Ѵ�.
	m_Camera->Render();

	// ī�޶� �� d3d ��ü���� ����, �� �� ���� ����� �����´�.
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetOrthoMatrix(orthoMatrix);

	// 2D �������� �����ϱ� ���� Z���۸� ����.
	m_D3D->TurnZBufferOff();

	// ��Ʈ�� ���ؽ��� �ε��� ���۸� �׷��� ������ ���ο� ��ġ�Ͽ� �׸��⸦ �غ��Ѵ�.
	if (!m_Bitmap->Render(m_D3D->GetDeviceContext(), 100, 100))
	{
		return false;
	}

	// ������ ȸ�� �� �� �ֵ��� ȸ�� ������ ���� ����� ȸ���Ѵ�.
	// = XMMatrixRotationY(rotation);

	// �� ���ؽ��� �ε��� ���۸� �׷��� ������ ���ο� ��ġ�Ͽ� ������� �غ��Ѵ�.
	//m_Model->Render(m_D3D->GetDeviceContext());


	// Light ���̴��� ����Ͽ� ���� ������ �Ѵ�.
	//if (!m_LightShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix,
	//	viewMatrix, projectionMatrix, m_Model->GetTexture(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor()
	//	, m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower()))
	//{
	//	return false;
	//}


	// �ؽ�ó ���̴��� ����Ͽ� ���� ������ �Ѵ�.
	if (!m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Bitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix,
		m_Bitmap->GetTexture()))
	{
		return false;
	}

	// ���2D �������� �Ϸ�Ǿ����Ƿ� Z���۸� �ٽ� �Ҵ�.
	m_D3D->TurnZBufferOn();

	// ���ۿ� �׷��� ���� ȭ�鿡 ǥ���մϴ�.
	m_D3D->EndScene();

	return true;
}
