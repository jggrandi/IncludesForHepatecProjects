#include <ErrorManager.h>
#include <MyMath.h>

#include <DualContouring.h>

using namespace std;

const int CDualContouring::m_edgeTable[256] = {
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
CDualContouring::CDualContouring(void)
{
	// Move to a new 'Create' function?
	SetupVertexPool();
}

/**
*/
CDualContouring::~CDualContouring(void)
{
	// As a shared pointer, there can someone else holding a pointer to the mesh 
	// when CDualContouring is destroyed.
	//DeleteMesh();
}

/**
*/
bool CDualContouring::SetTransform(EDGE_TRANSFORM edgeTransform)
{
	return false;
}

/**
*/
bool CDualContouring::GenerateMesh(float isoLevel)
{
	int cellType, 
		edgeType,
		effectiveWidth,
		effectiveHeight,
		effectiveDepth,
		x, 
		y, 
		z;
	float valueCache[4],
		xCoord,
		yCoord,
		zCoord;

	if (!m_image.get())
	{
		MarkError();

		return false;
	}

	if (m_isUsingSpanSpace)
		if (GenerateMeshFromSpanSpace(isoLevel))
			return true;

	m_image->SetClamp(true);

	DeleteMesh();

	// Move to a new 'Delete' function?
	m_vertexToEdgeMap.clear();

	m_isoLevel = isoLevel;

	if (!m_gridWidth || !m_gridHeight || !m_gridDepth)
		if (!SetGridSize(m_image->GetWidth(), m_image->GetHeight(), m_image->GetDepth()))
			return false;

	effectiveWidth = m_gridWidth - 1;
	effectiveHeight = m_gridHeight - 1;
	effectiveDepth = m_gridDepth - 1;

	for (z=1; z<effectiveDepth; z++)
	{
		zCoord = z*m_cellDepth;

		for (y=1; y<effectiveHeight; y++)
		{
			yCoord = y*m_cellHeight;

			valueCache[0] = m_image->GetValue(m_cellWidth, yCoord + m_cellHeight, zCoord);
			valueCache[1] = m_image->GetValue(m_cellWidth, yCoord, zCoord);
			valueCache[2] = m_image->GetValue(m_cellWidth, yCoord + m_cellHeight, zCoord + m_cellDepth);
			valueCache[3] = m_image->GetValue(m_cellWidth, yCoord, zCoord + m_cellDepth);

			for (x=1; x<effectiveWidth; x++) 
			{
				xCoord = x*m_cellWidth;

				cellType = 0;

				if (valueCache[1] < m_isoLevel)
					cellType |= 1;
				
				if (valueCache[0] < m_isoLevel)
					cellType |= 2;

				valueCache[0] = m_image->GetValue(xCoord + m_cellWidth, yCoord + m_cellHeight, zCoord);

				if (valueCache[0] < m_isoLevel)
					cellType |= 4;

				valueCache[1] = m_image->GetValue(xCoord + m_cellWidth, yCoord, zCoord);

				if (valueCache[1] < m_isoLevel)
					cellType |= 8;

				if (valueCache[3] < m_isoLevel)
					cellType |= 16;

				if (valueCache[2] < m_isoLevel)
					cellType |= 32;

				valueCache[2] = m_image->GetValue(xCoord + m_cellWidth, yCoord + m_cellHeight, zCoord + m_cellDepth);

				if (valueCache[2] < m_isoLevel)
					cellType |= 64;

				valueCache[3] = m_image->GetValue(xCoord + m_cellWidth, yCoord, zCoord + m_cellDepth);

				if (valueCache[3] < m_isoLevel)
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
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z - 1)); // 0
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z - 1)); // 3
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z)); // 2
#ifndef MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z)); // 2
#endif // !MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z)); // 1
#ifndef MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z - 1)); // 0
#endif // !MY_ENABLE_QUAD_MESH
							}
							else 
							{
#ifndef MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z - 1)); // 2
#endif // !MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z)); // 0
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z - 1)); // 3
#ifndef MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z)); // 0
#endif // !MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z - 1)); // 2
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z)); // 1
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
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z)); // 0
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y - 1, z)); // 3
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z)); // 2
#ifndef MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z)); // 2
#endif // !MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z)); // 1
#ifndef MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z)); // 0
#endif // !MY_ENABLE_QUAD_MESH
							}
							else // CW?
							{
#ifndef MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z)); // 2
#endif // !MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z)); // 0
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y - 1, z)); // 3
#ifndef MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z)); // 0
#endif // !MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z)); // 2
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z)); // 1
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
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z)); // 2
#endif // !MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z - 1)); // 0
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z - 1)); // 3
#ifndef MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z - 1)); // 0
#endif // !MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z)); // 2
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z)); // 1
							}
							else // CW?
							{
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z)); // 0
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z - 1)); // 3
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z - 1)); // 2
#ifndef MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z - 1)); // 2
#endif // !MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z)); // 1
#ifndef MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z)); // 0
#endif // !MY_ENABLE_QUAD_MESH
							}
						}
					}

					if (edgeType & 8)
						m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 3), GenerateVertex(xCoord, yCoord, zCoord, 3)));
					if (edgeType & 1)
						m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 0), GenerateVertex(xCoord, yCoord, zCoord, 0)));
					if (edgeType & 256)
						m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 8), GenerateVertex(xCoord, yCoord, zCoord, 8)));

					if (x == (effectiveWidth - 1)) 
					{
						if (edgeType & 4) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 2), GenerateVertex(xCoord, yCoord, zCoord, 2)));
						if (edgeType & 2048) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 11), GenerateVertex(xCoord, yCoord, zCoord, 11)));
					}
					if (y == (effectiveHeight - 1)) 
					{
						if (edgeType & 2) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 1), GenerateVertex(xCoord, yCoord, zCoord, 1)));
						if (edgeType & 512) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 9), GenerateVertex(xCoord, yCoord, zCoord, 9)));
					}
					if (z == (effectiveDepth - 1)) 
					{
						if (edgeType & 16) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 4), GenerateVertex(xCoord, yCoord, zCoord, 4)));
						if (edgeType & 128) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 7), GenerateVertex(xCoord, yCoord, zCoord, 7)));
					}

					if ((x == (effectiveWidth - 1)) && (y == (effectiveHeight - 1)))
					{
						if (edgeType & 1024) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 10), GenerateVertex(xCoord, yCoord, zCoord, 10)));
					}
					if ((x == (effectiveWidth - 1)) && (z == (effectiveDepth - 1)))
					{
						if (edgeType & 64) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 6), GenerateVertex(xCoord, yCoord, zCoord, 6)));
					}
					if ((y == (effectiveHeight - 1)) && (z == (effectiveDepth - 1)))
					{
						if (edgeType & 32) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 5), GenerateVertex(xCoord, yCoord, zCoord, 5)));
					}
				}
			}
		}
	}

	if (!GenerateDual())
	{
		MarkError();

		return false;
	}

	return true;
}

