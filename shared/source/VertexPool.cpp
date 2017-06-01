#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#endif // _MSC_VER

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

#include <GL/gl.h>

#include <iostream>
#include <fstream>

#include <ErrorManager.h>
#include <MyMath.h>

#include <VertexPool.h>

using namespace std;

/**
*/
CVertexPool::CVertexPool(void)
{
	Create();
}

/**
*/
CVertexPool::~CVertexPool(void)
{
}

/**
*/
void CVertexPool::SetMode(POLYGON_MODE mode)
{
	m_mode = mode;
}

/**
*/
CVertexPool::POLYGON_MODE CVertexPool::GetMode(void) const
{
	return CVertexPool::POLYGON_MODE(m_mode);
}

/**
*/
void CVertexPool::AddLabel(int label)
{ 
	m_labelArray.push_back(label);
}

/**
*/
void CVertexPool::SetLabel(int n, int label)
{
	if (n < (int)m_labelArray.size())
		m_labelArray[n] = label;
	else 
		MarkError();
}

/**
*/
int CVertexPool::GetLabel(int n) const
{
	if (n < (int)m_labelArray.size())
		return m_labelArray[n];

	MarkError();

	return 0;
}

/**
*/
const int *CVertexPool::GetLabelArray(void) const
{
	if (m_labelArray.empty())
		return 0;

	return &m_labelArray[0];
}

/**
*/
int CVertexPool::GetLabelCount(void) const
{
	return (int)m_labelArray.size();
}

/**
*/
void CVertexPool::ClearLabels(void)
{
	m_labelArray.clear();
}

/**
*/
void CVertexPool::AddColor(float r, float g, float b)
{ 
	if (!m_colorSize)
		m_colorSize = 3;

	if (m_colorSize == 3)
	{
		m_colorArray.push_back(r);
		m_colorArray.push_back(g);
		m_colorArray.push_back(b);
	}
	else
		MarkError();
}

/**
*/
void CVertexPool::AddColor(float r, float g, float b, float a)
{ 
	if (!m_colorSize)
		m_colorSize = 4;

	if (m_colorSize == 4)
	{
		m_colorArray.push_back(r);
		m_colorArray.push_back(g);
		m_colorArray.push_back(b);
		m_colorArray.push_back(a);
	}
	else
		MarkError();
}

/**
*/
void CVertexPool::AddColor(const float *begin, const float *end)
{
	int size;

	size = (int)(end - begin);

	if (!m_colorSize)
		m_colorSize = size;

	if (m_colorSize == size)
		m_colorArray.insert(m_colorArray.end(), begin, end);
	else
		MarkError();
}

/**
*/
void CVertexPool::SetColor(int n, float r, float g, float b)
{
	float *color;

	n *= m_colorSize;

	if ((n < (int)m_colorArray.size()) && (m_colorSize == 3))
	{
		color = &m_colorArray[0] + n;

		*color++ = r;
		*color++ = g;
		*color = b;
	}
	else 
		MarkError();
}

/**
*/
void CVertexPool::SetColor(int n, float r, float g, float b, float a)
{
	float *color;

	n *= m_colorSize;

	if ((n < (int)m_colorArray.size()) && (m_colorSize == 4))
	{
		color = &m_colorArray[0] + n;

		*color++ = r;
		*color++ = g;
		*color++ = b;
		*color = a;
	}
	else 
		MarkError();
}

/**
*/
void CVertexPool::SetColor(int n, const float *begin, const float *end)
{
	int size;
	float *color;

	size = (int)(end - begin);

	n *= m_colorSize;

	if ((n < (int)m_colorArray.size()) && (m_colorSize == size))
	{
		color = &m_colorArray[0] + n;

		for ( ; size--; )
			*color++ = *begin++;
	}
	else 
		MarkError();
}

/**
*/
void CVertexPool::SetColorSize(int size)
{
	if (size > 0)
		m_colorSize = size;
	else 
		MarkError();
}

/**
*/
const float *CVertexPool::GetColor(int n) const
{
	n *= m_colorSize;

	if (n < (int)m_colorArray.size())
		return (&m_colorArray[0] + n);

	MarkError();

	return 0;
}

/**
*/
const float *CVertexPool::GetColorArray(void) const
{
	if (m_colorArray.empty())
		return 0;

	return &m_colorArray[0];
}

/**
*/
int CVertexPool::GetColorSize(void) const
{
	return m_colorSize;
}

/**
*/
int CVertexPool::GetColorCount(void) const
{
	if (!m_colorSize)
		return 0;

	return (int)m_colorArray.size()/m_colorSize;
}

/**
*/
void CVertexPool::ClearColors(void)
{
	m_colorSize = 0;

	m_colorArray.clear();
}

/**
*/
void CVertexPool::AddTexCoord(float s)
{ 
	if (!m_texCoordSize)
		m_texCoordSize = 1;

	if (m_texCoordSize == 1)
		m_texCoordArray.push_back(s);
	else
		MarkError();
}

