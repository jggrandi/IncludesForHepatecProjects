#ifndef VERTEXPOOL_INCLUDED
#define VERTEXPOOL_INCLUDED

#include <vector>
#include <string>

#include <boost/utility.hpp>

// Struct?
class CVertexPool 
	: boost::noncopyable
{
public:
	typedef std::vector<unsigned int> element_array_type;
	enum STREAM_FORMAT {
		OFF,
		STEVEN_OFF,
		PLY,
		ASCII_STL,
		SURFEL,
		CRYSTAL_STRUCTURE_DISLOCATIONS
	};
	enum POLYGON_MODE {
		POINTS = 0x0000,
		LINES = 0x0001,
		LINE_LOOP = 0x0002,
		LINE_STRIP = 0x0003,
		TRIANGLES = 0x0004,
		TRIANGLE_STRIP = 0x0005,
		TRIANGLE_FAN = 0x0006,
		QUADS = 0x0007,
		QUAD_STRIP = 0x0008,
		POLYGON = 0x0009
	};
	CVertexPool(void);
	virtual ~CVertexPool(void);
	void SetMode(POLYGON_MODE mode);
	POLYGON_MODE GetMode(void) const;
	void AddLabel(int label);
	void SetLabel(int n, int label);
	int GetLabel(int n) const;
	const int *GetLabelArray(void) const;
	int GetLabelCount(void) const;
	void ClearLabels(void);
	void AddColor(float r, float g, float b);
	void AddColor(float r, float g, float b, float a);
	void AddColor(const float *begin, const float *end);
	void SetColor(int n, float r, float g, float b);
	void SetColor(int n, float r, float g, float b, float a);
	void SetColor(int n, const float *begin, const float *end);
	void SetColorSize(int size);
	const float *GetColor(int n) const;
	const float *GetColorArray(void) const;
	int GetColorSize(void) const;
	int GetColorCount(void) const;
	void ClearColors(void);
	void AddTexCoord(float s);
	void AddTexCoord(float s, float t);
	void AddTexCoord(float s, float t, float r);
	void AddTexCoord(float s, float t, float r, float q);
	void AddTexCoord(const float *begin, const float *end);
	void SetTexCoord(int n, float s);
	void SetTexCoord(int n, float s, float t);
	void SetTexCoord(int n, float s, float t, float r);
	void SetTexCoord(int n, float s, float t, float r, float q);
	void SetTexCoord(int n, const float *begin, const float *end);
	void SetTexCoordSize(int size);
	const float *GetTexCoord(int n) const;
	const float *GetTexCoordArray(void) const;
	int GetTexCoordSize(void) const;
	int GetTexCoordCount(void) const;
	void ClearTexCoords(void);
	void AddNormal(float x, float y, float z);
	void AddNormal(const float *begin, const float *end);
	void SetNormal(int n, float x, float y, float z);
	void SetNormal(int n, const float *begin, const float *end);
	const float *GetNormal(int n) const;
	const float *GetNormalArray(void) const;
	int GetNormalCount(void) const;
	void ClearNormals(void);
	void AddVertex(float x);
	void AddVertex(float x, float y);
	void AddVertex(float x, float y, float z);
	void AddVertex(float x, float y, float z, float w);
	void AddVertex(const float *begin, const float *end);
	void SetVertex(int n, float x);
	void SetVertex(int n, float x, float y);
	void SetVertex(int n, float x, float y, float z);
	void SetVertex(int n, float x, float y, float z, float w);
	void SetVertex(int n, const float *begin, const float *end);
	void SetVertexSize(int size);
	const float *GetVertex(int n) const;
	const float *GetVertexArray(void) const;
	int GetVertexSize(void) const;
	int GetVertexCount(void) const;
	void ClearVertices(void);
	void AddElement(unsigned int vertexId);
	void AddElementArray(const unsigned int *begin, const unsigned int *end);
	void AddLine(unsigned int firstId, unsigned int secondId);
	void AddTriangle(unsigned int firstId, unsigned int secondId, unsigned int thirdId);
	void AddQuad(unsigned int firstId, unsigned int secondId, unsigned int thirdId, unsigned int fourthId);
	unsigned int GetElement(int n) const;
	const unsigned *GetElementArray(void) const;
	int GetElementCount(void) const;
	void ClearElements(void);
	unsigned int GetPrimitiveType(void) const;
	const unsigned *GetPrimitive(int primitiveId) const;
	int GetPrimitiveSize(void) const;
	int GetPrimitiveCount(void) const;
	virtual void Clear(void);
	bool AppendVertexPool(const CVertexPool *vertexPool);
	bool Draw(void) const;
	bool Open(const std::string fileName, STREAM_FORMAT type);
	bool Save(const std::string fileName, STREAM_FORMAT type) const;
	bool IsValid(void) const;

	
	void RemoveVertex3D(void);
	void RemoveElement(void);

protected:
	bool SaveToASCIISTLFile(const std::string name) const;
	bool SaveToSURFELFile(const std::string name) const;
	bool OpenOFF(const std::string name);
	bool OpenStevenOFF(const std::string name);
	bool OpenCrystalStructureDislocation(const std::string name);
	virtual void Create(void);
public:
	unsigned int m_mode;
	std::vector<int> m_labelArray;
	int m_colorSize;
	std::vector<float> m_colorArray;
	int m_texCoordSize;
	std::vector<float> m_texCoordArray;
	std::vector<float> m_normalArray;
	int m_vertexSize;
	std::vector<float> m_vertexArray;
	std::vector<unsigned int> m_elementArray;
};

#endif // VERTEXPOOL_INCLUDED

