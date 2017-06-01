#include <cmath>

#include <ErrorManager.h>
#include <MarchingTetrahedra.h>

using namespace std;

const int CMarchingTetrahedra::m_cubeSubdivisionTable[6][4] = {
	{0, 7, 2, 6},
	{0, 7, 6, 4},
	{0, 7, 4, 5},
	{0, 7, 5, 1},
	{0, 7, 1, 3},
	{0, 7, 3, 2}
};

const int CMarchingTetrahedra::m_triangleTable[6][16][7] = {
	// Tetrahedron 0.
	{{-1, -1, -1, -1, -1, -1, -1},  // 0000
	 { 1,  6,  5, -1, -1, -1, -1},  // 0001
	 { 6, 12, 17, -1, -1, -1, -1},  // 0010
	 { 1, 12,  5,  5, 12, 17, -1},  // 0011
	 { 1, 11, 12, -1, -1, -1, -1},  // 0100
	 { 5, 11,  6,  6, 11, 12, -1},  // 0101
	 { 1, 11,  6,  6, 11, 17, -1},  // 0110
	 { 5, 11, 17, -1, -1, -1, -1},  // 0111
	 { 5, 17, 11, -1, -1, -1, -1},  // 1000
	 { 1,  6, 11, 11,  6, 17, -1},  // 1001
	 { 5,  6, 11, 11,  6, 12, -1},  // 1010
	 { 1, 12, 11, -1, -1, -1, -1},  // 1011
	 { 1,  5, 12, 12,  5, 17, -1},  // 1100
	 { 6, 17, 12, -1, -1, -1, -1},  // 1101
	 { 1,  5,  6, -1, -1, -1, -1},  // 1110
	 {-1, -1, -1, -1, -1, -1, -1}}, // 1111
	// Tetrahedron 1.
	{{-1, -1, -1, -1, -1, -1, -1},  // 0000
	 { 3,  5,  6, -1, -1, -1, -1},  // 0001
	 { 6, 17, 16, -1, -1, -1, -1},  // 0010
	 { 3,  5, 16, 16,  5, 17, -1},  // 0011
	 { 5, 15, 17, -1, -1, -1, -1},  // 0100
	 { 3, 15,  6,  6, 15, 17, -1},  // 0101
	 { 5, 15,  6,  6, 15, 16, -1},  // 0110
	 { 3, 15, 16, -1, -1, -1, -1},  // 0111
	 { 3, 16, 15, -1, -1, -1, -1},  // 1000
	 { 5,  6, 15, 15,  6, 16, -1},  // 1001
	 { 3,  6, 15, 15,  6, 17, -1},  // 1010
	 { 5, 17, 15, -1, -1, -1, -1},  // 1011
	 { 3, 16,  5,  5, 16, 17, -1},  // 1100
	 { 6, 16, 17, -1, -1, -1, -1},  // 1101
	 { 3,  6,  5, -1, -1, -1, -1},  // 1110
	 {-1, -1, -1, -1, -1, -1, -1}}, // 1111				 
	// Tetrahedron 2.									 
	{{-1, -1, -1, -1, -1, -1, -1},  // 0000
	 { 3,  6,  4, -1, -1, -1, -1},  // 0001
	 { 6, 16, 18, -1, -1, -1, -1},  // 0010
	 { 3, 16,  4,  4, 16, 18, -1},  // 0011
	 { 3, 14, 16, -1, -1, -1, -1},  // 0100
	 { 4, 14,  6,  6, 14, 16, -1},  // 0101
	 { 3, 14,  6,  6, 14, 18, -1},  // 0110
	 { 4, 14, 18, -1, -1, -1, -1},  // 0111
	 { 4, 18, 14, -1, -1, -1, -1},  // 1000
	 { 3,  6, 14, 14,  6, 18, -1},  // 1001
	 { 4,  6, 14, 14,  6, 16, -1},  // 1010
	 { 3, 16, 14, -1, -1, -1, -1},  // 1011
	 { 3,  4, 16, 16,  4, 18, -1},  // 1100
	 { 6, 18, 16, -1, -1, -1, -1},  // 1101
	 { 3,  4,  6, -1, -1, -1, -1},  // 1110
	 {-1, -1, -1, -1, -1, -1, -1}}, // 1111				 
	// Tetrahedron 3.									 
	{{-1, -1, -1, -1, -1, -1, -1},  // 0000
	 { 0,  4,  6, -1, -1, -1, -1},  // 0001
	 { 6, 18,  9, -1, -1, -1, -1},  // 0010
	 { 0,  4,  9,  9,  4, 18, -1},  // 0011
	 { 4,  8, 18, -1, -1, -1, -1},  // 0100
	 { 0,  8,  6,  6,  8, 18, -1},  // 0101
	 { 4,  8,  6,  6,  8,  9, -1},  // 0110
	 { 0,  8,  9, -1, -1, -1, -1},  // 0111
	 { 0,  9,  8, -1, -1, -1, -1},  // 1000
	 { 4,  6,  8,  8,  6,  9, -1},  // 1001
	 { 0,  6,  8,  8,  6, 18, -1},  // 1010
	 { 4, 18,  8, -1, -1, -1, -1},  // 1011
	 { 0,  9,  4,  4,  9, 18, -1},  // 1100
	 { 6,  9, 18, -1, -1, -1, -1},  // 1101
	 { 0,  6,  4, -1, -1, -1, -1},  // 1110
	 {-1, -1, -1, -1, -1, -1, -1}}, // 1111				 
	// Tetrahedron 4.									 
	{{-1, -1, -1, -1, -1, -1, -1},  // 0000
	 { 0,  6,  2, -1, -1, -1, -1},  // 0001
	 { 6,  9, 13, -1, -1, -1, -1},  // 0010                                  
	 { 0,  9,  2,  2,  9, 13, -1},  // 0011
	 { 0,  7,  9, -1, -1, -1, -1},  // 0100
	 { 2,  7,  6,  6,  7,  9, -1},  // 0101
	 { 0,  7,  6,  6,  7, 13, -1},  // 0110
	 { 2,  7, 13, -1, -1, -1, -1},  // 0111
	 { 2, 13,  7, -1, -1, -1, -1},  // 1000
	 { 0,  6,  7,  7,  6, 13, -1},  // 1001
	 { 2,  6,  7,  7,  6,  9, -1},  // 1010
	 { 0,  9,  7, -1, -1, -1, -1},  // 1011
	 { 0,  2,  9,  9,  2, 13, -1},  // 1100
	 { 6, 13,  9, -1, -1, -1, -1},  // 1101
	 { 0,  2,  6, -1, -1, -1, -1},  // 1110
	 {-1, -1, -1, -1, -1, -1, -1}}, // 1111				 
	// Tetrahedron 5.									 
	{{-1, -1, -1, -1, -1, -1, -1},  // 0000
	 { 1,  2,  6, -1, -1, -1, -1},  // 0001
	 { 6, 13, 12, -1, -1, -1, -1},  // 0010
	 { 1,  2, 12, 12,  2, 13, -1},  // 0011
	 { 2, 10, 13, -1, -1, -1, -1},  // 0100
	 { 1, 10,  6,  6, 10, 13, -1},  // 0101
	 { 2, 10,  6,  6, 10, 12, -1},  // 0110
	 { 1, 10, 12, -1, -1, -1, -1},  // 0111
	 { 1, 12, 10, -1, -1, -1, -1},  // 1000
	 { 2,  6, 10, 10,  6, 12, -1},  // 1001
	 { 1,  6, 10, 10,  6, 13, -1},  // 1010
	 { 2, 13, 10, -1, -1, -1, -1},  // 1011
	 { 1, 12,  2,  2, 12, 13, -1},  // 1100
	 { 6, 12, 13, -1, -1, -1, -1},  // 1101
	 { 1,  6,  2, -1, -1, -1, -1},  // 1110
	 {-1, -1, -1, -1, -1, -1, -1}}  // 1111				 
};

