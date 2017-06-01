#ifndef SHADER_INCLUDED
#define SHADER_INCLUDED

#include <string>

#include <boost/utility.hpp>

class CShader
	: boost::noncopyable
{
public:
	enum SHADER_TYPE {
		VERTEX_PROGRAM = 0x8620,
		GEOMETRY_PROGRAM = 0x8DD9,
		FRAGMENT_PROGRAM = 0x8B30
	};
	enum PARAMETER_TYPE {
		GEOMETRY_INPUT_TYPE = 0x8DDB,
		GEOMETRY_OUTPUT_TYPE = 0x8DDC,
		GEOMETRY_VERTICES_OUTPUT_MAX = 0x8DDA
	};
	enum PRIMITIVE_TYPE {
		// You must tell GL what kind of primitives your geometry shader will 
		// accept as input before linking your program. Note that this does 
		// not necessarily correspond to the parameter of glBegin().
		POINTS = 0x0000,
		//GL_LINES,
		//GL_LINES_ADJACENCY_EXT, 
		//GL_TRIANGLES, 
		//GL_TRIANGLES_ADJACENCY_EXT,
		// You must tell GL what kind of primitives your geometry shader will 
		// output before linking your program. This does not necessarily 
		// correspond to the value of GL_GEOMETRY_INPUT_TYPE_EXT.
		// POINTS = 0x0000,
		//GL_LINE_STRIP, 
		TRIANGLE_STRIP = 0x0005
	};
	CShader(void);
	bool Initialize(void);
	virtual ~CShader(void);
	bool Load(const std::string& source, SHADER_TYPE shaderType);
	bool LoadFromFile(const std::string& file, SHADER_TYPE shaderType);
	void SetTextureUnit(const std::string& name, int unit);
	void SetUniformParameter(const std::string& name, float x);
	void SetUniformParameter(const std::string& name, float x, float y);
	void SetUniformParameter(const std::string& name, float x, float y, float z);
	void SetUniformParameter(const std::string& name, float x, float y, float z, float w);
	void SetUniformParameter(const std::string& name, float *data);
	void SetUniformMatrix4x4Parameter(const std::string& name, bool rowMajorOrder, const float *data);
	bool SetParameter(PARAMETER_TYPE parameterType, unsigned int parameterValue);
	void Bind(void) const;
	static void Disable(void);
	bool IsValid(void) const;
private:
	void GuardedBind(void);
	void GuardedUnbind(void);
	void Create(void);
	void Destroy(void);
protected:
	unsigned int m_vertexShaderObjectHandle; 
	unsigned int m_geometryShaderObjectHandle; 
	unsigned int m_fragmentShaderObjectHandle; 
	unsigned int m_programObjectHandle; 
	unsigned int m_lastProgramObjectHandle;
};

#endif // SHADER_INCLUDED