/**
Vertex id's naming convention:
        5--------------6
       /|             /|
      / |            / |
     /  |           /  |
    1--------------2   |
    |   |          |   |
    |   4----------|---7
    |  /           |  /
    | /            | / 
    |/             |/
    0--------------3
(0,0,0)
*/
bool CDualContouring::GenerateMeshFromSpanSpace(float isoLevel)
{
	int cellType, 
		edgeType,
		effectiveWidth,
		effectiveHeight,
		effectiveDepth,
		isoLevelInSpanSpaceUnnormalizedX,
		isoLevelInSpanSpaceUnnormalizedY,
		sliceVoxelCount,
		voxelInSliceCount,
		x, 
		y, 
		z,
		i,
		j;
	float valueCache[4],
		isoLevelInSpanSpace,
		xCoord,
		yCoord,
		zCoord;
	vector<long>::iterator lastCellIndex,
		cellIndex;

	DeleteMesh();

	// Move to a new 'Delete' function?
	m_vertexToEdgeMap.clear();

	m_isoLevel = isoLevel;

	if (!m_gridWidth || !m_gridHeight || !m_gridDepth)
	{
		if (!SetGridSize(m_image->GetWidth(), m_image->GetHeight(), m_image->GetDepth()))
		{
			MarkError();

			return false;
		}
	}
	else if ((m_gridWidth != m_image->GetWidth()) || (m_gridHeight != m_image->GetHeight()) || (m_gridDepth != m_image->GetDepth()))
		return false;

	effectiveWidth = m_gridWidth - 1;
	effectiveHeight = m_gridHeight - 1;
	effectiveDepth = m_gridDepth - 1;

	isoLevelInSpanSpace = (isoLevel - m_imageValueMin)/(m_imageValueMax - m_imageValueMin);

	sliceVoxelCount = m_gridWidth*m_gridHeight;

	isoLevelInSpanSpaceUnnormalizedX = int(isoLevelInSpanSpace*m_spanSpaceWidth) - 1;

	if (isoLevelInSpanSpaceUnnormalizedX < 0)
		isoLevelInSpanSpaceUnnormalizedX = 0;

	isoLevelInSpanSpaceUnnormalizedY = int(isoLevelInSpanSpace*m_spanSpaceHeight) + 1;

	if (isoLevelInSpanSpaceUnnormalizedY < 0)
		isoLevelInSpanSpaceUnnormalizedY = 0;

	for (i=0; i<isoLevelInSpanSpaceUnnormalizedY; ++i)
		for (j=isoLevelInSpanSpaceUnnormalizedX; j<m_spanSpaceWidth; ++j)
		{
			lastCellIndex = m_spanSpace[i*m_spanSpaceWidth + j].end();

			for (cellIndex=m_spanSpace[i*m_spanSpaceWidth + j].begin(); cellIndex!=lastCellIndex; ++cellIndex)
			{
				z = int((*cellIndex)/sliceVoxelCount);

				voxelInSliceCount = (*cellIndex)%sliceVoxelCount;

				y = voxelInSliceCount/m_gridWidth;
				x = voxelInSliceCount%m_gridWidth;

				zCoord = float(z);
				yCoord = float(y);
				xCoord = float(x);

				valueCache[0] = m_image->GetValue(xCoord, yCoord + m_cellHeight, zCoord);
				valueCache[1] = m_image->GetValue(xCoord, yCoord, zCoord);
				valueCache[2] = m_image->GetValue(xCoord, yCoord + m_cellHeight, zCoord + m_cellDepth);
				valueCache[3] = m_image->GetValue(xCoord, yCoord, zCoord + m_cellDepth);

				cellType = 0;

				if (valueCache[1] < m_isoLevel)
					cellType |= 1;
				
				if (valueCache[0] < m_isoLevel)
					cellType |= 2;

				valueCache[0] = m_image->GetValue(xCoord + m_cellWidth, yCoord + m_cellHeight, zCoord);

				if (valueCache[0] < m_isoLevel)
					cellType |= 4;

				valueCache[1] = m_image->GetValue(xCoord + m_cellWidth, yCoord, zCoord);

				if (valueCache[1] < m_isoLevel)
					cellType |= 8;

				if (valueCache[3] < m_isoLevel)
					cellType |= 16;

				if (valueCache[2] < m_isoLevel)
					cellType |= 32;

				valueCache[2] = m_image->GetValue(xCoord + m_cellWidth, yCoord + m_cellHeight, zCoord + m_cellDepth);

				if (valueCache[2] < m_isoLevel)
					cellType |= 64;

				valueCache[3] = m_image->GetValue(xCoord + m_cellWidth, yCoord, zCoord + m_cellDepth);

				if (valueCache[3] < m_isoLevel)
					cellType |= 128;

				edgeType = m_edgeTable[cellType];

				if (edgeType != 0) 
				{
					m_cellToVertexMap[GetVertexId(x, y, z)] = (unsigned int)m_activeCellArray.size()/3;

					// Stores the origin of the active cell.
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
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z - 1)); // 0
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z - 1)); // 3
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z)); // 2
#ifndef MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z)); // 2
#endif // !MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z)); // 1
#ifndef MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z - 1)); // 0
#endif // !MY_ENABLE_QUAD_MESH
							}
							else 
							{
#ifndef MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z - 1)); // 2
#endif // !MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z)); // 0
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z - 1)); // 3
#ifndef MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z)); // 0
#endif // !MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z - 1)); // 2
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z)); // 1
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
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z)); // 0
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y - 1, z)); // 3
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z)); // 2
#ifndef MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z)); // 2
#endif // !MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z)); // 1
#ifndef MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z)); // 0
#endif // !MY_ENABLE_QUAD_MESH
							}
							else // CW?
							{
#ifndef MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z)); // 2
#endif // !MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z)); // 0
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y - 1, z)); // 3
#ifndef MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z)); // 0
#endif // !MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z)); // 2
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z)); // 1
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
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z)); // 2
#endif // !MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z - 1)); // 0
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z - 1)); // 3
#ifndef MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z - 1)); // 0
#endif // !MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z)); // 2
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z)); // 1
							}
							else // CW?
							{
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z)); // 0
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z - 1)); // 3
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z - 1)); // 2
#ifndef MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z - 1)); // 2
#endif // !MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z)); // 1
#ifndef MY_ENABLE_QUAD_MESH
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z)); // 0
#endif // !MY_ENABLE_QUAD_MESH
							}
						}
					}

					if (edgeType & 8)
						m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 3), GenerateVertex(xCoord, yCoord, zCoord, 3)));
					if (edgeType & 1)
						m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 0), GenerateVertex(xCoord, yCoord, zCoord, 0)));
					if (edgeType & 256)
						m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 8), GenerateVertex(xCoord, yCoord, zCoord, 8)));

					if (x == (effectiveWidth - 1)) 
					{
						if (edgeType & 4) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 2), GenerateVertex(xCoord, yCoord, zCoord, 2)));
						if (edgeType & 2048) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 11), GenerateVertex(xCoord, yCoord, zCoord, 11)));
					}
					if (y == (effectiveHeight - 1)) 
					{
						if (edgeType & 2) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 1), GenerateVertex(xCoord, yCoord, zCoord, 1)));
						if (edgeType & 512) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 9), GenerateVertex(xCoord, yCoord, zCoord, 9)));
					}
					if (z == (effectiveDepth - 1)) 
					{
						if (edgeType & 16) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 4), GenerateVertex(xCoord, yCoord, zCoord, 4)));
						if (edgeType & 128) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 7), GenerateVertex(xCoord, yCoord, zCoord, 7)));
					}

					if ((x == (effectiveWidth - 1)) && (y == (effectiveHeight - 1)))
					{
						if (edgeType & 1024) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 10), GenerateVertex(xCoord, yCoord, zCoord, 10)));
					}
					if ((x == (effectiveWidth - 1)) && (z == (effectiveDepth - 1)))
					{
						if (edgeType & 64) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 6), GenerateVertex(xCoord, yCoord, zCoord, 6)));
					}
					if ((y == (effectiveHeight - 1)) && (z == (effectiveDepth - 1)))
					{
						if (edgeType & 32) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 5), GenerateVertex(xCoord, yCoord, zCoord, 5)));
					}
				}
			}
		}

	if (!GenerateDual())
	{
		MarkError();

		return false;
	}

	return true;
}