/**
*/
void CVertexPool::AddTexCoord(float s, float t)
{ 
	if (!m_texCoordSize)
		m_texCoordSize = 2;

	if (m_texCoordSize == 2)
	{
		m_texCoordArray.push_back(s);
		m_texCoordArray.push_back(t);
	}
	else
		MarkError();
}

/**
*/
void CVertexPool::AddTexCoord(float s, float t, float r)
{ 
	if (!m_texCoordSize)
		m_texCoordSize = 3;

	if (m_texCoordSize == 3)
	{
		m_texCoordArray.push_back(s);
		m_texCoordArray.push_back(t);
		m_texCoordArray.push_back(r);
	}
	else
		MarkError();
}

/**
*/
void CVertexPool::AddTexCoord(float s, float t, float r, float q)
{
	if (!m_texCoordSize)
		m_texCoordSize = 4;

	if (m_texCoordSize == 4)
	{
		m_texCoordArray.push_back(s);
		m_texCoordArray.push_back(t);
		m_texCoordArray.push_back(r);
		m_texCoordArray.push_back(q);
	}
	else
		MarkError();
}

/**
*/
void CVertexPool::AddTexCoord(const float *begin, const float *end)
{
	int size;

	size = (int)(end - begin);

	if (!m_texCoordSize)
		m_texCoordSize = size;

	if (m_texCoordSize == size)
		m_texCoordArray.insert(m_texCoordArray.end(), begin, end);
	else
		MarkError();
}

/**
*/
void CVertexPool::SetTexCoord(int n, float s)
{
	float *texCoord;

	n *= m_texCoordSize;

	if ((n < (int)m_texCoordArray.size()) && (m_texCoordSize == 1))
	{
		texCoord = &m_texCoordArray[0] + n;

		*texCoord = s;
	}
	else 
		MarkError();
}

/**
*/
void CVertexPool::SetTexCoord(int n, float s, float t)
{
	float *texCoord;

	n *= m_texCoordSize;

	if ((n < (int)m_texCoordArray.size()) && (m_texCoordSize == 2))
	{
		texCoord = &m_texCoordArray[0] + n;

		*texCoord++ = s;
		*texCoord = t;
	}
	else 
		MarkError();
}

/**
*/
void CVertexPool::SetTexCoord(int n, float s, float t, float r)
{
	float *texCoord;

	n *= m_texCoordSize;

	if ((n < (int)m_texCoordArray.size()) && (m_texCoordSize == 3))
	{
		texCoord = &m_texCoordArray[0] + n;

		*texCoord++ = s;
		*texCoord++ = t;
		*texCoord = r;
	}
	else 
		MarkError();
}

/**
*/
void CVertexPool::SetTexCoord(int n, float s, float t, float r, float q)
{
	float *texCoord;

	n *= m_texCoordSize;

	if ((n < (int)m_texCoordArray.size()) && (m_texCoordSize == 4))
	{
		texCoord = &m_texCoordArray[0] + n;

		*texCoord++ = s;
		*texCoord++ = t;
		*texCoord++ = r;
		*texCoord = q;
	}
	else 
		MarkError();
}

/**
*/
void CVertexPool::SetTexCoord(int n, const float *begin, const float *end)
{
	int size;
	float *texCoord;

	size = (int)(end - begin);

	n *= m_texCoordSize;

	if ((n < (int)m_texCoordArray.size()) && (m_texCoordSize == size))
	{
		texCoord = &m_texCoordArray[0] + n;

		for ( ; size--; )
			*texCoord++ = *begin++;
	}
	else 
		MarkError();
}

/**
*/
void CVertexPool::SetTexCoordSize(int size)
{
	if (size > 0)
		m_texCoordSize = size;
	else 
		MarkError();
}

/**
*/
const float *CVertexPool::GetTexCoord(int n) const
{
	n *= m_texCoordSize;

	if (n < (int)m_texCoordArray.size())
		return (&m_texCoordArray[0] + n);

	MarkError();

	return 0;
}

/**
*/
const float *CVertexPool::GetTexCoordArray(void) const
{
	if (m_texCoordArray.empty())
		return 0;

	return &m_texCoordArray[0];
}

/**
*/
int CVertexPool::GetTexCoordSize(void) const
{
	return m_texCoordSize;
}

/**
*/
int CVertexPool::GetTexCoordCount(void) const
{
	if (!m_texCoordSize)
		return 0;

	return (int)m_texCoordArray.size()/m_texCoordSize;
}

/**
*/
void CVertexPool::ClearTexCoords(void)
{
	m_texCoordSize = 0;

	m_texCoordArray.clear();
}

/**
*/
void CVertexPool::AddNormal(float x, float y, float z)
{
	m_normalArray.push_back(x);
	m_normalArray.push_back(y);
	m_normalArray.push_back(z);
}

/**
*/
void CVertexPool::AddNormal(const float *begin, const float *end)
{
	int size;

	size = (int)(end - begin);

	if (size == 3)
		m_normalArray.insert(m_normalArray.end(), begin, end);
	else
		MarkError();
}

/**
*/
void CVertexPool::SetNormal(int n, float x, float y, float z)
{
	float *normal;

	n *= 3;

	if (n < (int)m_normalArray.size())
	{
		normal = &m_normalArray[0] + n;

		*normal++ = x;
		*normal++ = y;
		*normal = z;
	}
	else 
		MarkError();
}

