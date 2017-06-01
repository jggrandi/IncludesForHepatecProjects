#include <cmath>

#include <ErrorManager.h>
#include <MyMath.h>

#include <SurfaceNets.h>

using namespace std;

/**
*/
CSurfaceNets::CSurfaceNets(void)
{
	// Move to a new 'Create' function?
	SetupVertexPool();
}

/**
*/
CSurfaceNets::~CSurfaceNets(void)
{
	// I must be careful with the destruction of this object. It is possible 
	// that the object is sharing the triangle mesh with other objects, and thus 
	// I don't have to clear it.
	//DeleteMesh();
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
bool CSurfaceNets::GenerateMesh(float isoLevel)
{
	int width,
		height, 
		depth,
		cellType,
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

	m_image->SetClamp(true);

	DeleteMesh();

	// Move to a new 'Delete' function?
	m_vertexToEdgeMap.clear();
	m_vertexNeighborArray.clear();

	m_isoLevel = isoLevel;

	if (!m_gridWidth || !m_gridHeight || !m_gridDepth)
		if (!SetGridSize(m_image->GetWidth(), m_image->GetHeight(), m_image->GetDepth()))
		{
			MarkError();

			return false;
		}

	width  = m_gridWidth;
	height = m_gridHeight;
	depth  = m_gridDepth;

	for (z=1; z!=(depth - 1); z++)
	{
		zCoord = z*m_cellDepth;

		for (y=1; y!=(height - 1); y++)
		{
			yCoord = y*m_cellHeight;

			valueCache[0] = m_image->GetValue(m_cellWidth, yCoord + m_cellHeight, zCoord);
			valueCache[1] = m_image->GetValue(m_cellWidth, yCoord, zCoord);
			valueCache[2] = m_image->GetValue(m_cellWidth, yCoord + m_cellHeight, zCoord + m_cellDepth);
			valueCache[3] = m_image->GetValue(m_cellWidth, yCoord, zCoord + m_cellDepth);

			for (x=1; x!=(width - 1); x++) 
			{
				xCoord = x*m_cellWidth;

				cellType = 0;

				if (valueCache[1] < isoLevel)
					cellType |= 1;

				if (valueCache[0] < isoLevel)
					cellType |= 2;

				valueCache[0] = m_image->GetValue(xCoord + m_cellWidth, yCoord + m_cellHeight, zCoord);

				if (valueCache[0] < isoLevel)
					cellType |= 4;

				valueCache[1] = m_image->GetValue(xCoord + m_cellWidth, yCoord, zCoord);

				if (valueCache[1] < isoLevel)
					cellType |= 8;

				if (valueCache[3] < isoLevel)
					cellType |= 16;

				if (valueCache[2] < isoLevel)
					cellType |= 32;

				valueCache[2] = m_image->GetValue(xCoord + m_cellWidth, yCoord + m_cellHeight, zCoord + m_cellDepth);

				if (valueCache[2] < isoLevel)
					cellType |= 64;

				valueCache[3] = m_image->GetValue(xCoord + m_cellWidth, yCoord, zCoord + m_cellDepth);

				if (valueCache[3] < isoLevel)
					cellType |= 128;

				// Test for active cells.
				if ((cellType != 0) && (cellType != 255))
				{
					m_vertexToEdgeMap[GetVertexId(x, y, z)] = GenerateVertex(xCoord, yCoord, zCoord);

					// Test if the edge between vertices 0 (x, y, z) and 1 
					// (x, y + 1, z) is active.
					if ((cellType&1) != ((cellType>>1)&1))
					{
						if ((x > 1) && (z > 1))
						{
							if (cellType&1)
							{
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z - 1));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z));

								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z - 1));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z - 1));
							}
							else
							{
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z - 1));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z));

								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z - 1));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z - 1));
							}
						}
					}
					
					// Test if the edge between vertices 0 (x, y, z) and 4 
					// (x, y, z + 1) is active.
					if ((cellType&1) != ((cellType>>4)&1))
					{
						if ((x > 1) && (y > 1))
						{
							if (cellType&1)
							{
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z));

								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y - 1, z));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z));
							}
							else
							{
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z));

								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y, z));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x - 1, y - 1, z));
							}
						}
					}
					
					// Test if the edge between vertices 0 (x, y, z) and 2 
					// (x + 1, y, z) is active.
					if ((cellType&1) != ((cellType>>3)&1))
					{
						if ((y > 1) && (z > 1))
						{
							if (cellType&1)
							{
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z - 1));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z));

								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z - 1));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z - 1));
							}
							else
							{
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z - 1));

								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y, z - 1));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z - 1));
								m_triangleMesh->m_elementArray.push_back(GetVertexId(x, y - 1, z));
							}
						}
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
bool CSurfaceNets::ApplyLaplacianSmoothing(int numberOfIterations)
{
	vector<set<unsigned int> >::iterator vertexNeighbor;
	set<unsigned int>::iterator neighbor;
	const float *vertexArray,
		*vertex;
	int vertexId,
		x,
		y,
		z,
		i,
		j;
	float boundingBoxMargin,
		boundingBox[2][3],
		centroid[3];

	if (!m_triangleMesh->IsValid())
	{
		MarkError();

		return false;
	}

	if (m_triangleMesh->GetVertexSize() != 3)
	{
		MarkError();

		return false;
	}

	if (m_vertexNeighborArray.empty())
		if (!LoadVertexNeighborhoodInfo())
		{
			MarkError();

			return false;
		}

	boundingBoxMargin = 50.0f;

	vertexArray = m_triangleMesh->GetVertexArray();

	for (j=0; j<numberOfIterations; j++)
	{
		vertexId = 0;
			
		for (vertexNeighbor=m_vertexNeighborArray.begin(); vertexNeighbor!=m_vertexNeighborArray.end(); vertexNeighbor++)
		{
			vertex = m_triangleMesh->GetVertex(vertexId);

			// Calculates the origin of the cell which generates the vertex.
			x = int(float(vertex[0]/m_cellWidth));
			y = int(float(vertex[1]/m_cellHeight));
			z = int(float(vertex[2]/m_cellDepth));
			
			// Calculates the vertex bounding box, which is based on the cell 
			// origin. If I let the vertex go to the bounding box limits, however, 
			// I will broke the calculation of the cell origin (above); so, 
			// there is a "safe margin" on the bounding box, based on the cell 
			// edges size.
			boundingBox[0][0] = float(x)*m_cellWidth + m_cellWidth/boundingBoxMargin;
			boundingBox[0][1] = float(y)*m_cellHeight + m_cellHeight/boundingBoxMargin;
			boundingBox[0][2] = float(z)*m_cellDepth + m_cellDepth/boundingBoxMargin;
			boundingBox[1][0] = boundingBox[0][0] + m_cellWidth - 2.0f*m_cellWidth/boundingBoxMargin;
			boundingBox[1][1] = boundingBox[0][1] + m_cellHeight - 2.0f*m_cellHeight/boundingBoxMargin;
			boundingBox[1][2] = boundingBox[0][2] + m_cellDepth - 2.0f*m_cellDepth/boundingBoxMargin;

			for (i=0; i<3; i++)
				centroid[i] = 0.0f;

			for (neighbor=vertexNeighbor->begin(); neighbor!=vertexNeighbor->end(); neighbor++)
				for (i=0; i<3; i++)
					centroid[i] += vertexArray[3*(*neighbor) + i];

			for (i=0; i<3; i++)
				centroid[i] /= vertexNeighbor->size();

			ClampToBoundingBox(vertex, centroid, boundingBox);

			m_triangleMesh->SetVertex(vertexId, centroid[0], centroid[1], centroid[2]);

			vertex += 3;
			vertexId++;
		}
	}

	return true;
}

