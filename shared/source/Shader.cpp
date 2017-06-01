#include <iostream>
#include <fstream>

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
#include <MyGLH.h>

#include <Shader.h>

using namespace std;

/**
*/
CShader::CShader(void)
{
	Create();
}

/**
*/
CShader::~CShader(void)
{
	Destroy();
}

/**
*/
bool CShader::Initialize(void)
{
#ifdef _WIN32
	// Check if OpenGL is up.
	if (!wglGetCurrentContext())
	{
		MarkError();

		return false;
	}
#endif // _WIN32

	// Check if GL_ARB_shader_objects extension is supported.
	if (!glewIsExtensionSupported("GL_ARB_shader_objects"))
	{
		MarkError();

		return false;
	}

	// Debug only: check if GL_ARB_shader_objects extension is up.
	if (!glCreateProgramObjectARB ||
		!glCreateShaderObjectARB ||
		!glShaderSourceARB ||
		!glCompileShaderARB ||
		!glGetInfoLogARB ||
		!glGetObjectParameterivARB ||
		!glAttachObjectARB ||
		!glLinkProgramARB ||
		!glValidateProgramARB ||
		!glGetHandleARB ||
		!glUseProgramObjectARB ||
		!glGetUniformLocationARB ||
		!glUniform1iARB ||
		!glUniform2fARB ||
		!glUniform3fARB ||
		!glUniform4fARB ||
		!glUniform4fvARB ||
		!glDeleteObjectARB)
	{
		MarkError();

		return false;
	}

	m_programObjectHandle = glCreateProgramObjectARB();

	if (!m_programObjectHandle)
	{
		MarkError();

		return false;
	}

	if (!MyGLH::IsRenderingContextOk())
	{
		MarkError();

		return false;
	}

	return true;
}

/**
*/
bool CShader::Load(const string& source, SHADER_TYPE shaderType)
{
	GLhandleARB *shaderObjectHandle;
	GLcharARB *sourcePointer,
		infoLog[256];
	GLsizei infoLogLength;
	GLint sourceSize, 
		compilationStatus,
		linkingStatus,
		programStatus;

	if (source.empty())
	{
		MarkError();

		return false;
	}

	switch (shaderType)	{
		case VERTEX_PROGRAM:
			if (!m_vertexShaderObjectHandle)
				m_vertexShaderObjectHandle = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
			
			shaderObjectHandle = &m_vertexShaderObjectHandle;
			break;
		case GEOMETRY_PROGRAM:
			if (!m_geometryShaderObjectHandle)
				m_geometryShaderObjectHandle = glCreateShaderObjectARB(GL_GEOMETRY_SHADER_EXT);
			
			shaderObjectHandle = &m_geometryShaderObjectHandle;
			break;
		case FRAGMENT_PROGRAM:
			if (!m_fragmentShaderObjectHandle)
				m_fragmentShaderObjectHandle = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
			
			shaderObjectHandle = &m_fragmentShaderObjectHandle;
			break;
		default:
			MarkError();
			return false;
	}

	sourcePointer = (GLcharARB*)source.c_str();
	sourceSize = (GLint)source.size();

	glShaderSourceARB(*shaderObjectHandle, 1, (const GLcharARB **)&sourcePointer, &sourceSize);

	glCompileShaderARB(*shaderObjectHandle);

	infoLogLength = 0;

	glGetInfoLogARB(*shaderObjectHandle, 255, &infoLogLength, infoLog);

	// The ATI driver returns a friendly text saying that "its all ok" that can't 
	// be faced as an error (nVIDIA drivers just say something when something 
	// goes wrong). Given so, I give up from check the size of the answer as an 
	// indication of the error.
	//if (infoLogLength)
	//{
		//ReportError(infoLog);

		//return false;
	//}

	glGetObjectParameterivARB(*shaderObjectHandle, GL_OBJECT_COMPILE_STATUS_ARB, &compilationStatus);

	if (compilationStatus == GL_FALSE)
	{
		ReportError(infoLog);

		MarkError();

		return false;
	}

	glAttachObjectARB(m_programObjectHandle, *shaderObjectHandle);

	glLinkProgramARB(m_programObjectHandle);

	glGetInfoLogARB(m_programObjectHandle, 255, &infoLogLength, infoLog);

	// The ATI driver returns a friendly text saying that "its all ok" that can't 
	// be faced as an error (nVIDIA drivers just say something when something 
	// goes wrong). Given so, I give up from check the size of the answer as an 
	// indication of the error.
	//if (infoLogLength)
	//{
		//ReportError(infoLog);

		//return false;
	//}

	glGetObjectParameterivARB(m_programObjectHandle, GL_OBJECT_LINK_STATUS_ARB, &linkingStatus);

	if (linkingStatus == GL_FALSE)
	{
		ReportError(infoLog);

		MarkError();

		return false;
	}

	glValidateProgramARB(m_programObjectHandle);

	glGetObjectParameterivARB(m_programObjectHandle, GL_OBJECT_VALIDATE_STATUS_ARB, &programStatus);

	if (programStatus == GL_FALSE)
	{
		//MarkError();

		//return false;
	}

	if (!MyGLH::IsRenderingContextOk())
	{
		MarkError();

		return false;
	}

	return true;
}