/**
*/
void CVertexPool::SetNormal(int n, const float *begin, const float *end)
{
	int size;
	float *normal;

	size = (int)(end - begin);

	n *= 3;

	if (n < (int)m_normalArray.size())
	{
		normal = &m_normalArray[0] + n;

		for ( ; size--; )
			*normal++ = *begin++;
	}
	else 
		MarkError();
}

/**
*/
const float *CVertexPool::GetNormal(int n) const
{
	n *= 3;

	if (n < (int)m_normalArray.size())
		return (&m_normalArray[0] + n);

	MarkError();

	return 0;
}

/**
*/
const float *CVertexPool::GetNormalArray(void) const
{
	if (m_normalArray.empty())
		return 0;

	return &m_normalArray[0];
}

/**
*/
int CVertexPool::GetNormalCount(void) const
{
	return (int)m_normalArray.size()/3;
}

/**
*/
void CVertexPool::ClearNormals(void)
{
	m_normalArray.clear();
}

/**
*/
void CVertexPool::AddVertex(float x)
{ 
	if (!m_vertexSize)
		m_vertexSize = 1;

	if (m_vertexSize == 1)
		m_vertexArray.push_back(x);
	else
		MarkError();
}

/**
*/
void CVertexPool::AddVertex(float x, float y)
{ 
	if (!m_vertexSize)
		m_vertexSize = 2;

	if (m_vertexSize == 2)
	{
		m_vertexArray.push_back(x);
		m_vertexArray.push_back(y);
	}
	else
		MarkError();
}

/**
*/
void CVertexPool::AddVertex(float x, float y, float z)
{ 
	if (!m_vertexSize)
		m_vertexSize = 3;

	if (m_vertexSize == 3)
	{
		m_vertexArray.push_back(x);
		m_vertexArray.push_back(y);
		m_vertexArray.push_back(z);
	}
	else
		MarkError();
}

/**
*/
void CVertexPool::AddVertex(float x, float y, float z, float w)
{ 
	if (!m_vertexSize)
		m_vertexSize = 4;

	if (m_vertexSize == 4)
	{
		m_vertexArray.push_back(x);
		m_vertexArray.push_back(y);
		m_vertexArray.push_back(z);
		m_vertexArray.push_back(w);
	}
	else
		MarkError();
}

/**
*/
void CVertexPool::AddVertex(const float *begin, const float *end)
{
	int size;

	size = (int)(end - begin);

	if (!m_vertexSize)
		m_vertexSize = size;

	if (m_vertexSize == size)
		m_vertexArray.insert(m_vertexArray.end(), begin, end);
	else
		MarkError();
}

/**
*/
void CVertexPool::SetVertex(int n, float x)
{
	float *vertex;

	n *= m_vertexSize;

	if ((n < (int)m_vertexArray.size()) && (m_vertexSize == 1))
	{
		vertex = &m_vertexArray[0] + n;

		*vertex = x;
	}
	else 
		MarkError();
}

/**
*/
void CVertexPool::SetVertex(int n, float x, float y)
{
	float *vertex;

	n *= m_vertexSize;

	if ((n < (int)m_vertexArray.size()) && (m_vertexSize == 2))
	{
		vertex = &m_vertexArray[0] + n;

		*vertex++ = x;
		*vertex = y;
	}
	else 
		MarkError();
}

/**
*/
void CVertexPool::SetVertex(int n, float x, float y, float z)
{
	float *vertex;

	n *= m_vertexSize;

	if ((n < (int)m_vertexArray.size()) && (m_vertexSize == 3))
	{
		vertex = &m_vertexArray[0] + n;

		*vertex++ = x;
		*vertex++ = y;
		*vertex = z;
	}
	else 
		MarkError();
}

/**
*/
void CVertexPool::SetVertex(int n, float x, float y, float z, float w)
{
	float *vertex;

	n *= m_vertexSize;

	if ((n < (int)m_vertexArray.size()) && (m_vertexSize == 4))
	{
		vertex = &m_vertexArray[0] + n;

		*vertex++ = x;
		*vertex++ = y;
		*vertex++ = z;
		*vertex = w;
	}
	else 
		MarkError();
}

/**
*/
void CVertexPool::SetVertex(int n, const float *begin, const float *end)
{
	int size;
	float *vertex;

	size = (int)(end - begin);

	n *= m_vertexSize;

	if ((n < (int)m_vertexArray.size()) && (m_vertexSize == size))
	{
		vertex = &m_vertexArray[0] + n;

		for ( ; size--; )
			*vertex++ = *begin++;
	}
	else 
		MarkError();
}

/**
*/
void CVertexPool::SetVertexSize(int size)
{
	if (size > 0)
		m_vertexSize = size;
	else 
		MarkError();
}

/**
*/
const float *CVertexPool::GetVertex(int n) const
{
	n *= m_vertexSize;

	if (n < (int)m_vertexArray.size())
		return (&m_vertexArray[0] + n);

	MarkError();

	return 0;
}