/**
*/
void CDualContouring::SetupVertexPool(void)
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
int CDualContouring::GenerateVertex(float x, float y, float z, int edgeName)
{
	float orgCoord[3],
		destCoord[3];

	orgCoord[0] = destCoord[0] = x;
	orgCoord[1] = destCoord[1] = y;
	orgCoord[2] = destCoord[2] = z;

	switch (edgeName)
	{
	case 0:
		destCoord[1] += m_cellHeight;
		break;
	case 1:
		orgCoord[1] += m_cellHeight;
		destCoord[0] += m_cellWidth;
		destCoord[1] += m_cellHeight;
		break;
	case 2:
		orgCoord[0] += m_cellWidth;
		orgCoord[1] += m_cellHeight;
		destCoord[0] += m_cellWidth;
		break;
	case 3:
		orgCoord[0] += m_cellWidth;
		break;
	case 4:
		orgCoord[2] += m_cellDepth;
		destCoord[1] += m_cellHeight;
		destCoord[2] += m_cellDepth;
		break;
	case 5:
		orgCoord[1] += m_cellHeight;
		orgCoord[2] += m_cellDepth;
		destCoord[0] += m_cellWidth;
		destCoord[1] += m_cellHeight;
		destCoord[2] += m_cellDepth;
		break;
	case 6:
		orgCoord[0] += m_cellWidth;
		orgCoord[1] += m_cellHeight;
		orgCoord[2] += m_cellDepth;
		destCoord[0] += m_cellWidth;
		destCoord[2] += m_cellDepth;
		break;
	case 7:
		orgCoord[0] += m_cellWidth;
		orgCoord[2] += m_cellDepth;
		destCoord[2] += m_cellDepth;
		break;
	case 8:
		destCoord[2] += m_cellDepth;
		break;
	case 9:
		orgCoord[1] += m_cellHeight;
		destCoord[1] += m_cellHeight;
		destCoord[2] += m_cellDepth;
		break;
	case 10:
		orgCoord[0] += m_cellWidth;
		orgCoord[1] += m_cellHeight;
		destCoord[0] += m_cellWidth;
		destCoord[1] += m_cellHeight;
		destCoord[2] += m_cellDepth;
		break;
	case 11:
		orgCoord[0] += m_cellWidth;
		destCoord[0] += m_cellWidth;
		destCoord[2] += m_cellDepth;
		break;
	};

	return FindIntersection(orgCoord, destCoord);
}

