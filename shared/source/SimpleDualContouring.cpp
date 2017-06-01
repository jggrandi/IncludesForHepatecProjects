#include <ErrorManager.h>
#include <SimpleDualContouring.h>

using namespace std;

const int CSimpleDualContouring::m_edgeTable[256] = {
	0x0  , 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
	0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
	0x190, 0x99 , 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
	0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
	0x230, 0x339, 0x33 , 0x13a, 0x636, 0x73f, 0x435, 0x53c,
	0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
	0x3a0, 0x2a9, 0x1a3, 0xaa , 0x7a6, 0x6af, 0x5a5, 0x4ac,
	0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
	0x460, 0x569, 0x663, 0x76a, 0x66 , 0x16f, 0x265, 0x36c,
	0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
	0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff , 0x3f5, 0x2fc,
	0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
	0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55 , 0x15c,
	0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
	0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc ,
	0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
	0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
	0xcc , 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
	0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
	0x15c, 0x55 , 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
	0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
	0x2fc, 0x3f5, 0xff , 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
	0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
	0x36c, 0x265, 0x16f, 0x66 , 0x76a, 0x663, 0x569, 0x460,
	0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
	0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa , 0x1a3, 0x2a9, 0x3a0,
	0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
	0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33 , 0x339, 0x230,
	0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
	0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99 , 0x190,
	0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
	0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0
};

/**
*/
CSimpleDualContouring::CSimpleDualContouring(void)
{
	// Move to a new 'Create' function?
	SetupVertexPool();
}

/**
*/
CSimpleDualContouring::~CSimpleDualContouring(void)
{
	DeleteMesh();
}

/**
*/
bool CSimpleDualContouring::SetTransform(EDGE_TRANSFORM edgeTransform)
{
	MarkError();

	return false;
}

/**
*/
bool CSimpleDualContouring::SetCellSize(float width, float height, float depth)
{
	MarkError();

	return false;
}

/**
*/
bool CSimpleDualContouring::SetGridSize(int width, int height, int depth)
{
	MarkError();

	return false;
}