/**
*/
CMarchingTetrahedra::CMarchingTetrahedra(void)
{
	// Move to a new 'Create' function?
	SetupVertexPool();
}

/**
*/
CMarchingTetrahedra::~CMarchingTetrahedra(void)
{
	DeleteMesh();
}

/**
*/
bool CMarchingTetrahedra::GenerateMesh(float isoLevel)
{
	int cellType, 
		tetrahedronType,
		effectiveWidth,
		effectiveHeight,
		effectiveDepth,
		x, 
		y, 
		z,
		t,
		i;
	float valueCache[4],
		xCoord,
		yCoord,
		zCoord;

	if (!m_image.get())
	{
		MarkError();

		return false;
	}

	m_image->SetClamp(true);

	DeleteMesh();

	// Move to a new 'Delete' function?
	m_vertexToEdgeMap.clear();

	m_isoLevel = isoLevel;

	if (!m_gridWidth || !m_gridHeight || !m_gridDepth)
		if (!SetGridSize(m_image->GetWidth(), m_image->GetHeight(), m_image->GetDepth()))
		{
			MarkError();

			return false;
		}

	effectiveWidth = m_gridWidth - 1;
	effectiveHeight = m_gridHeight - 1;
	effectiveDepth = m_gridDepth - 1;

	for (z=1; z<effectiveDepth; z++)
	{
		zCoord = z*m_cellDepth;

		for (y=1; y<effectiveHeight; y++)
		{
			yCoord = y*m_cellHeight;

			valueCache[0] = m_image->GetValue(m_cellWidth, yCoord, zCoord);
			valueCache[1] = m_image->GetValue(m_cellWidth, yCoord, zCoord + m_cellDepth);
			valueCache[2] = m_image->GetValue(m_cellWidth, yCoord + m_cellHeight, zCoord);
			valueCache[3] = m_image->GetValue(m_cellWidth, yCoord + m_cellHeight, zCoord + m_cellDepth);

			for (x=1; x<effectiveWidth; x++) 
			{
				xCoord = x*m_cellWidth;

				cellType = 0;

				if (valueCache[0] < m_isoLevel)
					cellType |= 1;
				
				if (valueCache[1] < m_isoLevel)
					cellType |= 2;

				valueCache[0] = m_image->GetValue(xCoord + m_cellWidth, yCoord, zCoord);

				if (valueCache[0] < m_isoLevel)
					cellType |= 16;

				valueCache[1] = m_image->GetValue(xCoord + m_cellWidth, yCoord, zCoord + m_cellDepth);

				if (valueCache[1] < m_isoLevel)
					cellType |= 32;

				if (valueCache[3] < m_isoLevel)
					cellType |= 8;

				if (valueCache[2] < m_isoLevel)
					cellType |= 4;

				valueCache[2] = m_image->GetValue(xCoord + m_cellWidth, yCoord + m_cellHeight, zCoord);

				if (valueCache[2] < m_isoLevel)
					cellType |= 64;

				valueCache[3] = m_image->GetValue(xCoord + m_cellWidth, yCoord + m_cellHeight, zCoord + m_cellDepth);

				if (valueCache[3] < m_isoLevel)
					cellType |= 128;

				if (cellType)
				{
					for (i=1; i!=8; i++)
						if ((cellType&1) != ((cellType>>i)&1))
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, i - 1), GenerateVertex(xCoord, yCoord, zCoord, i - 1)));
					
					if (x == (effectiveWidth - 1)) 
					{
						if (((cellType>>4)&1) != ((cellType>>5)&1))
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 14), GenerateVertex(xCoord, yCoord, zCoord, 14)));
						
						if (((cellType>>4)&1) != ((cellType>>6)&1))
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 15), GenerateVertex(xCoord, yCoord, zCoord, 15)));
						
						if (((cellType>>4)&1) != ((cellType>>7)&1))
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 16), GenerateVertex(xCoord, yCoord, zCoord, 16)));
					}

					if (y == (effectiveHeight - 1)) 
					{
						if (((cellType>>2)&1) != ((cellType>>3)&1))
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 10), GenerateVertex(xCoord, yCoord, zCoord, 10)));
						
						if (((cellType>>2)&1) != ((cellType>>6)&1))
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 11), GenerateVertex(xCoord, yCoord, zCoord, 11)));
						
						if (((cellType>>2)&1) != ((cellType>>7)&1))
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 12), GenerateVertex(xCoord, yCoord, zCoord, 12)));
					}

					if (z == (effectiveDepth - 1)) 
					{
						if (((cellType>>1)&1) != ((cellType>>3)&1))
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 7), GenerateVertex(xCoord, yCoord, zCoord, 7)));
						
						if (((cellType>>1)&1) != ((cellType>>5)&1))
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 8), GenerateVertex(xCoord, yCoord, zCoord, 8)));
						
						if (((cellType>>1)&1) != ((cellType>>7)&1))
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 9), GenerateVertex(xCoord, yCoord, zCoord, 9)));
					}

					if ((x == (effectiveWidth - 1)) && (y == (effectiveHeight - 1))) 
						if (((cellType>>6)&1) != ((cellType>>7)&1))
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 17), GenerateVertex(xCoord, yCoord, zCoord, 17)));

					if ((y == (effectiveHeight - 1)) && (z == (effectiveDepth - 1))) 
						if (((cellType>>3)&1) != ((cellType>>7)&1))
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 13), GenerateVertex(xCoord, yCoord, zCoord, 13)));

					if ((x == (effectiveWidth - 1)) && (z == (effectiveDepth - 1))) 
						if (((cellType>>5)&1) != ((cellType>>7)&1))
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 18), GenerateVertex(xCoord, yCoord, zCoord, 18)));

					for (t=0; t!=6; t++)
					{
						tetrahedronType = 0;
						
						for (i=0; i!=4; i++)
							if ((1<<m_cubeSubdivisionTable[t][i]) & cellType)
								tetrahedronType |= 1<<i;

						if (tetrahedronType)
							for (i=0; m_triangleTable[t][tetrahedronType][i]!=-1; i++)
								m_triangleMesh->m_elementArray.push_back(GetEdgeId(x, y, z, m_triangleTable[t][tetrahedronType][i]));
					}
				}
			}
		}
	}

	RenameElements();

	return true;
}

