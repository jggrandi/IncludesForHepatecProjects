#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

#include <GL/gl.h>

#include <ErrorManager.h>
#include <MyGLH.h>
#include <MyMath.h>

#include "MyTerrain.h"

using namespace std;

/**
*/
CMyTerrain::CMyTerrain(void)
{
	Create();
}

/**
*/
CMyTerrain::~CMyTerrain(void)
{
}

/**
*/
bool CMyTerrain::Initialize(void)
{
	if (!InitializeMesh())
	{
		MarkError();

		return false;
	}

	if (!InitializeTexture())
	{
		MarkError();

		return false;
	}

	return true;
}

/**
*/
void CMyTerrain::LoadDrawState(void)
{
}

/**
*/
void CMyTerrain::UnloadDrawState(void)
{
}

/**
*/
void CMyTerrain::Draw(void) 
{
	LoadDrawState();

	m_texture->Bind(0);

	m_mesh->Draw();

	CTexture::DisableTextureUnit(0);

	UnloadDrawState();
}

/**
*/
boost::shared_ptr<CVertexPool> CMyTerrain::GetMesh(void)
{
	return m_mesh;
}

/**
*/
boost::shared_ptr<CTexture> CMyTerrain::GetTexture(void)
{
	return m_texture;
}

/**
*/
bool CMyTerrain::IsValid(void) 
{
	if (m_texture.get())
		if (m_texture->IsValid())
			if (m_mesh.get())
				if (m_mesh->IsValid())
					return true;

	return false;
}

/**
*/
bool CMyTerrain::InitializeMesh(void)
{
	m_mesh.reset(new CVertexPool);

	if (!m_mesh.get())
	{
		MarkError();

		return false;
	}

	m_mesh->AddTexCoord(0.0f, 0.0f);
	m_mesh->AddVertex(-4.0f, 0.0f, -4.0f);
	m_mesh->AddTexCoord(1.0f, 0.0f);
	m_mesh->AddVertex(4.0f, 0.0f, -4.0f);
	m_mesh->AddTexCoord(1.0f, 1.0f);
	m_mesh->AddVertex(4.0f, 0.0f, 4.0f);
	m_mesh->AddTexCoord(0.0f, 1.0f);
	m_mesh->AddVertex(-4.0f, 0.0f, 4.0f);
	
	m_mesh->AddQuad(0, 1, 2, 3);

	if (!m_mesh->IsValid())
	{
		MarkError();

		return false;
	}

	return true;
}

/**
*/
bool CMyTerrain::InitializeTexture(void)
{
	static unsigned char textureData[2*2*3] = {
		128, 255, 128, 128, 255, 128, 
		128, 255, 128, 128, 255, 128
	};

	m_texture.reset(new CTexture);

	if (!m_texture.get())
	{
		MarkError();

		return false;
	}

	if (!m_texture->Initialize(CTexture::TEXTURE_2D, CTexture::RGB))
	{
		MarkError();

		return false;
	}

	if (!m_texture->SetImage(2, 2, 0, CTexture::RGB, CTexture::UNSIGNED_BYTE, textureData))
	{
		MarkError();

		return false;
	}

	m_texture->SetParameter(CTexture::MAG_FILTER, CTexture::LINEAR);
	m_texture->SetParameter(CTexture::MIN_FILTER, CTexture::LINEAR);
	m_texture->SetParameter(CTexture::WRAP_S, CTexture::REPEAT);
	m_texture->SetParameter(CTexture::WRAP_T, CTexture::REPEAT);

	if (!MyGLH::IsRenderingContextOk())
	{
		MarkError();

		return false;
	}

	return true;
}

/**
*/
void CMyTerrain::Create(void)
{
	m_mesh.reset();

	m_texture.reset();
}