/**
*/
const float *CVertexPool::GetVertexArray(void) const
{
	if (m_vertexArray.empty())
		return 0;

	return &m_vertexArray[0];
}

/**
*/
int CVertexPool::GetVertexSize(void) const
{
	return m_vertexSize;
}

/**
*/
int CVertexPool::GetVertexCount(void) const
{
	if (!m_vertexSize)
		return 0;

	return (int)m_vertexArray.size()/m_vertexSize;
}

/**
*/
void CVertexPool::ClearVertices(void)
{
	m_vertexSize = 0;

	m_vertexArray.clear();
}

/**
*/
void CVertexPool::AddElement(unsigned int vertexId)
{
	m_elementArray.push_back(vertexId);
}

/**
*/
void CVertexPool::AddElementArray(const unsigned int *begin, const unsigned int *end)
{
	m_elementArray.insert(m_elementArray.end(), begin, end);
}

/**
*/
void CVertexPool::AddLine(unsigned int firstId, unsigned int secondId)
{
	if (!m_mode)
		m_mode = LINES;

	if (m_mode == LINES)
	{
		m_elementArray.push_back(firstId);
		m_elementArray.push_back(secondId);
	}
	else
		MarkError();
}

/**
*/
void CVertexPool::AddTriangle(unsigned int firstId, unsigned int secondId, unsigned int thirdId)
{
	if (!m_mode)
		m_mode = TRIANGLES;

	if (m_mode == TRIANGLES)
	{
		m_elementArray.push_back(firstId);
		m_elementArray.push_back(secondId);
		m_elementArray.push_back(thirdId);
	}
	else
		MarkError();
}

/**
*/
void CVertexPool::AddQuad(unsigned int firstId, unsigned int secondId, unsigned int thirdId, unsigned int fourthId)
{
	if (!m_mode)
		m_mode = QUADS;

	if (m_mode == QUADS)
	{
		m_elementArray.push_back(firstId);
		m_elementArray.push_back(secondId);
		m_elementArray.push_back(thirdId);
		m_elementArray.push_back(fourthId);
	}
	else
		MarkError();
}

/**
*/
unsigned int CVertexPool::GetElement(int n) const
{
	if (n < (int)m_elementArray.size())
		return m_elementArray[n];

	MarkError();

	return 0;
}

/**
*/
const unsigned int *CVertexPool::GetElementArray(void) const
{
	if (m_elementArray.empty())
		return 0;

	return &m_elementArray[0];
}

/**
*/
int CVertexPool::GetElementCount(void) const
{
	return (int)m_elementArray.size();
}

/**
*/
void CVertexPool::ClearElements(void)
{
	m_mode = CVertexPool::POINTS;

	m_elementArray.clear();
}

/**
*/
unsigned int CVertexPool::GetPrimitiveType(void) const
{
	return m_mode;
}

/**
*/
const unsigned *CVertexPool::GetPrimitive(int primitiveId) const
{
	int size;

	if (m_elementArray.empty())
		return 0;

	switch (m_mode)
	{
	case POINTS:
		size = 1;
		break;
	case LINES:
		size = 2;
		break;
	case TRIANGLES:
		size = 3;
		break;
	case QUADS:
		size = 4;
		break;
	default:
		return 0;
	}

	return &m_elementArray[0] + primitiveId*size;
}

/**
*/
int CVertexPool::GetPrimitiveSize(void) const
{
	int size;

	switch (m_mode)
	{
	case POINTS:
		size = 1;
		break;
	case LINES:
		size = 2;
		break;
	case TRIANGLES:
		size = 3;
		break;
	case QUADS:
		size = 4;
		break;
	default:
		return -1;
	}

	return size;
}

/**
*/
int CVertexPool::GetPrimitiveCount(void) const
{
	int size;

	switch (m_mode)
	{
	case POINTS:
		size = 1;
		break;
	case LINES:
		size = 2;
		break;
	case TRIANGLES:
		size = 3;
		break;
	case QUADS:
		size = 4;
		break;
	default:
		return -1;
	}

	return (int)m_elementArray.size()/size;
}

/**
*/
void CVertexPool::Clear(void)
{
	m_labelArray.clear();

	m_vertexArray.clear();
	m_colorArray.clear();
	m_normalArray.clear();
	m_texCoordArray.clear();

	m_elementArray.clear();
}