/**
*/
void CMarchingTetrahedra::SetupVertexPool(void)
{
	m_triangleMesh->m_vertexSize = 3;

	m_triangleMesh->m_mode = CVertexPool::TRIANGLES;

	m_activeEdges->m_vertexSize = 3;

	m_activeEdges->m_mode = CVertexPool::LINES;
}
/**
*/
int CMarchingTetrahedra::GenerateVertex(float x, float y, float z, int edgeNumber)
{
	float orgCoord[3],
		destCoord[3];

	orgCoord[0] = destCoord[0] = x;
	orgCoord[1] = destCoord[1] = y;
	orgCoord[2] = destCoord[2] = z;

	switch (edgeNumber)
	{
	case 0:
		destCoord[2] += m_cellDepth;
		break;
	case 1:
		destCoord[1] += m_cellHeight;
		break;
	case 2:
		destCoord[1] += m_cellHeight;
		destCoord[2] += m_cellDepth;
		break;
	case 3:
		destCoord[0] += m_cellWidth;
		break;
	case 4:
		destCoord[0] += m_cellWidth;
		destCoord[2] += m_cellDepth;
		break;
	case 5:
		destCoord[0] += m_cellWidth;
		destCoord[1] += m_cellHeight;
		break;
	case 6:
		destCoord[0] += m_cellWidth;
		destCoord[1] += m_cellHeight;
		destCoord[2] += m_cellDepth;
		break;
	case 7:
		orgCoord[2] += m_cellDepth;
		destCoord[1] += m_cellHeight;
		destCoord[2] += m_cellDepth;
		break;
	case 8:
		orgCoord[2] += m_cellDepth;
		destCoord[0] += m_cellWidth;
		destCoord[2] += m_cellDepth;
		break;
	case 9:
		orgCoord[2] += m_cellDepth;
		destCoord[0] += m_cellWidth;
		destCoord[1] += m_cellHeight;
		destCoord[2] += m_cellDepth;
		break;
	case 10:
		orgCoord[1] += m_cellHeight;
		destCoord[1] += m_cellHeight;
		destCoord[2] += m_cellDepth;
		break;
	case 11:
		orgCoord[1] += m_cellHeight;
		destCoord[0] += m_cellWidth;
		destCoord[1] += m_cellHeight;
		break;
	case 12:
		orgCoord[1] += m_cellHeight;
		destCoord[0] += m_cellWidth;
		destCoord[1] += m_cellHeight;
		destCoord[2] += m_cellDepth;
		break;
	case 13:
		orgCoord[1] += m_cellHeight;
		orgCoord[2] += m_cellDepth;
		destCoord[0] += m_cellWidth;
		destCoord[1] += m_cellHeight;
		destCoord[2] += m_cellDepth;
		break;
	case 14:
		orgCoord[0] += m_cellWidth;
		destCoord[0] += m_cellWidth;
		destCoord[2] += m_cellDepth;
		break;
	case 15:
		orgCoord[0] += m_cellWidth;
		destCoord[0] += m_cellWidth;
		destCoord[1] += m_cellHeight;
		break;
	case 16:
		orgCoord[0] += m_cellWidth;
		destCoord[0] += m_cellWidth;
		destCoord[1] += m_cellHeight;
		destCoord[2] += m_cellDepth;
		break;
	case 17:
		orgCoord[0] += m_cellWidth;
		orgCoord[1] += m_cellHeight;
		destCoord[0] += m_cellWidth;
		destCoord[1] += m_cellHeight;
		destCoord[2] += m_cellDepth;
		break;
	case 18:
		orgCoord[0] += m_cellWidth;
		orgCoord[2] += m_cellDepth;
		destCoord[0] += m_cellWidth;
		destCoord[1] += m_cellHeight;
		destCoord[2] += m_cellDepth;
		break;
	};

	TransformEdge(orgCoord, destCoord);

	return FindIntersection(orgCoord, destCoord);
}

