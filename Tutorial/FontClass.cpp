#include "stdafx.h"
#include "FontClass.h"

#include <fstream>

#include "TextureClass.h"

FontClass::FontClass()
{
}

FontClass::FontClass(const FontClass& other)
{
}

FontClass::~FontClass()
{
}

bool FontClass::Initialize(ID3D11Device* device, const char* fontFilename, const WCHAR* textureFilename)
{
	// ��Ʈ ������ �ε�
	if (!LoadFontData(fontFilename))
	{
		return false;
	}

	// ��Ʈ ���ڵ��� ������ �ִ� �ؽ��� �ε�
	return LoadTexture(device, textureFilename);
}

void FontClass::Shutdown()
{
	// ��Ʈ �ؽ��� ����
	ReleaseTexture();

	// ��Ʈ ����Ÿ ����
	ReleaseFontData();
}

ID3D11ShaderResourceView* FontClass::GetTexture()
{
	return m_Texture->GetTexture();
}

void FontClass::BuildVertexArray(void* vertices, const char* sentence, float drawX, float drawY)
{
	VertexType* vertexPtr;

	// vertices �� VertexType ����ü�� ���� ����
	vertexPtr = reinterpret_cast<VertexType*>(vertices);

	// ������ ���ڼ��� �����´�.
	int numLetters = static_cast<int>(strlen(sentence));

	// vertex �迭�� �ε��� �ʱ�ȭ
	int index = 0;

	// �� ���ڸ� quad�� �׸���.
	for (int i = 0; i < numLetters; i++)
	{
		// ������ ��ȣ
		int letter = static_cast<int>(sentence[i]) - 32;

		// ���� ���ڰ� ���� ����(�����̽�)��� ���� 3�ȼ� �̵���Ų��.
		if (letter == 0)
		{
			drawX = drawX + 3.0f;
		}
		else
		{
			// �������� ù��° �ﰢ��
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f); // Top left
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f); // UV
			index++;

			vertexPtr[index].position = XMFLOAT3((drawX + m_Font[letter].size), (drawY - 16), 0.0f); // Bottom right
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f); // UV
			index++;

			vertexPtr[index].position = XMFLOAT3(drawX, drawY - 16, 0.0f); // Bottom left
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 1.0f); // UV
			index++;

			// �������� �ι�° �ﰢ��
			vertexPtr[index].position = XMFLOAT3(drawX, drawY, 0.0f); // Top left
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].left, 0.0f); // UV
			index++;

			vertexPtr[index].position = XMFLOAT3((drawX + m_Font[letter].size), drawY, 0.0f); // Top right
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 0.0f); // UV
			index++;

			vertexPtr[index].position = XMFLOAT3(drawX + m_Font[letter].size, drawY - 16, 0.0f); // Bottom right
			vertexPtr[index].texture = XMFLOAT2(m_Font[letter].right, 1.0f); // UV
			index++;

			// ���� ������ x ��ġ�� ���Ͽ� �� ������ ������� 1 �ȼ� �� �̵���Ų��.
			drawX = drawX + m_Font[letter].size + 1.0f;
		}
	}

}

bool FontClass::LoadFontData(const char* filename)
{
	std::ifstream fin;
	int i;
	char temp;

	// ��Ʈ���� �Ҵ�
	m_Font = new FontType[95];
	if (!m_Font)
	{
		return false;
	}


	// ��Ʈ����Ÿ ����
	fin.open(filename);
	if (fin.fail())
	{
		return false;
	}

	// �ؽ�Ʈ�� ���� 95���� ���� ������ �����Ѵ�.
	for (i = 0; i < 95; i++)
	{
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}
		fin.get(temp);
		while (temp != ' ')
		{
			fin.get(temp);
		}

		fin >> m_Font[i].left;
		fin >> m_Font[i].right;
		fin >> m_Font[i].size;
	}

	// ������ �ݴ´�.
	fin.close();

	return true;
}

void FontClass::ReleaseFontData()
{
	// ��Ʈ ����Ÿ �迭 ����
	if (m_Font)
	{
		delete[] m_Font;
		m_Font = nullptr;
	}
}

bool FontClass::LoadTexture(ID3D11Device* device, const WCHAR* filename)
{
	// �ؽ��� ��ü ����
	m_Texture = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}

	// �ؽ��� ��ü �ʱ�ȭ
	return m_Texture->Initialize(device, filename);
}

void FontClass::ReleaseTexture()
{
	// �ؽ��� ��ü ����
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = nullptr;
	}
}