/**
*/
void CSurfaceNets::SetupVertexPool(void)
{
	m_triangleMesh->m_vertexSize = 3;

	m_triangleMesh->m_mode = CVertexPool::TRIANGLES;

	m_activeEdges->m_vertexSize = 3;

	m_activeEdges->m_mode = CVertexPool::LINES;
}

/**
*/
void CSurfaceNets::ClampToBoundingBox(const float *oldVertexCoord, float *newVertexCoord, const float boundingBox[2][3])
{
	int i;
	float scale;
	bool clamp;

	scale = 1.0f;

	clamp = false;

	for (i=0; i<3; i++)
	{
		if (newVertexCoord[i] < boundingBox[0][i])
		{
			scale = min(scale, (boundingBox[0][i] - oldVertexCoord[i])/(newVertexCoord[i] - oldVertexCoord[i]));

			clamp = true;
		}
		else if (newVertexCoord[i] > boundingBox[1][i])
		{
			scale = min(scale, (boundingBox[1][i] - oldVertexCoord[i])/(newVertexCoord[i] - oldVertexCoord[i]));

			clamp = true;
		}
	}

	if (clamp)
		MyMath::LinearInterpolate<float, 3>(oldVertexCoord, newVertexCoord, scale, newVertexCoord);
}

/**
*/
bool CSurfaceNets::LoadVertexNeighborhoodInfo(void)
{
	int triangleCount,
		i;
	const unsigned int *elementArray;
	
	if (!m_triangleMesh->IsValid())
	{
		MarkError();

		return false;
	}

	if (m_triangleMesh->GetMode() != CVertexPool::TRIANGLES)
	{
		MarkError();

		return false;
	}

	triangleCount = m_triangleMesh->GetPrimitiveCount();

	elementArray = m_triangleMesh->GetElementArray();

	m_vertexNeighborArray.assign(m_triangleMesh->GetVertexCount(), set<unsigned int>());
	
	for (i=0; i<triangleCount; i++)
	{
		m_vertexNeighborArray[*elementArray].insert(*(elementArray + 1));
		m_vertexNeighborArray[*(elementArray + 1)].insert(*elementArray);
		
		m_vertexNeighborArray[*(elementArray + 1)].insert(*(elementArray + 2));
		m_vertexNeighborArray[*(elementArray + 2)].insert(*(elementArray + 1));
		
		m_vertexNeighborArray[*(elementArray + 2)].insert(*elementArray);
		m_vertexNeighborArray[*elementArray].insert(*(elementArray + 2));
		
		elementArray += 3;
	}

	return true;
}