/**
*/
bool CSimpleDualContouring::GenerateMesh(float isoLevel)
{
	int width,
		height, 
		depth,
		cellType, 
		edgeType,
		lineSize,
		sliceSize,
		index,
		x, 
		y, 
		z;
	const float *valueArray;
	float valueCache[4];

	if (!m_image.get())
	{
		MarkError();

		return false;
	}

	if (m_image->GetType() != CGageAdaptor::FLOAT)
	{
		MarkError();

		return false;
	}

	DeleteMesh();

	m_isoLevel = isoLevel;

	width  = m_image->GetWidth();
	height = m_image->GetHeight();
	depth  = m_image->GetDepth();

	valueArray = m_image->GetValueArray();

	lineSize = width;
	sliceSize = lineSize*height;

	for (z=1; z!=(depth - 1); z++)
	{
		for (y=1; y!=(height - 1); y++)
		{
			index = z*sliceSize + y*lineSize + 1;

			valueCache[0] = valueArray[index + lineSize];
			valueCache[1] = valueArray[index];
			valueCache[2] = valueArray[index + sliceSize + lineSize];
			valueCache[3] = valueArray[index + sliceSize];

			for (x=1; x!=(width - 1); x++) 
			{
				index = z*sliceSize + y*lineSize + x;

				cellType = 0;

				if (valueCache[1] < isoLevel)
					cellType |= 1;
				if (valueCache[0] < isoLevel)
					cellType |= 2;

				valueCache[0] = valueArray[index + lineSize + 1];

				if (valueCache[0] < isoLevel)
					cellType |= 4;

				valueCache[1] = valueArray[index + 1];

				if (valueCache[1] < isoLevel)
					cellType |= 8;
				if (valueCache[3] < isoLevel)
					cellType |= 16;
				if (valueCache[2] < isoLevel)
					cellType |= 32;

				valueCache[2] = valueArray[index + sliceSize + lineSize + 1];

				if (valueCache[2] < isoLevel)
					cellType |= 64;

				valueCache[3] = valueArray[index + sliceSize + 1];

				if (valueCache[3] < isoLevel)
					cellType |= 128;

				edgeType = m_edgeTable[cellType];

				if (edgeType != 0) 
				{
					m_cellToVertexMap[GetVertexId(x, y, z)] = (unsigned int)m_activeCellArray.size()/3;

					m_activeCellArray.push_back(x);
					m_activeCellArray.push_back(y);
					m_activeCellArray.push_back(z);

					// Edge connecting the vertices 1 and 0.
					if ((cellType&1) != ((cellType>>1)&1))
					{
						if (x > 1 && z > 1)
						{
							if ((cellType>>1)&1)
							{
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z - 1));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z));
#ifndef MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z));
#endif // !MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z - 1));
#ifndef MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z - 1));
#endif // !MY_ENABLE_QUAD_MESH
							}
							else 
							{
#ifndef MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z - 1));
#endif // !MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z));
#ifndef MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z));
#endif // !MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z - 1));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z - 1));
							}
						}
					}
					
					// Edge connecting the vertices 1 and 3.
					if ((cellType&1) != ((cellType>>4)&1))
					{
						if (x > 1 && y > 1)
						{
							if ((cellType>>4)&1)
							{
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z));
#ifndef MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z));
#endif // !MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y - 1, z));
#ifndef MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z));
#endif // !MY_ENABLE_QUAD_MESH
							}
							else
							{
#ifndef MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z));
#endif // !MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z));
#ifndef MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z));
#endif // !MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y - 1, z));
							}
						}
					}
					
					// Edge connecting the vertices 1 and 5.
					if ((cellType&1) != ((cellType>>3)&1))
					{
						if (y > 1 && z > 1)
						{
							if ((cellType>>3)&1)
							{
#ifndef MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z));
#endif // !MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z - 1));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z));
#ifndef MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z - 1));
#endif // !MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z - 1));
							}
							else 
							{
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z - 1));
#ifndef MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z - 1));
#endif // !MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z - 1));
#ifndef MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z));
#endif // !MY_ENABLE_QUAD_MESH
							}
						}
					}

					if (edgeType & 8)
						m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 3), GenerateVertex(x, y, z, 3)));
					if (edgeType & 1)
						m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 0), GenerateVertex(x, y, z, 0)));
					if (edgeType & 256)
						m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 8), GenerateVertex(x, y, z, 8)));

					if (x == (width - 2)) 
					{
						if (edgeType & 4) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 2), GenerateVertex(x, y, z, 2)));
						if (edgeType & 2048) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 11), GenerateVertex(x, y, z, 11)));
					}
					if (y == (height - 2)) 
					{
						if (edgeType & 2) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 1), GenerateVertex(x, y, z, 1)));
						if (edgeType & 512) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 9), GenerateVertex(x, y, z, 9)));
					}
					if (z == (depth - 2)) 
					{
						if (edgeType & 16) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 4), GenerateVertex(x, y, z, 4)));
						if (edgeType & 128) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 7), GenerateVertex(x, y, z, 7)));
					}

					if ((x == (width - 2)) && (y == (height - 2)))
					{
						if (edgeType & 1024) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 10), GenerateVertex(x, y, z, 10)));
					}
					if ((x == (width - 2)) && (z == (depth - 2)))
					{
						if (edgeType & 64) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 6), GenerateVertex(x, y, z, 6)));
					}
					if ((y == (height - 2)) && (z == (depth - 2)))
					{
						if (edgeType & 32) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 5), GenerateVertex(x, y, z, 5)));
					}
				}
			}
		}
	}

	return GenerateDual();
}

/**
*/
void CSimpleDualContouring::SetupVertexPool(void)
{
	m_triangleMesh->m_vertexSize = 3;

#ifdef MY_ENABLE_QUAD_MESH
	m_triangleMesh->m_mode = CVertexPool::QUADS;
#else // !MY_ENABLE_QUAD_MESH
	m_triangleMesh->m_mode = CVertexPool::TRIANGLES;
#endif // !MY_ENABLE_QUAD_MESH

	m_activeEdges->m_vertexSize = 3;

	m_activeEdges->m_mode = CVertexPool::LINES;
}