/**
*/
int CMarchingTetrahedra::GetEdgeId(int x, int y, int z, int edgeNumber) const
{
	switch (edgeNumber) 
	{
	case 0:
		return GetVertexId(x, y, z);
	case 1:
		return GetVertexId(x, y, z) + 1;
	case 2:
		return GetVertexId(x, y, z) + 2;
	case 3:
		return GetVertexId(x, y, z) + 3;
	case 4:
		return GetVertexId(x, y, z) + 4;
	case 5:
		return GetVertexId(x, y, z) + 5;
	case 6:
		return GetVertexId(x, y, z) + 6;
	case 7:
		return GetVertexId(x, y, z + 1) + 1;
	case 8:
		return GetVertexId(x, y, z + 1) + 3;
	case 9:
		return GetVertexId(x, y, z + 1) + 5;
	case 10:
		return GetVertexId(x, y + 1, z);
	case 11:
		return GetVertexId(x, y + 1, z) + 3;
	case 12:
		return GetVertexId(x, y + 1, z) + 4;
	case 13:
		return GetVertexId(x, y + 1, z + 1) + 3;
	case 14:
		return GetVertexId(x + 1, y, z);
	case 15:
		return GetVertexId(x + 1, y, z) + 1;
	case 16:
		return GetVertexId(x + 1, y, z) + 2;
	case 17:
		return GetVertexId(x + 1, y + 1, z);
	case 18:
		return GetVertexId(x + 1, y, z + 1) + 1;
	case 19: // Equivalent to edge 13.
		return GetVertexId(x + 1, y + 1, z + 1) + 3;
	case 20: // Equivalent to edge 12.
		return GetVertexId(x + 1, y + 1, z) + 4;
	case 21: // Equivalent to edge 6.
		return GetVertexId(x + 1, y, z) + 6;
	case 22: // Equivalent to edge 9.
		return GetVertexId(x + 1, y, z + 1) + 5;
	case 23: // Equivalent to edge 6.
		return GetVertexId(x, y + 1, z) + 6;
	case 24: // Equivalent to edge 16.
		return GetVertexId(x + 1, y + 1, z) + 2;
	case 25: // Equivalent to edge 18.
		return GetVertexId(x + 1, y + 1, z + 1) + 1;
	case 26: // Equivalent to edge 9.
		return GetVertexId(x, y + 1, z + 1) + 5;
	case 27: // Equivalent to edge 12.
		return GetVertexId(x, y + 1, z + 1) + 4;
	case 28: // Equivalent to edge 17.
		return GetVertexId(x + 1, y + 1, z + 1);
	case 29: // Equivalent to edge 16.
		return GetVertexId(x + 1, y, z + 1) + 2;
	case 30: // Equivalent to edge 6.
		return GetVertexId(x, y, z + 1) + 6;
	}

	return 0;
}

/**
*/
int CMarchingTetrahedra::GetVertexId(int x, int y, int z) const
{
	return 7*(m_gridWidth*(z*m_gridHeight + y) + x);
}

/**
*/
void CMarchingTetrahedra::RenameElements(void)
{
	vector<unsigned int>::iterator element;

	for (element=m_triangleMesh->m_elementArray.begin(); element!=m_triangleMesh->m_elementArray.end(); element++)
		(*element) = m_vertexToEdgeMap[(*element)];

	m_vertexToEdgeMap.clear();
}

