#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

#include <GL/glew.h>

#ifdef _WIN32
#include <GL/wglew.h>
#else // !_WIN32
#include <GL/glxew.h>
#endif // _WIN32

#include <ErrorManager.h>
#include <MyGLH.H>

#include <Texture.h>

using namespace std;

vector<unsigned int> CTexture::m_textureUnitArray(16, 0);

/**
*/
CTexture::CTexture(void)
{
	Create();
}

/**
*/
CTexture::~CTexture(void)
{
	Destroy();
}

/**
*/
bool CTexture::Initialize(unsigned int target, int internalFormat)
{
#ifdef _WIN32
	// Check if OpenGL is up.
	if (!wglGetCurrentContext())
	{
		MarkError();

		return false;
	}
#endif // _WIN32

	// Check if GL_ARB_multitexture extension is supported.
	if (!glewIsExtensionSupported("GL_ARB_multitexture"))
	{
		MarkError();

		return false;
	}

	// Debug only: check if GL_ARB_multitexture extension is up.
	if (!glActiveTextureARB)
	{
		MarkError();

		return false;
	}

	glGenTextures(1, &m_name);

	m_target = target;

	switch (target) {
		case GL_TEXTURE_1D:
			m_binding = GL_TEXTURE_BINDING_1D;
			break;
		case GL_TEXTURE_2D:
		case GL_TEXTURE_DEPTH:
			m_binding = GL_TEXTURE_BINDING_2D;
			break;
		case GL_TEXTURE_RECTANGLE_ARB:
			m_binding = GL_TEXTURE_BINDING_RECTANGLE_ARB;
			break;
		case GL_TEXTURE_3D:
			m_binding = GL_TEXTURE_BINDING_3D;
			break;
		case GL_TEXTURE_CUBE_MAP:
			m_binding = GL_TEXTURE_BINDING_CUBE_MAP;
			break;
		default:
			MarkError();
			return false;
	}

	m_internalFormat = internalFormat;

	InitializeDefaultContext();

	if (!MyGLH::IsRenderingContextOk())
	{
		MarkError();
		
		return false;
	}

	return true;
}

/**
*/
bool CTexture::SetImage(int width, int height, int border, unsigned int format, unsigned int type, const void *pixels)
{
	GuardedBind();

	glTexImage2D(m_target, 0, m_internalFormat, width, height, border, format, type, pixels);

	GuardedUnbind();

	m_width = width;
	m_height = height;
	
	m_depth = 0;

	if (!MyGLH::IsRenderingContextOk())
	{
		MarkError();
		
		return false;
	}

	return true;
}

/**
*/
bool CTexture::SetImage(int width, int height, int depth, int border, unsigned int format, unsigned int type, const void *pixels)
{
	GuardedBind();

	glTexImage3D(m_target, 0, m_internalFormat, width, height, depth, border, format, type, pixels);

	GuardedUnbind();

	m_width = width;
	m_height = height;
	
	m_depth = depth;

	if (!MyGLH::IsRenderingContextOk())
	{
		MarkError();
		
		return false;
	}
	
	return true;
}

/**
*/
bool CTexture::SetSubImage(int xOffset, int yOffset, int width, int height, unsigned int format, unsigned int type, const void *pixels)
{
	GuardedBind();

	glTexSubImage2D(m_target, 0, xOffset, yOffset, width, height, format, type, pixels);

	GuardedUnbind();

	if (!MyGLH::IsRenderingContextOk())
	{
		MarkError();
		
		return false;
	}

	return true;
}

/**
*/
bool CTexture::SetSubImage(int xOffset, int yOffset, int zOffset, int width, int height, int depth, unsigned int format, unsigned int type, const void *pixels)
{
	GuardedBind();

	glTexSubImage3D(m_target, 0, xOffset, yOffset, zOffset, width, height, depth, format, type, pixels);

	GuardedUnbind();

	if (!MyGLH::IsRenderingContextOk())
	{
		MarkError();
		
		return false;
	}

	return true;
}

