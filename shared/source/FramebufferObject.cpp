#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

#include <GL/glew.h>

#ifdef _WIN32
#include <GL/wglew.h>
#else // !_WIN32
#include <GL/glxew.h>
#endif // _WIN32

#include <algorithm>

#include <ErrorManager.h>
#include <MyGLH.h>

#include <FramebufferObject.h>

using namespace std;

int CFramebufferObject::m_defaultDrawBuffer = GL_BACK;
int CFramebufferObject::m_defaultFramebufferViewport[4] = {0, 0, 0, 0};

/**
*/
CFramebufferObject::CFramebufferObject(void)
{
	Create();
}

/**
*/
CFramebufferObject::~CFramebufferObject(void)
{
	Destroy();
}

/**
*/
bool CFramebufferObject::Initialize(void)
{
#ifdef _WIN32
	// Check if OpenGL is up.
	if (!wglGetCurrentContext())
	{
		MarkError();

		return false;
	}
#endif // _WIN32

	// Check if GL_EXT_framebuffer_object extension is supported.
	if (!glewIsExtensionSupported("GL_EXT_framebuffer_object"))
	{
		MarkError();

		return false;
	}

	// Debug only: check if GL_EXT_framebuffer_object extension is up.
	if (!glGenFramebuffersEXT ||
		!glIsFramebufferEXT ||
		!glGetRenderbufferParameterivEXT ||
		!glFramebufferRenderbufferEXT ||
		!glBindFramebufferEXT ||
		!glCheckFramebufferStatusEXT ||
		!glFramebufferTexture1DEXT ||
		!glFramebufferTexture2DEXT ||
		!glFramebufferTexture3DEXT ||
		!glDeleteFramebuffersEXT)
	{
		MarkError();

		return false;
	}

	glGenFramebuffersEXT(1, &m_name);
	
	m_width = 0;
	m_height = 0;
	
	m_lastFramebuffer = 0;

	if (!MyGLH::IsRenderingContextOk())
	{
		MarkError();

		return false;
	}

	return true;
}

/**
*/
bool CFramebufferObject::AttachTexture(ATTACHMENT_TYPE attachment, boost::shared_ptr<CTexture> texture, int level, int slice)
{
	if (!texture.get())
	{
		MarkError();

		return false;
	}

	//if ((m_width == 0) || (attachment == COLOR_ATTACHMENT0))
	//{
		m_width = texture->GetWidth();
		m_height = texture->GetHeight();
	//}
	//else if ((m_width != texture->GetWidth()) || (m_height != texture->GetHeight()))
	//{
	//	MarkError();
	//
	//	return false;
	//}

	if (!AttachTextureND(attachment, texture->GetTarget(), texture->GetName(), level, slice))
	{
		MarkError();

		return false;
	}

	if (glIsFramebufferEXT(m_name) == GL_FALSE)
	{
		MarkError();

		return false;
	}

	if (!MyGLH::IsRenderingContextOk())
	{
		MarkError();

		return false;
	}

	if (find(m_drawBufferArray.begin(), m_drawBufferArray.end(), attachment) == m_drawBufferArray.end())
		m_drawBufferArray.push_back(attachment);

	return true;
}

/**
*/
bool CFramebufferObject::AttachRenderbuffer(ATTACHMENT_TYPE attachment, unsigned int name)
{
	//int renderbufferWidth,
	//	renderbufferHeight;
	//
	//glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, name);
	//
	//glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_WIDTH_EXT, &renderbufferWidth);
	//glGetRenderbufferParameterivEXT(GL_RENDERBUFFER_EXT, GL_RENDERBUFFER_HEIGHT_EXT, &renderbufferHeight);
	//
	//glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);
	//
	//if (m_width != 0)
	//{
	//	if (m_width != renderbufferWidth)
	//	{
	//		MarkError();
	//
	//		return false;
	//	}
	//
	//	if (m_height != renderbufferHeight)
	//	{
	//		MarkError();
	//
	//		return false;
	//	}
	//}

	GuardedBind();

	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, attachment, GL_RENDERBUFFER_EXT, name);

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
void CFramebufferObject::Bind(void)
{
	int currentFramebuffer;

	glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &currentFramebuffer);

	if (currentFramebuffer == 0)
	{
		glGetIntegerv(GL_DRAW_BUFFER, &m_defaultDrawBuffer);

		glGetIntegerv(GL_VIEWPORT, m_defaultFramebufferViewport);
	}

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_name);

	glDrawBuffers(GLsizei(m_drawBufferArray.size()), &m_drawBufferArray[0]);

	glViewport(0, 0, m_width, m_height);
}

