#include "stdafx.h"
#include "ColorshaderClass.h"
#include <fstream>

ColorShaderClass::ColorShaderClass()
{
}

ColorShaderClass::ColorShaderClass(const ColorShaderClass& other)
{
}

ColorShaderClass::~ColorShaderClass()
{
}

bool ColorShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	// ���� ���̴��� �ȼ� ���̴��� �ʱ�ȭ �Ѵ�.
	return InitializeShader(device, hwnd, L"./color.vs", L"./color.ps");
}

void ColorShaderClass::Shutdown()
{
	// �������̴�, �ȼ� ���̴� �� �׿� ���õ� �͵��� ��ȯ.
	ShutdownShader();
}

bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix,
	XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	// �������� ����� ���̴��� ���ڸ� �Է��Ѵ�.
	if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix))
	{
		return false;
	}

	//���̴��� �̿��Ͽ� �غ�� ���۸� �׸���.
	RenderShader(deviceContext, indexCount);
	return true;
}

bool ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* vsFilename, const WCHAR* psFilename)
{
	ID3D10Blob* errorMessage = nullptr;

	// ���ؽ� ���̴� �ڵ带 ������ �Ѵ�.
	ID3D10Blob* vertexShaderBuffer = nullptr;
	if(FAILED(D3DCompileFromFile(vsFilename, nullptr, nullptr, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMessage)))
	{
		// ���̴� ������ ���н� �����޽��� ���.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// ������ ������ �ƴ϶�� ���̴� ������ ã�� �� ���� ����̴�.
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// �ȼ� ���̴� �ڵ带 ������ �Ѵ�.
	ID3D10Blob* pixelShaderBuffer = nullptr;
	if(FAILED(D3DCompileFromFile(psFilename, nullptr, nullptr, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
	&pixelShaderBuffer, &errorMessage)))
	{
		// ���̴� ������ ���н� �����޽��� ���.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// ������ ������ �ƴ϶�� ���̴� ������ ã�� �� ���� ����̴�.
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// ���۷κ��� ���� ���̴��� �����Ѵ�.
	if(FAILED(device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
		nullptr, &m_vertexShader)))
	{
		return false;
	}

	// ���ۿ��� �ȼ� ���̴��� �����Ѵ�.
	if(FAILED(device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(),
		nullptr, &m_pixelShader)))
	{
		return false;
	}
	
	// ���� �Է� ���̾ƿ� ����ü�� �����մϴ�.
	// �� ������ ModelClass�� ���̴��� VertexType ������ ��ġ�ؾ� �Ѵ�.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// ���̾ƿ��� ��� ���� �����ɴϴ�.
	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// ���� �Է� ���̾ƿ��� ����ϴ�.
	if(FAILED(device->CreateInputLayout(polygonLayout, numElements,
		vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout)))
	{
		return false;
	}

	// �� �̻� ������ �ʴ� ���� ���̴� ���ۿ� �ȼ� ���̴� ���۸� �����Ѵ�.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = nullptr;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = nullptr;

	// ���� ���̴��� �ִ� ��� ��� ������ ����ü�� �ۼ��Ѵ�.
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// ��� ���� �����͸� ����� �� Ŭ�������� ���� ���̴� ��� ���ۿ� ������ �� �ְ� �Ѵ�.
	if(FAILED(device->CreateBuffer(&matrixBufferDesc, nullptr, &m_matrixBuffer)))
	{
		return false;
	}

	return true;
}

void ColorShaderClass::ShutdownShader()
{
	if(m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = nullptr;
	}

	if(m_layout)
	{
		m_layout->Release();
		m_layout = nullptr;
	}

	if(m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = nullptr;
	}

	if(m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = nullptr;
	}


}

void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
{
	// ���� �޽����� ���â�� ǥ���Ѵ�.
	OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));

	//////////////////////////////////
	// ���Ϸ� �������.

	//char* compileErrors;
	//unsigned long bufferSize, i;
	//std::ofstream fout;


	//// ���� �޼����� ��� �ִ� ���ڿ� ������ �����͸� �����ɴϴ�.
	//compileErrors = (char*)(errorMessage->GetBufferPointer());

	//// �޼����� ���̸� �����ɴϴ�.
	//bufferSize = errorMessage->GetBufferSize();

	//// ������ ���� �ȿ� �޼����� ����մϴ�.
	//fout.open("shader-error.txt");

	//// ���� �޼����� ���ϴ�.
	//for (i = 0; i < bufferSize; i++)
	//{
	//	fout << compileErrors[i];
	//}

	//// ������ �ݽ��ϴ�.
	//fout.close();

	/////////////////////////////////////
	/////////////////////////////////////


	// ���� �޽����� ��ȯ �Ѵ�.
	errorMessage->Release();
	errorMessage = nullptr;

	// ������ ������ ������ �˾� �޼����� �˷��ش�.
	MessageBox(hwnd, L"Error compiling shader.", shaderFilename, MB_OK);
}

bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix,
	XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
	// ����� transpose�Ͽ� ���̴����� ����� �� �ְ� �Ѵ�.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// ��� ������ ������ �� �� �ֵ��� ��ٴ�.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if(FAILED(deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// ��� ������ �����Ϳ� ���� �����͸� �����´�.
	MatrixBufferType* dataPtr = reinterpret_cast<MatrixBufferType*>(mappedResource.pData);

	// ��� ���ۿ� ����� �����Ѵ�.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// ��� ������ ����� Ǭ��.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// ���� ���̴������� ��� ������ ��ġ�� �����Ѵ�.
	unsigned bufferNumber = 0;

	// ���������� ���� ���̴��� ��� ���۸� �ٲ� ������ �ٲ۴�.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	return true;
	
}

void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// ���� �Է� ���̾ƿ��� �����Ѵ�.
	deviceContext->IASetInputLayout(m_layout);

	// �ﰢ���� �׸� ���� ���̴��� �ȼ� ���̴��� �����Ѵ�.
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader, nullptr, 0);

	// �ﰢ���� �׸���.
	deviceContext->DrawIndexed(indexCount, 0, 0);
	
}
