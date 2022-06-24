#include "stdafx.h"
#include "ModelClass.h"

#include "TextureClass.h"

ModelClass::ModelClass()
{
}

ModelClass::ModelClass(const ModelClass& other)
{
}

ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D11Device* device, const WCHAR* textureFilename)
{
	// ���� �� �ε��� ���۸� �ʱ�ȭ�մϴ�.
	if (!InitializeBuffers(device))
	{
		return false;
	}

	// �� ���� �ؽ�ó�� �ε��Ѵ�.
	return LoadTexture(device, textureFilename);
}

void ModelClass::Shutdown()
{
	// �� �ؽ�ó ��ȯ
	ReleaseTexture();

	//���ؽ� �� �ε��� ���� ����
	ShutdownBuffers();
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// �׸��⸦ �غ��ϱ� ���� �׷��� ������ ���ο� �������� �ε��� ���۸� ���´�.
	RenderBuffers(deviceContext);
}

int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_texture->GetTexture();
}

bool ModelClass::LoadTexture(ID3D11Device* device, const WCHAR* filename)
{
	// �ؽ���Ŭ���� ��ü ����
	m_texture = new TextureClass;


	if (!m_texture)
	{
		return false;
	}

	// ��ü �ʱ�ȭ
	return m_texture->Initialize(device, filename);
}

void ModelClass::ReleaseTexture()
{
	// �ؽ��� Ŭ���� ��ü ��ȯ
	if (m_texture)
	{
		m_texture->Shutdown();
		delete m_texture;
		m_texture = nullptr;
	}
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	// ���� �迭�� ���� ���� �����մϴ�.
	m_vertexCount = 4;

	// �ε��� �迭�� �ε��� ���� �����մϴ�.
	m_indexCount = 6;

	// ���� �迭�� �����.
	VertexType* vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// �ε��� �迭�� ����ϴ�.
	unsigned long* indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}

	// ���� �迭�� �����͸� �����մϴ�.

	// �ﰢ��
	//vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f); // bottom left.
	//vertices[0].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	//vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f); // Top middle
	//vertices[1].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	//vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f); // bottom right.
	//vertices[2].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);


	// �簢��
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f); // bottom left.
	vertices[0].texture = XMFLOAT2(0.0f, 1.0f);
	vertices[0].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	vertices[1].position = XMFLOAT3(-1.0f, 1.0f, 0.0f); // Top left
	vertices[1].texture = XMFLOAT2(0.0f, 0.0f);
	vertices[1].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	vertices[2].position = XMFLOAT3(1.0f, 1.0f, 0.0f); // top right.
	vertices[2].texture = XMFLOAT2(1.0f, 0.0f);
	vertices[2].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	vertices[3].position = XMFLOAT3(1.0f, -1.0f, 0.0f); // bottom right.
	vertices[3].texture = XMFLOAT2(1.0f, 1.0f);
	vertices[3].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// �ε��� �迭�� ���� �����Ѵ�.
	// �ﰢ��
	//indices[0] = 0; // Bottom left;
	//indices[1] = 1; // Top middle;
	//indices[2] = 2; // Bottom right;

	// �簢��
	indices[0] = 0; // Bottom left;
	indices[1] = 1; // Top left;
	indices[2] = 2; // Top right;
	indices[3] = 2; // Top right;
	indices[4] = 3; // Bottom right;
	indices[5] = 0; // Bottom left;


	// ���� ������ description �ۼ�.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// ���� �����͸� ����Ű�� ���� ���ҽ� ����ü�� �ۼ��Ѵ�.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// ���� ���� ���۸� ����ϴ�.
	if(FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
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
	if(FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
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

void ModelClass::ShutdownBuffers()
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

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
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
