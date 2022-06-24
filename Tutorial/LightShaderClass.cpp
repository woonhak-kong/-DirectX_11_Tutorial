#include "stdafx.h"
#include "LightShaderClass.h"

LightShaderClass::LightShaderClass()
{
}

LightShaderClass::LightShaderClass(const LightShaderClass& other)
{
}

LightShaderClass::~LightShaderClass()
{
}

bool LightShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	// ���� �� �ȼ� ���̴� �ʱ�ȭ
	return InitializeShader(device, hwnd, L"./light.vs", L"./light.ps");
}

void LightShaderClass::Shutdown()
{
	// ���ؽ� �� �ȼ� ���̴��� ���õ� ��ü ����
	ShutdownShader();
}

bool LightShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture,
	XMFLOAT3 lightDirection, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor, XMFLOAT3 cameraPosition, XMFLOAT4 specularColor, float specularPower)
{
	// �������� ����� ���̴� �Ű� ������ ����
	if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, ambientColor, diffuseColor,
		cameraPosition, specularColor, specularPower))
	{
		return false;
	}

	// ������ ���۸� ���̴��� ������ �Ѵ�.
	RenderShader(deviceContext, indexCount);
	return true;
}

bool LightShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, const WCHAR* vsFilename, const WCHAR* psFilename)
{
	ID3D10Blob* errorMessage = nullptr;

	// ���ؽ� ���̴� �ڵ带 ������ �Ѵ�.
	ID3D10Blob* vertexShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(vsFilename, nullptr, nullptr, "LightVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMessage)))
	{
		// ���̴� ������ ���н� �����޽��� ���.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// ������ ������ �ƴ϶�� ���̴� ������ ã�� �� ���� ����̴�.
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing VertexShader Shader File", MB_OK);
		}

		return false;
	}

	// �ȼ� ���̴� �ڵ带 ������ �Ѵ�.
	ID3D10Blob* pixelShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(psFilename, nullptr, nullptr, "LightPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
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
			MessageBox(hwnd, psFilename, L"Missing PixelShade Shader File", MB_OK);
		}

		return false;
	}

	// ���۷κ��� ���� ���̴��� �����Ѵ�.
	if (FAILED(device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
		nullptr, &m_vertexShader)))
	{
		return false;
	}

	// ���ۿ��� �ȼ� ���̴��� �����Ѵ�.
	if (FAILED(device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(),
		nullptr, &m_pixelShader)))
	{
		return false;
	}

	// ���� �Է� ���̾ƿ� ����ü�� �����մϴ�.
	// �� ������ ModelClass�� ���̴��� VertexType ������ ��ġ�ؾ� �Ѵ�.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	// ���̾ƿ��� ��� ���� �����ɴϴ�.
	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// ���� �Է� ���̾ƿ��� ����ϴ�.
	if (FAILED(device->CreateInputLayout(polygonLayout, numElements,
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
	if (FAILED(device->CreateBuffer(&matrixBufferDesc, nullptr, &m_matrixBuffer)))
	{
		return false;
	}

	// �ؽ�ó ���÷� ���� ����ü�� ���� �� �����Ѵ�.
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// �ؽ�ó ���÷� ���¸� ����ϴ�.
	if (FAILED(device->CreateSamplerState(&samplerDesc, &m_sampleState)))
	{
		return false;
	}


	// ���ؽ� ���̴����ִ� ī�޶� ���� ��� ������ ������ ����
	D3D11_BUFFER_DESC cameraBufferDesc;
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	// �� Ŭ���� ������ ���� ���̴� ��� ���ۿ� ������ �� �� �ֵ��� ī�޶� ��� ���� �����͸� �����.
	if (FAILED(device->CreateBuffer(&cameraBufferDesc, nullptr, &m_cameraBuffer)))
	{
		return false;
	}


	// �ȼ� ���̴����ִ� ���� ���� ��� ������ ������ �����մϴ�.
	// D3D11_BIND_CONSTANT_BUFFER�� ����ϸ� Bytewidth�� �׻� 16��� ����ϸ� �׷��� ������, CreateBuffer�� �����Ѵ�.
	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// �� Ŭ���� ������ ���� ���̴� ��� ���ۿ� ������ �� �� �ֵ��� ��� ���� �����͸� ����ϴ�.
	if (FAILED(device->CreateBuffer(&lightBufferDesc, nullptr, &m_lightBuffer)))
	{
		return false;
	}

	return true;

}

void LightShaderClass::ShutdownShader()
{
	// ī�޶� ��� ���� ����
	if (m_cameraBuffer)
	{
		m_cameraBuffer->Release();
		m_cameraBuffer = nullptr;
	}

	// light constant ���� ����
	if (m_lightBuffer)
	{
		m_lightBuffer->Release();
		m_lightBuffer = nullptr;
	}

	// ���÷� ���� ����
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = nullptr;
	}

	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = nullptr;
	}

	if (m_layout)
	{
		m_layout->Release();
		m_layout = nullptr;
	}

	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = nullptr;
	}

	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = nullptr;
	}
}

void LightShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
{
	// ���� �޽����� ���â�� ǥ���Ѵ�.
	OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));

	// ���� �޽����� ��ȯ �Ѵ�.
	errorMessage->Release();
	errorMessage = nullptr;

	// ������ ������ ������ �˾� �޼����� �˷��ش�.
	MessageBox(hwnd, L"Error compiling shader.", shaderFilename, MB_OK);
}

bool LightShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix,
	ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirection, XMFLOAT4 ambientColor, XMFLOAT4 diffuseColor, XMFLOAT3 cameraPosition, XMFLOAT4 specularColor,
	float specularPower)
{
	// ����� transpose�Ͽ� ���̴����� ����� �� �ְ� �Ѵ�.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// ��� ������ ������ �� �� �ֵ��� ��ٴ�.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
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

	// �� �� �ֵ��� ī�޶� ��� ���۸� ��޴ϴ�.
	if (FAILED(deviceContext->Map(m_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0,&mappedResource)))
	{
		return false;
	}

	CameraBufferType* dataPtr3 = reinterpret_cast<CameraBufferType*>(mappedResource.pData);

	// ī�޶� ��ġ�� ��� ���ۿ� �����Ѵ�.
	dataPtr3->cameraPosition = cameraPosition;
	dataPtr3->padding = 0.0f;

	// ī�޶� ��� ���۸� ��� �����Ѵ�.
	deviceContext->Unmap(m_cameraBuffer, 0);

	// ���ؽ� ���̴����� ī�޶� ��� ������ ��ġ�� �����Ѵ�.
	bufferNumber = 1;

	// ���� ������Ʈ �� ������ ���ؽ� ���̴����� ī�޶� ��� ���۸� �����Ѵ�.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_cameraBuffer);

	// �ȼ� ���̴����� ���̴� �ؽ�ó ���ҽ��� �����Ѵ�.
	deviceContext->PSSetShaderResources(0, 1, &texture);

	// light constant buffer�� ��� �� �ֵ��� ����Ѵ�.
	if(FAILED(deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// ��� ������ �����Ϳ� ���� �����͸� �����´�.
	LightBufferType* dataPtr2 = reinterpret_cast<LightBufferType*>(mappedResource.pData);

	// �� �������� ������ۿ� �ִ´�.
	dataPtr2->ambientColor = ambientColor;
	dataPtr2->diffuseColor = diffuseColor;
	dataPtr2->lightDirection = lightDirection;
	dataPtr2->specularColor = specularColor;
	dataPtr2->specularPower = specularPower;

	// ��� ������ ����� �����Ѵ�.
	deviceContext->Unmap(m_lightBuffer, 0);

	// �ȼ� ���̴����� ���� ��� ������ ��ġ�� �����Ѵ�.
	bufferNumber = 0;

	// ���������� ������Ʈ �� ������ �ȼ� ���̴����� ���� ��� ���۸� �����Ѵ�.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

	return true;
}

void LightShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// ���� �Է� ���̾ƿ��� �����Ѵ�.
	deviceContext->IASetInputLayout(m_layout);

	// �ﰢ���� �׸� ���� ���̴��� �ȼ� ���̴��� �����Ѵ�.
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader, nullptr, 0);

	// �ȼ� ���̴����� ���÷� ���¸� �����Ѥ�.��
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// �ﰢ���� �׸���.
	deviceContext->DrawIndexed(indexCount, 0, 0);
}