/**
*/
bool CShader::LoadFromFile(const string& file, SHADER_TYPE shaderType)
{
	ifstream inputFile;
	istreambuf_iterator<char> inputBegin,
		inputEnd;
	string source;

	if (file.empty())
	{
		MarkError();

		return false;
	}

	inputFile.open(file.c_str());

	inputBegin = inputFile;

	source.assign(inputBegin, inputEnd);

	if (!Load(source, shaderType))
	{
		MarkError();

		return false;
	}

	return true;
}

/**
*/
void CShader::SetTextureUnit(const std::string& name, int unit)
{
	GLint location;

	GuardedBind();

	location = glGetUniformLocationARB(m_programObjectHandle, (const GLchar *)name.c_str());

	if (location != -1)
		glUniform1iARB(location, unit);
	else
		MarkError();

	GuardedUnbind();
}

/**
*/
void CShader::SetUniformParameter(const std::string& name, float x)
{
	GLint location;

	GuardedBind();

	location = glGetUniformLocationARB(m_programObjectHandle, (const GLchar *)name.c_str());

	if (location != -1)
		glUniform1fARB(location, x);
	else
		MarkError();

	GuardedUnbind();
}

/**
*/
void CShader::SetUniformParameter(const std::string& name, float x, float y)
{
	GLint location;

	GuardedBind();

	location = glGetUniformLocationARB(m_programObjectHandle, (const GLchar *)name.c_str());

	if (location != -1)
		glUniform2fARB(location, x, y);
	else
		MarkError();

	GuardedUnbind();
}

/**
*/
void CShader::SetUniformParameter(const std::string& name, float x, float y, float z)
{
	GLint location;

	GuardedBind();

	location = glGetUniformLocationARB(m_programObjectHandle, (const GLchar *)name.c_str());

	if (location != -1)
		glUniform3fARB(location, x, y, z);
	else
		MarkError();

	GuardedUnbind();
}

/**
*/
void CShader::SetUniformParameter(const std::string& name, float x, float y, float z, float w)
{
	GLint location;

	GuardedBind();

	location = glGetUniformLocationARB(m_programObjectHandle, (const GLchar *)name.c_str());

	if (location != -1)
		glUniform4fARB(location, x, y, z, w);
	else
		MarkError();

	GuardedUnbind();
}

/**
*/
void CShader::SetUniformParameter(const std::string& name, float *data)
{
	GLint location;

	GuardedBind();

	location = glGetUniformLocationARB(m_programObjectHandle, (const GLchar *)name.c_str());

	if (location != -1)
		glUniform4fvARB(location, 4, data);
	else
		MarkError();

	GuardedUnbind();
}

/**
*/
void CShader::SetUniformMatrix4x4Parameter(const std::string& name, bool rowMajorOrder, const float *data)
{
	GLint location;

	GuardedBind();

	location = glGetUniformLocationARB(m_programObjectHandle, (const GLchar *)name.c_str());

	if (location != -1)
		glUniformMatrix4fv(location, 1, (rowMajorOrder) ? GL_TRUE:GL_FALSE, data);
	else
		MarkError();

	GuardedUnbind();
}

/**
*/
bool CShader::SetParameter(PARAMETER_TYPE parameterId, unsigned int parameterValue)
{
	GuardedBind();

	glProgramParameteriEXT(m_programObjectHandle, parameterId , parameterValue);

	GuardedUnbind();

	return true;
}

/**
*/
void CShader::Bind(void) const
{
	glUseProgramObjectARB(m_programObjectHandle);
}

/**
*/
void CShader::Disable(void)
{
	glUseProgramObjectARB(0);
}

/**
*/
bool CShader::IsValid(void) const
{
	return (m_programObjectHandle && (m_vertexShaderObjectHandle || m_fragmentShaderObjectHandle));
}

/**
*/
void CShader::GuardedBind(void)
{
	m_lastProgramObjectHandle = glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
	
	if (m_lastProgramObjectHandle != m_programObjectHandle)
		glUseProgramObjectARB(m_programObjectHandle);
}

/**
*/
void CShader::GuardedUnbind(void)
{
	if (m_lastProgramObjectHandle != m_programObjectHandle)
		glUseProgramObjectARB(m_lastProgramObjectHandle);
}

/**
*/
void CShader::Create(void)
{
	m_vertexShaderObjectHandle = 0;
	m_fragmentShaderObjectHandle = 0;

	m_programObjectHandle = 0;
	m_lastProgramObjectHandle = 0;
}

/**
*/
void CShader::Destroy(void)
{
	if (m_programObjectHandle)
	{
		// Automatically detach shader objects attached to it, but not delete 
		// them.
		glDeleteObjectARB(m_programObjectHandle);
		
		if (m_vertexShaderObjectHandle)
			glDeleteObjectARB(m_vertexShaderObjectHandle);
		
		if (m_fragmentShaderObjectHandle)
			glDeleteObjectARB(m_fragmentShaderObjectHandle);
	}
}

