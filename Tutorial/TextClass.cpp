#include "stdafx.h"
#include "TextClass.h"

#include "FontClass.h"
#include "FontShaderClass.h"

TextClass::TextClass()
{
}

TextClass::TextClass(const TextClass& other)
{
}

TextClass::~TextClass()
{
}

bool TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int screenWidth, int screenHeight, XMMATRIX baseViewMatrix)
{

	// ȭ�� �ʺ� ���� �� ����.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// baseViewMatrix ����.
	m_baseViewMatrix = baseViewMatrix;

	// Font ��ü ����
	m_Font = new FontClass;
	if (!m_Font)
	{
		return false;
	}

	// Font ��ü �ʱ�ȭ
	if (!(m_Font->Initialize(device, "./fontdata.txt", L"./Textures/font.dds")))
	{
		MessageBox(hwnd, L"Could not initialize the font object", L"Error", MB_OK);
		return false;
	}

	// ��Ʈ ���̴� ��ü ����
	m_FontShader = new FontShaderClass;
	if (!m_FontShader)
	{
		return false;
	}

	// ��Ʈ���̴� ��ü �ʱ�ȭ
	if (!(m_FontShader->Initialize(device, hwnd)))
	{
		MessageBox(hwnd, L"Could not initialize the font shader object", L"Error", MB_OK);
		return false;
	}

	// ù��° ���� �ʱ�ȭ
	if (!(InitializeSentence(&m_sentence1, 16, device)))
	{
		return false;
	}

	// ���� Sentence Vertex Buffer�� �������� ������Ʈ �Ѵ�.
	if (!(UpdateSentence(m_sentence1, "Hello", 100, 100, 1.0f, 1.0f, 1.0f, deviceContext)))
	{
		return false;
	}

	// �ι�° ���� �ʱ�ȭ
	if (!(InitializeSentence(&m_sentence2, 16, device)))
	{
		return false;
	}

	// ���� Sentence Vertex Buffer�� �������� ������Ʈ �Ѵ�.
	if (!(UpdateSentence(m_sentence2, "Goodbye", 100, 200, 1.0f, 1.0f, 0.0f, deviceContext)))
	{
		return false;
	}

	return true;
}

void TextClass::Shutdown()
{
	// ���� ��ü ����
	ReleaseSentence(&m_sentence1);
	ReleaseSentence(&m_sentence2);

	// ��Ʈ���̴� ��ü ����
	if (m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = nullptr;
	}

	// ��Ʈ ��ü ����
	if (m_Font)
	{
		m_Font->Shutdown();
		delete m_Font;
		m_Font = nullptr;
	}

}

bool TextClass::Render(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX orthoMatrix)
{
	bool result;

	// ù��° ���� �׸���
	result = RenderSentence(deviceContext, m_sentence1, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	// �ι�° ���� �׸���
	result = RenderSentence(deviceContext, m_sentence2, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	return true;
}

bool TextClass::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
{
	// SentenceType ��ü ����
	*sentence = new SentenceType;
	if (!*sentence)
	{
		return false;
	}

	// ���ٽ� ���۸� null�� �ʱ�ȭ
	(*sentence)->vertexBuffer = nullptr;
	(*sentence)->indexBuffer = nullptr;

	// �ִ� ���� �ʱ�ȭ
	(*sentence)->maxLength = maxLength;

	// ������ �ʱ�ȭ
	(*sentence)->vertexCount = 6 * maxLength; // �ѱ��ڿ� 6���� �ʿ�(�ﰢ�� 2��)

	// �ε����� �� �ʱ�ȭ
	(*sentence)->indexCount = (*sentence)->vertexCount;

	// ���ؽ� �迭 ����
	VertexType* vertices = new VertexType[(*sentence)->vertexCount];
	if (!vertices)
	{
		return false;
	}

	// �ε��� �迭 ����
	unsigned long* indices = new unsigned long[(*sentence)->indexCount];
	if (!indices)
	{
		return false;
	}

	// ���ؽ� ��� 0���� �ʱ�ȭ
	memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));

	// �ε��� ��� �ʱ�ȭ
	for (int i = 0; i < (*sentence)->indexCount; i++)
	{
		indices[i] = i;
	}

	// ���� ���� ���� ��ũ���� ����
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Subresource ����ü�� ���ؽ� ������ �����͸� �Ҵ�.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// ���ؽ� ���� ����
	bool result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// ���� �ε��� ������ ��ũ���� �ۼ�
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Subresource ����ü�� �ε��� ����Ÿ ������ �Ҵ�
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// �ε��� ���� ����
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// ���̻� ������� �ʴ� ���ؽ�, �ε��� �迭 �Ҵ� ����
	delete[] vertices;
	vertices = nullptr;
	delete[] indices;
	indices = nullptr;

	return true;
}

