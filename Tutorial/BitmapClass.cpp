#include "stdafx.h"
#include "BitmapClass.h"

#include "TextureClass.h"

BitmapClass::BitmapClass()
{
}

BitmapClass::BitmapClass(const BitmapClass& other)
{
}

BitmapClass::~BitmapClass()
{
}

bool BitmapClass::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, const WCHAR* textureFilename, int bitmapWidth, int bitmapHeight)
{
	// ȭ�� ũ�⸦ ��������� ����
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// �������� ��Ʈ���� �ȼ��� ũ�⸦ ����
	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	// ���� ������ ��ġ�� ������ �ʱ�ȭ �Ѵ�.
	m_previousPosX = -1;
	m_previousPosY = -1;

	// ���� �� �ε��� ���۸� �ʱ�ȭ �Ѵ�.
	if (!InitializeBuffers(device))
	{
		return false;
	}

	// �� ���� �ؽ�ó�� �ε��Ѵ�.
	return LoadTexture(device, textureFilename);

}

void BitmapClass::Shutdown()
{
	// �� �ؽ��ĸ� ��ȯ�Ѵ�.
	ReleaseTexture();

	// ���ؽ� �� �ε��� ���۸� �����Ѵ�.
	ShutdownBuffers();
}

bool BitmapClass::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	// ȭ���� �ٸ� ��ġ�� �������ϱ� ���� ���� ���� ���۸� �ٽ� �����Ѥ�.
	if (!UpdateBuffers(deviceContext, positionX, positionY))
	{
		return false;
	}

	// �׸��⸦ �غ��ϱ� ���� �׷��� ������ ���ο� �������� �ε��� ���۸� ���´�.
	RenderBuffers(deviceContext);

	return true;
}

int BitmapClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* BitmapClass::GetTexture()
{
	return m_texture->GetTexture();
}

bool BitmapClass::InitializeBuffers(ID3D11Device* device)
{
	// ���� �迭�� ���� ���� �ε��� �迭�� �ε��� ���� �����Ѵ�.
	m_indexCount = m_vertexCount = 6;

	// ���� �迭�� �����.
	VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}


	// ���� �迭�� 0���� �ʱ�ȭ �Ѵ�.
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// �ε��� �迭�� �����.
	unsigned long* indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// �����ͷ� �ε��� �迭�� �ε��Ѵ�.
	for (int i = 0; i < m_vertexCount; i++)
	{
		indices[i] = i;
	}


	// ���� ���� ������ ����ü�� �����Ѵ�.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource ������ ���� �����Ϳ� ���� �����͸� �����Ѵ�.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// ���� ���� ���۸� ����ϴ�.
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
	{
		return false;
	}

	// �ε��� ���� description ����
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// �ε��� �����͸� ����Ű�� ���� ���ҽ� ����ü �ۼ�
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// �ε��� ���۸� �����մϴ�.
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
	{
		return false;
	}

	// �������� �ε��� ���� ����
	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	return true;
}

void BitmapClass::ShutdownBuffers()
{
	// �ε��� ���� ����
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}

	// ���� ���۸� �����մϴ�.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_indexBuffer = nullptr;
	}
}

bool BitmapClass::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	float left, right, top, bottom;
	VertexType* vertices;
	VertexType* verticesPtr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;


	// �� ��Ʈ���� ������ �� ��ġ�� ������� ���� ��� ���� ���۸� ������Ʈ ���� �ʴ´�.
	if ((positionX == m_previousPosX) && (positionY == m_previousPosY))
	{
		return true;
	}

	// ����� ��� ������ �Ǵ� ��ġ�� ������Ʈ �Ѵ�.
	m_previousPosX = positionX;
	m_previousPosY = positionY;

	// ��Ʈ �� ������ ȭ�� ��ǥ�� ����Ѵ�.
	left = static_cast<float>((m_screenWidth / 2) * -1) + static_cast<float>(positionX);

	// ��Ʈ�� �������� ȭ�� ��ǥ�� ����Ѵ�.
	right = left + static_cast<float>(m_bitmapWidth);

	// ��Ʈ�� ����� ȭ�� ��ǥ�� ����Ѵ�.
	top = static_cast<float>(m_screenHeight / 2) - static_cast<float>(positionY);

	// ��Ʈ �� �Ʒ����� ȭ�� ��ǥ�� ����Ѵ�.
	bottom = top - static_cast<float>(m_bitmapHeight);


	// ���� �迭�� �����.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// ���� �迭�� �����͸� �ε��Ѵ�.
	// ù ��° �ﰢ��
	vertices[0].position = XMFLOAT3(left, top, 0.0f);
	vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[1].position = XMFLOAT3(right, bottom, 0.0f);
	vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

	vertices[2].position = XMFLOAT3(left, bottom, 0.0f);
	vertices[2].texture = XMFLOAT2(0.0f, 1.0f);


	// �ι�° �ﰢ��
	vertices[3].position = XMFLOAT3(left, top, 0.0f);
	vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

	vertices[4].position = XMFLOAT3(right, top, 0.0f);
	vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

	vertices[5].position = XMFLOAT3(right, bottom, 0.0f);
	vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

	// ���ؽ� ���۸� �� �� �ֵ��� ��ٴ�.
	if (FAILED(deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// ���� ������ �����͸� ����Ű�� �����͸� ��´�.
	verticesPtr = reinterpret_cast<VertexType*>(mappedResource.pData);

	// �����͸� ���� ���ۿ� �����Ѵ�.
	memcpy(verticesPtr, vertices, (sizeof(VertexType) * m_vertexCount));
	
	// ���� ������ ����� �����Ѵ�.
	deviceContext->Unmap(m_vertexBuffer, 0);

	// ���̻� �ʿ����� ���� ������ �迭 ����
	delete[] vertices;
	vertices = 0;

	return true;
}

void BitmapClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// ���� ������ ������ �������� �����Ѵ�.
	stride = sizeof(VertexType);
	offset = 0;

	// input assembler�� ���� ���۸� Ȱ��ȭ�Ͽ� �׷��� �� �ְ� �Ѵ�.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// input Assembler�� �ε��� ���۸� Ȱ��ȭ�� �׷��� �� �ְ��Ѵ�.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// ���� ���۷� �׸� �⺻���� �����Ѵ�.
	// ���⼭�� �ﰢ���̴�.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

bool BitmapClass::LoadTexture(ID3D11Device* device, const WCHAR* filename)
{
	// �ؽ��� ������Ʈ�� �����Ѵ�.
	if (!m_texture)
	{
		return false;
	}

	// �ؽ�ó ������Ʈ�� �ʱ�ȭ�Ѵ�.
	return m_texture->Initialize(device, filename);
}

void BitmapClass::ReleaseTexture()
{
	// �ؽ��� ������Ʈ�� ������ �Ѵ�.
	if (m_texture)
	{
		m_texture->Shutdown();
		delete m_texture;
		m_texture = nullptr;
	}
	
}