/**
*/
int CSimpleDualContouring::GenerateVertex(int x, int y, int z, int edgeName)
{
	float orgCoord[3],
		destCoord[3];

	orgCoord[0] = destCoord[0] = (float)x;
	orgCoord[1] = destCoord[1] = (float)y;
	orgCoord[2] = destCoord[2] = (float)z;

	switch (edgeName)
	{
	case 0:
		destCoord[1] += 1;
		break;
	case 1:
		orgCoord[1] += 1;
		destCoord[0] += 1;
		destCoord[1] += 1;
		break;
	case 2:
		orgCoord[0] += 1;
		orgCoord[1] += 1;
		destCoord[0] += 1;
		break;
	case 3:
		orgCoord[0] += 1;
		break;
	case 4:
		orgCoord[2] += 1;
		destCoord[1] += 1;
		destCoord[2] += 1;
		break;
	case 5:
		orgCoord[1] += 1;
		orgCoord[2] += 1;
		destCoord[0] += 1;
		destCoord[1] += 1;
		destCoord[2] += 1;
		break;
	case 6:
		orgCoord[0] += 1;
		orgCoord[1] += 1;
		orgCoord[2] += 1;
		destCoord[0] += 1;
		destCoord[2] += 1;
		break;
	case 7:
		orgCoord[0] += 1;
		orgCoord[2] += 1;
		destCoord[2] += 1;
		break;
	case 8:
		destCoord[2] += 1;
		break;
	case 9:
		orgCoord[1] += 1;
		destCoord[1] += 1;
		destCoord[2] += 1;
		break;
	case 10:
		orgCoord[0] += 1;
		orgCoord[1] += 1;
		destCoord[0] += 1;
		destCoord[1] += 1;
		destCoord[2] += 1;
		break;
	case 11:
		orgCoord[0] += 1;
		destCoord[0] += 1;
		destCoord[2] += 1;
		break;
	};

	return FindIntersection(orgCoord, destCoord);
}

/**
*/
int CSimpleDualContouring::FindIntersection(float *orgCoord, float *destCoord)
{
	float mu,
		orgValue,
		destValue,
		vertexCoord[3];
	const float *vertexNormal;
	int vertexIndex,
		width,
		height, 
		lineSize,
		sliceSize,
		i;
	const float *valueArray;

	width  = m_image->GetWidth();
	height = m_image->GetHeight();

	lineSize = width;
	sliceSize = lineSize*height;

	valueArray = m_image->GetValueArray();

	orgValue = valueArray[(int)orgCoord[2]*sliceSize + (int)orgCoord[1]*lineSize + (int)orgCoord[0]];
	destValue = valueArray[(int)destCoord[2]*sliceSize + (int)destCoord[1]*lineSize + (int)destCoord[0]];

	mu = (m_isoLevel - orgValue)/(destValue - orgValue);

	if ((mu < 0.0f) || (mu > 1.0f))
		MarkError();

	for (i=3; i--; )
		vertexCoord[i] = (float)orgCoord[i] + mu*(destCoord[i] - orgCoord[i]);

	vertexIndex = (int)m_triangleMesh->m_vertexArray.size()/3;

	m_triangleMesh->m_vertexArray.push_back(vertexCoord[0]);
	m_triangleMesh->m_vertexArray.push_back(vertexCoord[1]);
	m_triangleMesh->m_vertexArray.push_back(vertexCoord[2]);

	vertexNormal = m_image->GetNormal(vertexCoord[0], vertexCoord[1], vertexCoord[2]);

	m_triangleMesh->m_normalArray.push_back(-vertexNormal[0]);
	m_triangleMesh->m_normalArray.push_back(-vertexNormal[1]);
	m_triangleMesh->m_normalArray.push_back(-vertexNormal[2]);

	m_activeEdges->m_vertexArray.push_back((float)orgCoord[0]);
	m_activeEdges->m_vertexArray.push_back((float)orgCoord[1]);
	m_activeEdges->m_vertexArray.push_back((float)orgCoord[2]);
	m_activeEdges->m_vertexArray.push_back((float)destCoord[0]);
	m_activeEdges->m_vertexArray.push_back((float)destCoord[1]);
	m_activeEdges->m_vertexArray.push_back((float)destCoord[2]);

	m_activeEdges->m_elementArray.push_back((unsigned int)m_activeEdges->m_elementArray.size());
	m_activeEdges->m_elementArray.push_back((unsigned int)m_activeEdges->m_elementArray.size());

	return vertexIndex;
}