/**
*/
bool CTexture::BuildMipmaps(void)
{
	vector<unsigned char> textureData;

	if (m_width == 0)
	{
		MarkError();

		return false;
	}

	if (m_height == 0)
	{
		MarkError();

		return false;
	}

	if (m_depth != 0)
	{
		MarkError();

		return false;
	}

	textureData.assign(4*m_width*m_height, 0);

	GuardedBind();

	glGetTexImage(m_target, 0, GL_RGBA, GL_UNSIGNED_BYTE, &textureData[0]);

	if (!MyGLH::IsRenderingContextOk())
	{
		MarkError();

		return false;
	}

	if (gluBuild2DMipmaps(m_target, m_internalFormat, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, &textureData[0]))
	{
		MarkError();

		return false;
	}

	SetParameter(CTexture::MIN_FILTER, CTexture::LINEAR_MIPMAP_LINEAR);
	SetParameter(CTexture::MAG_FILTER, CTexture::LINEAR);

	GuardedUnbind();

	return true;
}

/**
*/
void CTexture::SetParameter(unsigned int name, int value)
{
	GuardedBind();

	glTexParameteri(m_target, name, value);

	GuardedUnbind();

	if (!MyGLH::IsRenderingContextOk())
		MarkError();
}

/**
*/
void CTexture::SetParameter(unsigned int name, float value)
{
	GuardedBind();

	glTexParameterf(m_target, name, value);

	GuardedUnbind();

	if (!MyGLH::IsRenderingContextOk())
		MarkError();
}

/**
*/
void CTexture::SetParameter(unsigned int name, const int *value)
{
	GuardedBind();

	glTexParameteriv(m_target, name, value);

	GuardedUnbind();

	if (!MyGLH::IsRenderingContextOk())
		MarkError();
}

/**
*/
void CTexture::SetParameter(unsigned int name, const float *value)
{
	GuardedBind();

	glTexParameterfv(m_target, name, value);

	GuardedUnbind();

	if (!MyGLH::IsRenderingContextOk())
		MarkError();
}

/**
*/
void CTexture::SetActiveTextureUnit(unsigned int unit)
{
	glActiveTextureARB(GL_TEXTURE0_ARB + unit);
}

/**
*/
void CTexture::DisableTextureUnit(unsigned int unit)
{
	if (m_textureUnitArray.at(unit))
	{
		glActiveTextureARB(GL_TEXTURE0_ARB + unit);

		glBindTexture(m_textureUnitArray.at(unit), 0);

		glDisable(m_textureUnitArray.at(unit));

		m_textureUnitArray.at(unit) = 0;

		glActiveTextureARB(GL_TEXTURE0_ARB);
	}
}

/**
*/
void CTexture::Bind(unsigned int unit)
{
	glActiveTextureARB(GL_TEXTURE0_ARB + unit);

	m_textureUnitArray.at(unit) = m_target;

	glBindTexture(m_target, m_name);

	glEnable(m_target);
}

/**
*/
int CTexture::GetName(void) const
{
	return m_name;
}

/**
*/
int CTexture::GetWidth(void) const
{
	return m_width;
}

/**
*/
int CTexture::GetHeight(void) const
{
	return m_height;
}

/**
*/
int CTexture::GetDepth(void) const
{
	return m_depth;
}

/**
*/
unsigned int CTexture::GetTarget(void) const
{
	return m_target;
}

/**
*/
unsigned int CTexture::GetInternalFormat(void) const
{
	return m_internalFormat;
}

/**
*/
bool CTexture::IsValid(void)
{
	return (glIsTexture(m_name) != GL_FALSE);
}

/**
*/
void CTexture::GuardedBind(void)
{
	int name;

	glGetIntegerv(m_binding, &name);

	if (m_name != name)
	{
		glBindTexture(m_target, m_name);

		m_lastTexture = name;
	}
}

/**
*/
void CTexture::GuardedUnbind(void)
{
	if (m_name != m_lastTexture)
		glBindTexture(m_target, m_lastTexture);
}

/**
*/
void CTexture::InitializeDefaultContext(void)
{
	// It seems that the textures are not created with default parameters, and 
	// I have to enforce it.
	GuardedBind();

	glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_CLAMP);

	GuardedUnbind();
}

/**
*/
void CTexture::Create(void)
{
	m_name = 0;
	m_lastTexture = 0;

	m_target = 0;

	m_internalFormat = 0;

	m_width = 0;
	m_height = 0;
	m_depth = 0;

	m_binding = 0;
}

/**
*/
void CTexture::Destroy(void)
{
	if (m_name)
		if (glIsTexture(m_name) != GL_FALSE)
			glDeleteTextures(1, &m_name);

	m_name = 0;
	m_lastTexture = 0;

	m_target = 0;

	m_internalFormat = 0;

	m_width = 0;
	m_height = 0;
	m_depth = 0;

	m_binding = 0;
}

