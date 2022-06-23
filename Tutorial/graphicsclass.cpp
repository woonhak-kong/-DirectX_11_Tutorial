#include "stdafx.h"
#include "graphicsclass.h"
#include "D3dclass.h"
#include "CameraClass.h"
#include "ModelClass.h"
#include "ColorshaderClass.h"
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
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	// m_Model ��ü ����
	m_Model = new ModelClass;
	if (!m_Model)
	{
		return false;
	}

	// m_Model �ʱ�ȭ
	if (!m_Model->Initialize(m_D3D->GetDevice(), L"./Textures/WoodCrate01.dds"))
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
	}

	// �ؽ��� ���̴� ��ü ����
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
	{
		return false;
	}

	// �ؽ��� ���̴� ���� �ʱ�ȭ
	if (!m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize texture shader object.", L"Error", MB_OK);
		return false;
	}

	//// m_ColorShader ��ü ����
	//m_ColorShader = new ColorShaderClass;
	//if (!m_ColorShader)
	//{
	//	return false;
	//}

	//// m_ColorShader ��ü �ʱ�ȭ
	//if (!m_ColorShader->Initialize(m_D3D->GetDevice(), hwnd))
	//{
	//	MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
	//	return false;
	//}

	return true;
}

void GraphicsClass::Shutdown()
{
	// m_ColorShader ��ü ��ȯ
	//if (m_ColorShader)
	//{
	//	m_ColorShader->Shutdown();
	//	delete m_ColorShader;
	//	m_ColorShader = nullptr;
	//}

	// m_TextureShader ��ü ��ȯ
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = nullptr;
	}

	// m_Model ��ü ��ȯ
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = nullptr;
	}

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
	
	//�׷��� �������� �����մϴ�.
	if (!Render())
	{
		
		return false;
	}
	return true;
}

bool GraphicsClass::Render()
{
	// �� �׸��⸦ �����ϱ� ���� ������ ������ ����ϴ�.
	m_D3D->BeginScene(1.0f, 1.0f, 0.0f, 1.0f);

	// ī�޶��� ��ġ�� ���� �� ����� �����Ѵ�.
	m_Camera->Render();

	// ī�޶� �� d3d ��ü���� ����, �� �� ���� ����� �����´�.
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// �� ���ؽ��� �ε��� ���۸� �׷��� ������ ���ο� ��ġ�Ͽ� ������� �غ��Ѵ�.
	m_Model->Render(m_D3D->GetDeviceContext());

	// �ؽ�ó ���̴��� ����Ͽ� ���� ������ �Ѵ�.
	if (!m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Model->GetTexture()))
	{
		return false;
	}


	// ���� ���̴��� ����Ͽ� ���� ������ �Ѵ�.
	/*if (!m_ColorShader->Render(m_D3D->GetDeviceContext(),
		m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix))
	{
		return false;
	}*/

	// ���ۿ� �׷��� ���� ȭ�鿡 ǥ���մϴ�.
	m_D3D->EndScene();

	return true;
}