/**
*/
int CSimpleDualContouring::GetEdgeId(int x, int y, int z, int edgeName) const
{
	switch (edgeName) 
	{
	case 0:
		return GetVertexId(x, y, z) + 1;
	case 1:
		return GetVertexId(x, y + 1, z);
	case 2:
		return GetVertexId(x + 1, y, z) + 1;
	case 3:
		return GetVertexId(x, y, z);
	case 4:
		return GetVertexId(x, y, z + 1) + 1;
	case 5:
		return GetVertexId(x, y + 1, z + 1);
	case 6:
		return GetVertexId(x + 1, y, z + 1) + 1;
	case 7:
		return GetVertexId(x, y, z + 1);
	case 8:
		return GetVertexId(x, y, z) + 2;
	case 9:
		return GetVertexId(x, y + 1, z) + 2;
	case 10:
		return GetVertexId(x + 1, y + 1, z) + 2;
	case 11:
		return GetVertexId(x + 1, y, z) + 2;
	default:
		return 0;
	}
}

/**
*/
int CSimpleDualContouring::GetVertexId(int x, int y, int z) const
{
	return 3*(m_image->GetWidth()*(z*m_image->GetHeight() + y) + x);
}

/**
*/
bool CSimpleDualContouring::GenerateDual(void)
{
	vector<unsigned int>::iterator activeCell;
	int edgeId,
		vertexId,
		i;
	float dualVertex[3];
	const float *dualNormal;
	vector<float> dualVertexArray,
		dualNormalArray;

	if (m_activeCellArray.empty())
	{
		MarkError();

		return false;
	}

	for (activeCell=m_activeCellArray.begin(); activeCell!=m_activeCellArray.end(); activeCell+=3)
	{
		ResetDualVertexSearch();

		// For each of the 12 edges of the cell, ...
		for (i=12; i--; )
		{
			// ... get the edge name...
			edgeId = GetEdgeId(*activeCell, *(activeCell + 1), *(activeCell + 2), i);

			if (m_vertexToEdgeMap.find(edgeId) != m_vertexToEdgeMap.end())
			{
				// ... and the name of the vertex associated to the edge.
				vertexId = m_vertexToEdgeMap[edgeId];

				AddEdgeIntersectionToDualVertexSearch(*activeCell, *(activeCell + 1), *(activeCell + 2), &m_triangleMesh->m_vertexArray[3*vertexId], &m_triangleMesh->m_normalArray[3*vertexId]);
			}
		}

		if (!FindDualVertex(*activeCell, *(activeCell + 1), *(activeCell + 2), dualVertex))
		{
			MarkError();

			return false;
		}

		dualNormal = m_image->GetNormal(dualVertex[0], dualVertex[1], dualVertex[2]);

		dualNormalArray.push_back(dualNormal[0]);
		dualNormalArray.push_back(dualNormal[1]);
		dualNormalArray.push_back(dualNormal[2]);

		dualVertexArray.insert(dualVertexArray.end(), dualVertex, dualVertex + 3);
	}

	RenameElements();

	m_triangleMesh->m_vertexArray.assign(dualVertexArray.begin(), dualVertexArray.end());
	m_triangleMesh->m_normalArray.assign(dualNormalArray.begin(), dualNormalArray.end());

	m_vertexToEdgeMap.clear();
	m_cellToVertexMap.clear();

	m_activeCellArray.clear();

	return true;
}

/**
*/
void CSimpleDualContouring::ResetDualVertexSearch(void)
{
	m_cellVertexArray.clear();
	m_cellGradientArray.assign(3*8, 0.0f);
}

/**
*/
void CSimpleDualContouring::AddEdgeIntersectionToDualVertexSearch(int x, int y, int z, float *position, float *normal)
{
	float vertex[4],
		planeCoeff[4],
		distance;
	int i,
		j;
	static float cellVertexArray[8][4] = {
		0.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};

	m_cellVertexArray.insert(m_cellVertexArray.end(), position, position + 3);

	vertex[0] = position[0] - x;
	vertex[1] = position[1] - y;
	vertex[2] = position[2] - z;

	vertex[3] = 1.0f;

	planeCoeff[0] = normal[0];
	planeCoeff[1] = normal[1];
	planeCoeff[2] = normal[2];
	planeCoeff[3] = -(vertex[0]*normal[0]) - (vertex[1]*normal[1]) - (vertex[2]*normal[2]);

	for (j=8; j--; )
	{
		distance = 0.0f;

		for (i=4; i--; )
			distance += planeCoeff[i]*cellVertexArray[j][i];

		for (i=3; i--; )
			m_cellGradientArray[3*j + i] += distance*normal[i];
	}
}