/**
*/
int CSurfaceNets::GenerateVertex(float x, float y, float z)
{
	int vertexIndex;
	const float *normalAnswer;

	vertexIndex = (int)m_triangleMesh->m_vertexArray.size()/3;

	x += 0.5f*m_cellWidth;
	y += 0.5f*m_cellHeight;
	z += 0.5f*m_cellDepth;

	m_triangleMesh->m_vertexArray.push_back(x);
	m_triangleMesh->m_vertexArray.push_back(y);
	m_triangleMesh->m_vertexArray.push_back(z);

	normalAnswer = m_image->GetNormal(x, y, z);

	m_triangleMesh->m_normalArray.push_back(-normalAnswer[0]);
	m_triangleMesh->m_normalArray.push_back(-normalAnswer[1]);
	m_triangleMesh->m_normalArray.push_back(-normalAnswer[2]);

	return vertexIndex;
}

/**
*/
int CSurfaceNets::GetVertexId(int x, int y, int z) const
{
	return 3*(m_gridWidth*(z*m_gridHeight + y) + x);
}

/**
*/
void CSurfaceNets::RenameElements(void)
{
	vector<unsigned int>::iterator element;

	for (element=m_triangleMesh->m_elementArray.begin(); element!=m_triangleMesh->m_elementArray.end(); element++)
		(*element) = m_vertexToEdgeMap[(*element)];

	m_vertexToEdgeMap.clear();
}