/**
*/
void CFramebufferObject::Disable(void) 
{
	int currentFramebuffer;

	glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &currentFramebuffer);

	if (currentFramebuffer != 0) 
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

		glDrawBuffer(m_defaultDrawBuffer);

		glViewport(m_defaultFramebufferViewport[0], m_defaultFramebufferViewport[1], m_defaultFramebufferViewport[2], m_defaultFramebufferViewport[3]);
	}
}

/**
*/
unsigned int CFramebufferObject::GetWidth(void) const
{
	return m_width;
}

/**
*/
unsigned int CFramebufferObject::GetHeight(void) const
{
	return m_height;
}

/**
*/
bool CFramebufferObject::IsValid(void)
{
	bool status;

	status = false;

	if (glIsFramebufferEXT(m_name) != GL_FALSE)
	{
		GuardedBind();

		if (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) == GL_FRAMEBUFFER_COMPLETE_EXT)
			status = true;

		GuardedUnbind();
	}

	return status;
}

/**
*/
bool CFramebufferObject::GetPixel(unsigned int x, unsigned int y, float *pixel)
{
	int currentReadBuffer;

	if (x >= m_width)
	{
		MarkError();

		return false;
	}

	if (y >= m_height)
	{
		MarkError();

		return false;
	}

	GuardedBind();

	glGetIntegerv(GL_READ_BUFFER, &currentReadBuffer);

	glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
	
	glReadPixels(x, y, 1, 1, GL_RGBA, GL_FLOAT, pixel);
	
	glReadBuffer(currentReadBuffer);

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
void CFramebufferObject::GuardedBind(void) 
{
	glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &m_lastFramebuffer);

	if (m_name != m_lastFramebuffer)
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_name);
}

/**
*/
void CFramebufferObject::GuardedUnbind(void) const
{
	if (m_name != m_lastFramebuffer) 
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_lastFramebuffer);
}

/**
*/
bool CFramebufferObject::AttachTextureND(unsigned int attachment, unsigned int target, unsigned int name, int level, int slice)
{
	GuardedBind();

	switch (target)	{
		case GL_TEXTURE_1D:
			glFramebufferTexture1DEXT(GL_FRAMEBUFFER_EXT, attachment, GL_TEXTURE_1D, name, level);
			break;
		case GL_TEXTURE_2D:
		case GL_TEXTURE_DEPTH:
		case GL_TEXTURE_RECTANGLE_ARB:
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, attachment, target, name, level);     
			break;
		case GL_TEXTURE_3D:
			glFramebufferTexture3DEXT(GL_FRAMEBUFFER_EXT, attachment, GL_TEXTURE_3D, name, level, slice);
			break;
		default:
			MarkError();

			GuardedUnbind();
			return false;
	}

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
void CFramebufferObject::Create(void)
{
	m_name = 0;
	
	m_width = 0;
	m_height = 0;

	m_drawBufferArray.clear();
	
	m_lastFramebuffer = 0;
}

/**
*/
void CFramebufferObject::Destroy(void)
{
	if (m_name)
		if (glIsFramebufferEXT(m_name) != GL_FALSE)
			glDeleteFramebuffersEXT(1, &m_name);
}