bool TextClass::UpdateSentence(SentenceType* sentence, const char* text, int positionX, int positionY, float red, float green, float blue , ID3D11DeviceContext* deviceContext)
{
	// ������ ���� ����
	sentence->red = red;
	sentence->green = green;
	sentence->blue = blue;

	// ������ ���� ��������
	auto numLetters = static_cast<int>(strlen(text));

	// ������ �����÷ο� Ȯ��
	if (numLetters > sentence->maxLength)
	{
		return false;
	}

	// ���ؽ� ��� ����
	auto vertices = new VertexType[sentence->vertexCount];
	if (!vertices)
	{
		return false;
	}

	// ���ؽ� ��� 0���� �ʱ�ȭ
	memset(vertices, 0, sizeof(VertexType) * sentence->vertexCount);

	// X�� Y�� �ȼ���ġ�� ����Ѵ�.
	auto drawX = static_cast<float>(((m_screenWidth / 2) * -1) + positionX);
	auto drawY = static_cast<float>((m_screenHeight / 2) - positionY);

	// ���� �ؽ�Ʈ�� ���� �׸��� ��ġ�� ����Ͽ� ���ؽ� ��̸� �����ϱ����� ��Ʈ Ŭ������ ����Ѵ�.
	m_Font->BuildVertexArray(reinterpret_cast<void*>(vertices), text, drawX, drawY);

	// ���ؽ� ���۸� ���� ���� ��ٴ�.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	bool result = deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// ���ؽ� ������ �����͸� ����Ű�� �����͸� �����´�.
	auto verticesPtr = reinterpret_cast<VertexType*>(mappedResource.pData);

	// ����Ÿ�� ���ؽ� ���۷� �����Ѵ�.
	memcpy(verticesPtr, reinterpret_cast<void*>(vertices), sizeof(VertexType) * sentence->vertexCount);

	// ���ؽ� ������ ������� �Ѵ�.
	deviceContext->Unmap(sentence->vertexBuffer, 0);

	// ���̻� ������� �ʴ� ���ؽ� ��� �Ҵ�����
	delete[] vertices;
	vertices = nullptr;

	return true;
}

void TextClass::ReleaseSentence(SentenceType** sentence)
{
	if (*sentence)
	{
		// ���� ���ؽ� ���� ����
		if ((*sentence)->vertexBuffer)
		{
			(*sentence)->vertexBuffer->Release();
			(*sentence)->vertexBuffer = nullptr;
		}

		// ���� �ε��� ���� ����
		if (((*sentence)->indexBuffer))
		{
			(*sentence)->indexBuffer->Release();
			(*sentence)->indexBuffer = nullptr;
		}

		// sentence ����
		delete* sentence;
		*sentence = nullptr;
	}
}

bool TextClass::RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, XMMATRIX worldMatrix, XMMATRIX orthoMatrix)
{
	// ���ؽ� ������ ��Ʈ���̵�� �������� ����
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// �����ɼ� �ֵ��� ��ǲ ��������� ���ؽ� ���۰� Ȱ��ȭ �ǵ��� �����Ѵ�.
	deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

	// �ε��� ���� Ȱ��ȭ
	deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// primitive Ÿ�� ����, ���⼭�� �ﰢ��
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// �ȼ� ���� ���͸� ��ǲ�� ���� �������� �����.
	auto pixelColor = XMFLOAT4(sentence->red, sentence->green, sentence->blue, 1.0f);

	// ��Ʈ���̴��� �̿��Ͽ� �ؽ�Ʈ�� �����Ѵ�.
	bool result = m_FontShader->Render(deviceContext, sentence->indexCount, worldMatrix, m_baseViewMatrix, orthoMatrix,
		m_Font->GetTexture(), pixelColor);
	if (!result)
	{
		return false;
	}
	return true;
}