/**
*/
int CDualContouring::GetEdgeId(int x, int y, int z, int edgeName) const
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
int CDualContouring::GetVertexId(int x, int y, int z) const
{
	return 3*(m_gridWidth*(z*m_gridHeight + y) + x);
}

/**
*/
bool CDualContouring::GenerateDual(void)
{
	vector<unsigned int>::iterator activeCell;
	int edgeId,
		vertexId,
		i;
	float dualVertex[3];
	const float *dualNormal;
	vector<float> dualVertexArray,
		dualNormalArray;

	// Returns if there is not active cells.
	if (m_activeCellArray.empty())
	{
		MarkError();

		return false;
	}

	// Run through all active cells. Each active cell is represented by the 
	// triple (x, y, z) which indicates the origin of the cell.
	for (activeCell=m_activeCellArray.begin(); activeCell!=m_activeCellArray.end(); activeCell+=3)
	{
		ResetDualVertexSearch();

		// For each of the 12 edges of the cell, ...
		for (i=0; i<12; ++i)
		{
			// ... get the edge name,...
			edgeId = GetEdgeId(*activeCell, *(activeCell + 1), *(activeCell + 2), i);

			// ... check if the edge is active,...
			if (m_vertexToEdgeMap.find(edgeId) != m_vertexToEdgeMap.end())
			{
				// ... and get the name of the vertex generated by the edge.
				vertexId = m_vertexToEdgeMap[edgeId];

				// All vertices generated on cell edges are combined to 
				// compute the position of the dual vertex. The position of 
				// the dual vertex is computed iteratively, where each 
				// iteration integrates the force exerted by all vertices on 
				// the dual vertex, in order to find the optimal position to 
				// place the dual vertex.
				AddEdgeIntersectionToDualVertexSearch(*activeCell, *(activeCell + 1), *(activeCell + 2), &m_triangleMesh->m_vertexArray[3*vertexId], &m_triangleMesh->m_normalArray[3*vertexId]);
			}
		}

		if (!FindDualVertex(*activeCell, *(activeCell + 1), *(activeCell + 2), dualVertex))
		{
			MarkError();

			return false;
		}

		dualNormal = m_image->GetNormal(dualVertex[0], dualVertex[1], dualVertex[2]);

		dualNormalArray.push_back(-dualNormal[0]);
		dualNormalArray.push_back(-dualNormal[1]);
		dualNormalArray.push_back(-dualNormal[2]);

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
void CDualContouring::ResetDualVertexSearch(void)
{
	m_cellVertexArray.clear();
	m_cellGradientArray.assign(3*8, 0.0f);
}

/**
*/
void CDualContouring::AddEdgeIntersectionToDualVertexSearch(int x, int y, int z, float *position, float *normal)
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

	vertex[0] = position[0] - (float)x*m_cellWidth;
	vertex[1] = position[1] - (float)y*m_cellHeight;
	vertex[2] = position[2] - (float)z*m_cellDepth;

	vertex[3] = 1.0f;

	planeCoeff[0] = normal[0];
	planeCoeff[1] = normal[1];
	planeCoeff[2] = normal[2];
	planeCoeff[3] = -(vertex[0]*normal[0]) - (vertex[1]*normal[1]) - (vertex[2]*normal[2]);

	for (j=8; j--; )
	{
		distance = planeCoeff[0]*cellVertexArray[j][0]*m_cellWidth;
		distance += planeCoeff[1]*cellVertexArray[j][1]*m_cellHeight;
		distance += planeCoeff[2]*cellVertexArray[j][2]*m_cellDepth;
		distance += planeCoeff[3]*cellVertexArray[j][3];

		for (i=0; i<3; ++i)
			m_cellGradientArray[3*j + i] += distance*normal[i];
	}
}

/**
*/
bool CDualContouring::FindDualVertex(int x, int y, int z, float *position)
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
		return false;

	for (i=0; i<3; ++i)
		centroid[i] = 0.0f;

	vertexCount = (int)m_cellVertexArray.size()/3;

	for (j=vertexCount; j--; )
		for (i=0; i<3; ++i)
			centroid[i] += m_cellVertexArray[3*j + i];

	for (i=0; i<3; ++i)
		centroid[i] /= vertexCount;

	// X, y, and z define the cell origin, instead of vertex coordinates.
	boundingBox[0][0] = (float)x*m_cellWidth;
	boundingBox[0][1] = (float)y*m_cellHeight;
	boundingBox[0][2] = (float)z*m_cellDepth;
	boundingBox[1][0] = boundingBox[0][0] + m_cellWidth;
	boundingBox[1][1] = boundingBox[0][1] + m_cellHeight;
	boundingBox[1][2] = boundingBox[0][2] + m_cellDepth;

	for (i=0; i<3; ++i)
		vertex[i] = centroid[i];

	for (i=0; i<3; ++i)
	{
		if ((vertex[i] < boundingBox[0][i]) || (vertex[i] > boundingBox[1][i]))
			MarkError();
	}

	// A weak attempt to set a adaptive stepsize.
	stepsize = min(m_cellWidth, min(m_cellHeight, m_cellDepth))*0.05f;

	lastSquaredNorma = m_cellWidth*m_cellWidth + m_cellHeight*m_cellHeight + m_cellDepth*m_cellDepth;

	// Number of steps?
	for (k=8; k--; )
	{
		tx = (vertex[0] - boundingBox[0][0])/m_cellWidth;

		for (i=0; i<3; ++i)
		{
			weightedGradientArray[0][i] = (1.0f - tx)*m_cellGradientArray[3*0 + i] + tx*m_cellGradientArray[3*4 + i];
			weightedGradientArray[1][i] = (1.0f - tx)*m_cellGradientArray[3*1 + i] + tx*m_cellGradientArray[3*5 + i];
			weightedGradientArray[2][i] = (1.0f - tx)*m_cellGradientArray[3*2 + i] + tx*m_cellGradientArray[3*6 + i];
			weightedGradientArray[3][i] = (1.0f - tx)*m_cellGradientArray[3*3 + i] + tx*m_cellGradientArray[3*7 + i];
		}

		ty = (vertex[1] - boundingBox[0][1])/m_cellHeight;

		for (i=0; i<3; ++i)
		{
			weightedGradientArray[0][i] = (1.0f - ty)*weightedGradientArray[0][i] + ty*weightedGradientArray[2][i];
			weightedGradientArray[1][i] = (1.0f - ty)*weightedGradientArray[1][i] + ty*weightedGradientArray[3][i];
		}

		tz = (vertex[2] - boundingBox[0][2])/m_cellDepth;

		squaredNorma = 0.0f;

		for (i=0; i<3; ++i)
		{
			weightedGradientArray[0][i] = (1.0f - tz)*weightedGradientArray[0][i] + tz*weightedGradientArray[1][i];

			squaredNorma += weightedGradientArray[0][i]*weightedGradientArray[0][i];
		}

		if (lastSquaredNorma < squaredNorma)
			stepsize *= 0.95f;
		else
			lastSquaredNorma = squaredNorma;

		for (i=0; i<3; ++i)
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
void CDualContouring::RenameElements(void)
{
	vector<unsigned int>::iterator element;

	for (element=m_triangleMesh->m_elementArray.begin(); element!=m_triangleMesh->m_elementArray.end(); element++)
		(*element) = m_cellToVertexMap[(*element)];

	m_cellToVertexMap.clear();
}