/**
*/
bool CSimpleDualContouring::FindDualVertex(int x, int y, int z, float *position)
{
	int vertexCount,
		i,
		j,
		k;
	float centroid[3],
		boundingBox[2][3],
		vertex[3],
		weightedGradientArray[4][3],
		tx,
		ty,
		tz,
		stepsize,
		lastSquaredNorma,
		squaredNorma;

	if (m_cellVertexArray.empty())
	{
		MarkError();

		return false;
	}

	for (i=3; i--; )
		centroid[i] = 0.0f;

	vertexCount = (int)m_cellVertexArray.size()/3;

	for (j=vertexCount; j--; )
		for (i=3; i--; )
			centroid[i] += m_cellVertexArray[3*j + i];

	for (i=3; i--; )
		centroid[i] /= vertexCount;

	// X, y, and z define the cell origin, instead of vertex coordinates.
	boundingBox[0][0] = (float)x;
	boundingBox[0][1] = (float)y;
	boundingBox[0][2] = (float)z;
	boundingBox[1][0] = boundingBox[0][0] + 1.0f;
	boundingBox[1][1] = boundingBox[0][1] + 1.0f;
	boundingBox[1][2] = boundingBox[0][2] + 1.0f;

	for (i=3; i--; )
		vertex[i] = centroid[i];

	for (i=3; i--; )
		if ((vertex[i] < boundingBox[0][i]) || (vertex[i] > boundingBox[1][i]))
			MarkError();

	// A weak attempt to set a adaptive stepsize.
	stepsize = 0.05f;

	lastSquaredNorma = 3;

	// Number of steps?
	for (k=8; k--; )
	{
		tx = vertex[0] - (float)floor(vertex[0]);

		for (i=3; i--; )
		{
			weightedGradientArray[0][i] = (1.0f - tx)*m_cellGradientArray[3*0 + i] + tx*m_cellGradientArray[3*4 + i];
			weightedGradientArray[1][i] = (1.0f - tx)*m_cellGradientArray[3*1 + i] + tx*m_cellGradientArray[3*5 + i];
			weightedGradientArray[2][i] = (1.0f - tx)*m_cellGradientArray[3*2 + i] + tx*m_cellGradientArray[3*6 + i];
			weightedGradientArray[3][i] = (1.0f - tx)*m_cellGradientArray[3*3 + i] + tx*m_cellGradientArray[3*7 + i];
		}

		ty = vertex[1] - (float)floor(vertex[1]);

		for (i=3; i--; )
		{
			weightedGradientArray[0][i] = (1.0f - ty)*weightedGradientArray[0][i] + ty*weightedGradientArray[2][i];
			weightedGradientArray[1][i] = (1.0f - ty)*weightedGradientArray[1][i] + ty*weightedGradientArray[3][i];
		}

		tz = vertex[2] - (float)floor(vertex[2]);

		squaredNorma = 0.0f;

		for (i=3; i--; )
		{
			weightedGradientArray[0][i] = (1.0f - tz)*weightedGradientArray[0][i] + tz*weightedGradientArray[1][i];

			squaredNorma += weightedGradientArray[0][i]*weightedGradientArray[0][i];
		}

		if (lastSquaredNorma < squaredNorma)
			stepsize *= 0.95f;
		else
			lastSquaredNorma = squaredNorma;

		for (i=3; i--; )
		{
			vertex[i] -= stepsize*weightedGradientArray[0][i];

			if (vertex[i] < boundingBox[0][i])
				vertex[i] = boundingBox[0][i];
			else if (vertex[i] > boundingBox[1][i])
				vertex[i] = boundingBox[1][i];
		}
	}

	position[0] = vertex[0];
	position[1] = vertex[1];
	position[2] = vertex[2];

	return true;
}

/**
*/
void CSimpleDualContouring::RenameElements(void)
{
	vector<unsigned int>::iterator element;

	for (element=m_triangleMesh->m_elementArray.begin(); element!=m_triangleMesh->m_elementArray.end(); element++)
		(*element) = m_cellToVertexMap[(*element)];

	m_cellToVertexMap.clear();
}