/**
*/
bool CVertexPool::Draw(void) const
{
	if (!m_colorArray.empty())
	{
		glColorPointer(m_colorSize, GL_FLOAT, 0, &m_colorArray[0]);

		glEnableClientState(GL_COLOR_ARRAY);
	}

	if (!m_normalArray.empty())
	{
		glNormalPointer(GL_FLOAT, 0, &m_normalArray[0]);

		glEnableClientState(GL_NORMAL_ARRAY);
	}

	if (!m_texCoordArray.empty())
	{
		glTexCoordPointer(m_texCoordSize, GL_FLOAT, 0, &m_texCoordArray[0]);

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	if (!m_vertexArray.empty())
	{
		glVertexPointer(m_vertexSize, GL_FLOAT, 0, &m_vertexArray[0]);

		glEnableClientState(GL_VERTEX_ARRAY);
	}

	if (!m_elementArray.empty())
		glDrawElements(m_mode, (GLsizei)m_elementArray.size(), GL_UNSIGNED_INT, &m_elementArray[0]);

	if (!m_colorArray.empty())
		glDisableClientState(GL_COLOR_ARRAY);

	if (!m_normalArray.empty())
		glDisableClientState(GL_NORMAL_ARRAY);

	if (!m_texCoordArray.empty())
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	if (!m_vertexArray.empty())
		glDisableClientState(GL_VERTEX_ARRAY);

	return true;
}

/**
*/
bool CVertexPool::AppendVertexPool(const CVertexPool *vertexPool)
{
	int vertexCount;
	vector<unsigned int>::const_iterator element;

	if (m_mode != vertexPool->m_mode)
	{
		MarkError();

		return false;
	}

	if (!vertexPool->m_labelArray.empty())
		m_labelArray.insert(m_labelArray.end(), vertexPool->m_labelArray.begin(), vertexPool->m_labelArray.end());

	if (m_colorSize != 0)
		if (m_colorSize != vertexPool->m_colorSize)
		{
			MarkError();

			return false;
		}

		m_colorSize = vertexPool->m_colorSize;

		if (!vertexPool->m_colorArray.empty())
			m_colorArray.insert(m_colorArray.end(), vertexPool->m_colorArray.begin(), vertexPool->m_colorArray.end());

		if (m_texCoordSize != 0)
			if (m_texCoordSize != vertexPool->m_texCoordSize)
			{
				MarkError();

				return false;
			}

			if (!vertexPool->m_texCoordArray.empty())
				m_texCoordArray.insert(m_texCoordArray.end(), vertexPool->m_texCoordArray.begin(), vertexPool->m_texCoordArray.end());

			m_texCoordSize = vertexPool->m_texCoordSize;

			if (!vertexPool->m_normalArray.empty())
				m_normalArray.insert(m_normalArray.end(), vertexPool->m_normalArray.begin(), vertexPool->m_normalArray.end());

			if (m_vertexSize != 0)
				if (m_vertexSize != vertexPool->m_vertexSize)
				{
					MarkError();

					return false;
				}

				m_vertexSize = vertexPool->m_vertexSize;

				vertexCount = GetVertexCount();

				if (!vertexPool->m_vertexArray.empty())
					m_vertexArray.insert(m_vertexArray.end(), vertexPool->m_vertexArray.begin(), vertexPool->m_vertexArray.end());

				for (element=vertexPool->m_elementArray.begin(); element!=vertexPool->m_elementArray.end(); element++)
					m_elementArray.push_back(vertexCount + *element);

				if (!IsValid())
				{
					MarkError();

					return false;
				}

				return true;
}

/**
*/
bool CVertexPool::Open(const string fileName, STREAM_FORMAT type)
{
	switch (type) 
	{
	case OFF:
		if (!OpenOFF(fileName))
		{
			MarkError();
			
			return false;
		}
		break;
	case STEVEN_OFF:
		if (!OpenStevenOFF(fileName))
		{
			MarkError();
			
			return false;
		}
		break;
	case PLY:
	case ASCII_STL:
	case SURFEL:
		MarkError();
		
		return false;
	case CRYSTAL_STRUCTURE_DISLOCATIONS:
		if (!OpenCrystalStructureDislocation(fileName))
		{
			MarkError();
			
			return false;
		}
		break;
	}

	return true;
}

/**
*/
bool CVertexPool::Save(const string fileName, STREAM_FORMAT type) const
{
	ofstream modelFile;
	const float *vertex,
		*color;
	const unsigned int *element;
	int vertexCount,
		elementCount,
		primitiveSize,
		primitiveCount,
		i;

	if (m_vertexArray.empty())
	{
		MarkError();

		return false;
	}

	vertex = &m_vertexArray[0];
	vertexCount = (int)m_vertexArray.size()/m_vertexSize;

	if (m_elementArray.empty())
	{
		MarkError();

		return false;
	}

	element = &m_elementArray[0];
	elementCount = (int)m_elementArray.size();

	if (!m_colorArray.empty())
		color = &m_colorArray[0];
	else
		color = 0;

	primitiveSize = 1;

	switch (m_mode) {
  case GL_POINTS:
	  primitiveSize = 1;
	  break;
  case GL_LINES:
	  primitiveSize = 2;
	  break;
  case GL_TRIANGLES:
	  primitiveSize = 3;
	  break;
  case GL_QUADS:
	  primitiveSize = 4;
	  break;
  case GL_LINE_LOOP:
  case GL_LINE_STRIP:
  case GL_TRIANGLE_STRIP:
  case GL_TRIANGLE_FAN:
  case GL_QUAD_STRIP:
  case GL_POLYGON:
	  MarkError();

	  return false;
	};

	primitiveCount = elementCount/primitiveSize;

	switch (type) {
  case OFF:
	  modelFile.open(fileName.c_str());

	  modelFile << "OFF" << endl;
	  modelFile << vertexCount << " " << primitiveCount << " " << primitiveCount*primitiveSize << endl;

	  for ( ; vertexCount--; )
	  {
		  for (i=0; i!=m_vertexSize; i++)
			  modelFile << *vertex++ << " ";

		  modelFile << endl;
	  }

	  for ( ; primitiveCount--; )
	  {
		  modelFile << primitiveSize << " ";

		  for (i=primitiveSize; i--; )
			  modelFile << *element++ << " ";

		  modelFile << endl;
	  }
	  break;
  case STEVEN_OFF:
	  if (!color)
	  {
		  MarkError();

		  return false;
	  }

	  modelFile.open(fileName.c_str());

	  modelFile << vertexCount << " " << primitiveCount << endl;

	  for ( ; vertexCount--; )
	  {
		  for (i=0; i!=m_vertexSize; i++)
			  modelFile << *vertex++ << " ";

		  modelFile << (*color) << " ";

		  color += m_colorSize;

		  modelFile << endl;
	  }

	  for ( ; primitiveCount--; )
	  {
		  for (i=primitiveSize; i--; )
			  modelFile << *element++ << " ";

		  modelFile << endl;
	  }
	  break;
  case PLY:
	  modelFile.open(fileName.c_str());

	  modelFile << "ply" << endl;
	  modelFile << "format ascii 1.0" << endl;

	  modelFile << "element vertex " << vertexCount << endl;
	  if (m_vertexSize >= 1)
		  modelFile << "property float32 x" << endl;
	  if (m_vertexSize >= 2)
		  modelFile << "property float32 y" << endl;
	  if (m_vertexSize >= 3)
		  modelFile << "property float32 z" << endl;
	  if (m_vertexSize >= 4)
		  modelFile << "property float32 w" << endl;

	  modelFile << "element face " << primitiveCount << endl;
	  modelFile << "property list uint8 int32 vertex_indices" << endl;

	  modelFile << "end_header" << endl;

	  for ( ; vertexCount--; )
	  {
		  for (i=0; i!=m_vertexSize; i++)
			  modelFile << *vertex++ << " ";

		  modelFile << endl;
	  }

	  for ( ; primitiveCount--; )
	  {
		  modelFile << primitiveSize << " ";

		  for (i=primitiveSize; i--; )
			  modelFile << *element++ << " ";

		  modelFile << endl;
	  }
	  break;
  case ASCII_STL:
	  if (!SaveToASCIISTLFile(fileName))
	  {
		  MarkError();

		  return false;
	  }
	  break;
  case SURFEL:
	  if (!SaveToSURFELFile(fileName))
	  {
		  MarkError();

		  return false;
	  }
	  break;
	};

	return true;
}

/**
*/
bool CVertexPool::IsValid(void) const
{
	int colorCount,
		texCoordCount,
		normalCount,	
		vertexCount,
		elementCount;
	const unsigned int *element;

	colorCount = (int)m_colorArray.size();

	if (colorCount)
	{
		if (!m_colorSize)
			return false;

		colorCount /= m_colorSize;
	}

	texCoordCount = (int)m_texCoordArray.size();

	if (texCoordCount)
	{
		if (!m_texCoordSize)
			return false;

		texCoordCount /= m_texCoordSize;
	}

	normalCount = (int)m_normalArray.size()/3;

	vertexCount = (int)m_vertexArray.size();

	if (vertexCount)
	{
		if (!m_vertexSize)
			return false;

		vertexCount /= m_vertexSize;

		if (colorCount)
			if (colorCount != vertexCount)
				return false;

		if (normalCount)
			if (normalCount != vertexCount)
				return false;

		if (texCoordCount)
			if (texCoordCount != vertexCount)
				return false;
	}

	elementCount = (int)m_elementArray.size();

	if (elementCount)
	{
		element = &m_elementArray[0];

		for ( ; elementCount--; element++)
			if (*element > (unsigned int)vertexCount)
				return false;
	}

	return true;
}

/**
*/
bool CVertexPool::SaveToASCIISTLFile(const std::string fileName) const
{
	ofstream modelFileStream;
	// Pointers to triangle vertices.
	const float *firstVertex,
		*secondVertex,
		*thirdVertex;
	const unsigned int *element;
	int elementCount,
		primitiveCount,
		i;
	// Auxiliary vectors used to calculate the triangle normal.
	float u[3],
		v[3],
		normal[3];

	if (m_vertexArray.empty())
	{
		MarkError();

		return false;
	}

	if (m_vertexSize != 3)
	{
		MarkError();

		return false;
	}

	if (m_elementArray.empty())
	{
		MarkError();

		return false;
	}

	element = &m_elementArray[0];
	elementCount = (int)m_elementArray.size();

	if (m_mode != GL_TRIANGLES)
	{
		MarkError();

		return false;
	}

	primitiveCount = elementCount/3;

	modelFileStream.open(fileName.c_str());

	if (!modelFileStream.is_open())
	{
		MarkError();

		return false;
	}

	modelFileStream << "solid " << fileName.substr(0, fileName.size() - 4) << endl;

	for ( ; primitiveCount--; )
	{
		firstVertex = GetVertex(element[0]);
		secondVertex = GetVertex(element[1]);
		thirdVertex = GetVertex(element[2]);

		for (i=0; i<3; i++)
		{
			u[i] = secondVertex[i] - firstVertex[i];
			v[i] = thirdVertex[i] - firstVertex[i];
		}

		MyMath::CrossProduct<float>(u, v, normal);
		MyMath::Normalize<float, 3>(normal);

		modelFileStream << "facet normal " << normal[0] << " " << normal[1] << " " << normal[2] << endl;

		modelFileStream << "  outer loop" << endl;

		modelFileStream << "    vertex   " << firstVertex[0] << " " << firstVertex[1] << " " << firstVertex[2] << endl;
		modelFileStream << "    vertex   " << secondVertex[0] << " " << secondVertex[1] << " " << secondVertex[2] << endl;
		modelFileStream << "    vertex   " << thirdVertex[0] << " " << thirdVertex[1] << " " << thirdVertex[2] << endl;

		modelFileStream << "  endloop" << endl;

		modelFileStream << "endfacet" << endl;

		element += 3;
	}

	modelFileStream << "endsolid" << endl;

	return true;
}

/**
*/
bool CVertexPool::SaveToSURFELFile(const std::string fileName) const
{
	ofstream modelFileStream;
	const float *vertexCoord,
		*normalCoord,
		*colorCoord;
	int primitiveCount,
		i;

	if (m_vertexArray.empty())
	{
		MarkError();

		return false;
	}

	if (m_vertexSize != 3)
	{
		MarkError();

		return false;
	}

	vertexCoord = &m_vertexArray[0];

	if (m_normalArray.empty())
	{
		MarkError();

		return false;
	}

	normalCoord = &m_normalArray[0];

	if (m_colorArray.empty())
		colorCoord = 0;
	else
		colorCoord = &m_colorArray[0];

	primitiveCount = GetVertexCount();

	modelFileStream.open(fileName.c_str());

	if (!modelFileStream.is_open())
	{
		MarkError();

		return false;
	}

	modelFileStream << primitiveCount << endl;

	for (i=0; i<primitiveCount; i++)
	{
		modelFileStream << vertexCoord[0] << " " << vertexCoord[1] << " " << vertexCoord[2];

		modelFileStream << " ";

		if (colorCoord != 0)
			modelFileStream << colorCoord[0] << " " << colorCoord[1] << " " << colorCoord[2];
		else
			modelFileStream << "1.0 1.0 1.0";

		modelFileStream << " ";

		modelFileStream << normalCoord[0] << " " << normalCoord[1] << " " << normalCoord[2];

		modelFileStream << " ";

		modelFileStream << "1.0" << endl;

		vertexCoord += m_vertexSize;

		if (colorCoord != 0)
			colorCoord += m_colorSize;

		normalCoord += 3;
	}

	return true;
}

/**
*/
bool CVertexPool::OpenOFF(const string fileName)
{
	FILE *fileStream;
	char token[128];
	int fileStatus,
		vertexCount,
		cellCount,
		dummyInt[3],
		i,
		j;
	float dummyFloat[3];

	if ((fileStream = fopen(fileName.c_str(), "r")) == NULL)
	{
		MarkError();

		return false;
	}

	fileStatus = fscanf(fileStream, "%s", token);

	if ((fileStatus == EOF) || (fileStatus == 0))
	{
		MarkError();

		return false;
	}

	if (strcmp(token, "OFF") != 0)
	{
		MarkError();

		return false;
	}

	fileStatus = fscanf(fileStream, "%i", &vertexCount);

	if ((fileStatus == EOF) || (fileStatus == 0))
	{
		MarkError();

		return false;
	}

	fileStatus = fscanf(fileStream, "%i", &cellCount);

	if ((fileStatus == EOF) || (fileStatus == 0))
	{
		MarkError();

		return false;
	}

	fileStatus = fscanf(fileStream, "%i", dummyInt);

	if ((fileStatus == EOF) || (fileStatus == 0))
	{
		MarkError();

		return false;
	}

	Clear();

	for (i=0; i!=vertexCount; i++)
	{
		for (j=0; j!=3; j++)
		{
			fileStatus = fscanf(fileStream, "%f", dummyFloat + j);

			if ((fileStatus == EOF) || (fileStatus == 0))
			{
				MarkError();

				return false;
			}
		}

		AddVertex(dummyFloat, dummyFloat + 3);
	}

	for (i=0; i!=cellCount; i++)
	{
		fileStatus = fscanf(fileStream, "%i", dummyInt);

		if ((fileStatus == EOF) || (fileStatus == 0))
		{
			MarkError();

			return false;
		}

		if (dummyInt[0] != 3)
		{
			MarkError();

			return false;
		}

		for (j=0; j!=3; j++)
		{
			fileStatus = fscanf(fileStream, "%i", dummyInt + j);

			if ((fileStatus == EOF) || (fileStatus == 0))
			{
				MarkError();

				return false;
			}
		}

		AddTriangle(dummyInt[0], dummyInt[1], dummyInt[2]);
	}

	fclose(fileStream);

	return true;
}

/**
*/
bool CVertexPool::OpenStevenOFF(const string fileName)
{
	FILE *fileStream;
	int fileStatus,
		vertexCount,
		cellCount,
		i,
		j;
	unsigned int dummyInt[4];		
	float dummyFloat[4];

	if ((fileStream = fopen(fileName.c_str(), "r")) == NULL)
	{
		MarkError();

		return false;
	}

	fileStatus = fscanf(fileStream, "%i", &vertexCount);

	if ((fileStatus == EOF) || (fileStatus == 0))
	{
		MarkError();

		return false;
	}

	fileStatus = fscanf(fileStream, "%i", &cellCount);

	if ((fileStatus == EOF) || (fileStatus == 0))
	{
		MarkError();

		return false;
	}

	Clear();

	for (i=0; i<vertexCount; ++i)
	{
		for (j=0; j<4; ++j)
		{
			fileStatus = fscanf(fileStream, "%f", dummyFloat + j);

			if ((fileStatus == EOF) || (fileStatus == 0))
			{
				MarkError();

				return false;
			}
		}

		AddVertex(dummyFloat, dummyFloat + 3);
		
		AddColor(*(dummyFloat + 3), *(dummyFloat + 3), *(dummyFloat + 3), *(dummyFloat + 3));
	}

	for (i=0; i<cellCount; ++i)
	{
		for (j=0; j<3; ++j)
		{
			fileStatus = fscanf(fileStream, "%i", dummyInt + j);

			if ((fileStatus == EOF) || (fileStatus == 0))
			{
				MarkError();

				return false;
			}
		}

		AddElementArray(dummyInt, dummyInt + 4);
	}

	fclose(fileStream);

	return true;
}

/**
*/
bool CVertexPool::OpenCrystalStructureDislocation(const string fileName)
{
	FILE *fileStream;
	char token[128];
	int fileStatus,
		lineSizeCount,
		dummyInt,
		j;
	float dummyFloat[3],
		texCoord[3],
		vertexCoord[3];

	if ((fileStream = fopen(fileName.c_str(), "r")) == NULL)
	{
		MarkError();

		return false;
	}

	// Ignores the header of the file.
	do 
	{
		fileStatus = fscanf(fileStream, "%s", token);
		
		if ((fileStatus == EOF) || (fileStatus == 0))
		{
			MarkError();
			
			return false;
		}

		// Searches for the end of the line.
		if (token[0] == '#')
		{
			lineSizeCount = 0;

			while (fgetc(fileStream) != '\n')
				if (++lineSizeCount > 2048)
				{
					MarkError();

					return false;
				}
		}
		else
			break;
	} while ((fileStatus != EOF) && (fileStatus != 0));

	dummyFloat[0] = float(atof(token));

	Clear();

	SetMode(POINTS);

	while ((fileStatus != EOF) && (fileStatus != 0))
	{
		// number (readed from file in the previous iteration), type and mass.
		for (j=1; j!=3; ++j)
		{
			fileStatus = fscanf(fileStream, "%f", texCoord + j);

			if ((fileStatus == EOF) || (fileStatus == 0))
			{
				MarkError();

				return false;
			}
		}

		// x, y and z.
		for (j=0; j!=3; ++j)
		{
			fileStatus = fscanf(fileStream, "%f", vertexCoord + j);

			if ((fileStatus == EOF) || (fileStatus == 0))
			{
				MarkError();

				return false;
			}
		}

		// vx, vy and vz.
		for (j=0; j!=3; ++j)
		{
			fileStatus = fscanf(fileStream, "%f", dummyFloat + j);

			if ((fileStatus == EOF) || (fileStatus == 0))
			{
				MarkError();

				return false;
			}
		}

		// Crist.
		fileStatus = fscanf(fileStream, "%i", &dummyInt);

		if ((fileStatus == EOF) || (fileStatus == 0))
		{
			MarkError();

			return false;
		}

		// Discards red points (points along planes).
		if (dummyInt != 1)
		{
			AddLabel(dummyInt);

			AddTexCoord(texCoord[0], texCoord[1], texCoord[2]);

			AddVertex(vertexCoord, vertexCoord + 3);

			// Color computed from crist.
			AddColor(float(dummyInt&1), float((dummyInt&2)>>1), float((dummyInt&4)>>2));

			AddElement(GetElementCount());
		}

		fileStatus = fscanf(fileStream, "%f", dummyFloat);
	}

	fclose(fileStream);

	return true;
}

/**
*/
void CVertexPool::Create(void)
{
	m_labelArray.clear();

	m_vertexSize = 0;
	m_vertexArray.clear();

	m_colorSize = 0;
	m_colorArray.clear();

	m_normalArray.clear();

	m_texCoordSize = 0;
	m_texCoordArray.clear();

	m_mode = GL_POINTS;

	m_elementArray.clear();
}

void CVertexPool::RemoveVertex3D(void)
{ 
	if (m_vertexArray.size()>= 3)
	{
		m_vertexArray.pop_back();
		m_vertexArray.pop_back();
		m_vertexArray.pop_back();
	}
	else
		MarkError();
}

void CVertexPool::RemoveElement()
{
	m_elementArray.pop_back();
}