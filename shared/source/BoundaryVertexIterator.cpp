#include <ErrorManager.h>
#include <BoundaryVertexIterator.h>

#include <iostream>

using namespace std;

/**
*/
CBoundaryVertexIterator::CBoundaryVertexIterator(void)
{
	Create();
}

/**
*/
CBoundaryVertexIterator::~CBoundaryVertexIterator(void)
{
}

/**
*/
CBoundaryVertexIterator* CBoundaryVertexIterator::operator++(void)
{
	if (m_borderId < m_boundaryVertexArray.size())
		if (m_cursor != m_boundaryVertexArray[m_borderId].end())
			m_cursor++;

	return this;
}

/**
*/
bool CBoundaryVertexIterator::SetMesh(boost::shared_ptr<CVertexPool> vertexPool)
{
	const unsigned int *primitive;
	int primitiveCount,
		primitiveSize,
		i,
		j;
	vector<vector<pair<unsigned int, unsigned int> > >::iterator neighborEdgeArray;
	vector<pair<unsigned int, unsigned int> >::iterator neighborEdge,
		nextNeighborEdge;
	unsigned int firstVertex,
		currentVertex;
	vector<vector<vector<pair<unsigned int, unsigned int> > >::iterator> traversedEdgeArray;

	if (!vertexPool.get())
	{
		MarkError();

		return false;
	}

	if (!vertexPool->GetVertexCount() || !vertexPool->GetElementCount())
	{
		MarkError();

		return false;
	}

	primitive = vertexPool->GetElementArray();
	
	primitiveCount = vertexPool->GetPrimitiveCount();
	primitiveSize = vertexPool->GetPrimitiveSize();

	// O procedimento lida apenas com triângulos.
	if (primitiveSize != 3)
	{
		MarkError();

		return false;
	}

	m_neighborArray.assign(vertexPool->GetVertexCount(), vector<pair<unsigned int, unsigned int> >());

	// Loops over the triangles.
	for (i=0; i!=primitiveCount; i++)
	{
		// To each triangle $t$ incident to a vertex $i$, adds the edge which 
		// connects the neighbors of $i$ in $t$ to the list of neighboring 
		// edges of $i$.
		for (j=0; j!=primitiveSize; j++)
			m_neighborArray[primitive[j]].push_back(make_pair(primitive[(j + 1)%3], primitive[(j + 2)%3]));

		primitive += primitiveSize;
	}

	for (neighborEdgeArray=m_neighborArray.begin(); neighborEdgeArray!=m_neighborArray.end(); neighborEdgeArray++)
	{
		// Problema com os vertices do morphing. Alguns eram vertices falsos, 
		// incluidos apenas para obter um mapeamento sobrejetivo. Estes 
		// vertices nao tem vizinhos.
		if (neighborEdgeArray->empty())
			continue;

		neighborEdge = neighborEdgeArray->begin();

		// To each vertex $i$, loops over the edges which connects the 
		// neighbors of $i$.
		while (neighborEdge != neighborEdgeArray->end())
		{
			if (neighborEdgeArray->size() == 1)
				break;

			nextNeighborEdge = neighborEdge + 1;

			// Try to find the next edge in the edge circuit.
			while (nextNeighborEdge != neighborEdgeArray->end())
			{
				if ((neighborEdge->second == nextNeighborEdge->first) || (neighborEdge->second == nextNeighborEdge->second) ||(neighborEdge->first == nextNeighborEdge->first) || (neighborEdge->first == nextNeighborEdge->second))
					break;

				nextNeighborEdge++;
			}

			// Collapses $neighborEdge$ and $nextNeighborEdge$ edges.
			if (nextNeighborEdge != neighborEdgeArray->end())
			{
				if (neighborEdge->second == nextNeighborEdge->first)
					neighborEdge->second = nextNeighborEdge->second;
				else if (neighborEdge->second == nextNeighborEdge->second)
					neighborEdge->second = nextNeighborEdge->first;
				else if (neighborEdge->first == nextNeighborEdge->first)
					neighborEdge->first = nextNeighborEdge->second;
				else if (neighborEdge->first == nextNeighborEdge->second)
					neighborEdge->first = nextNeighborEdge->first;
				else
				{
					MarkError();

					return false;
				}

				// If we reach this point, the edges $neighborEdge$ and 
				// $nextNeighborEdge$ were collapsed, and $nextNeighborEdge$ 
				// can be excluded from the list.
				neighborEdgeArray->erase(nextNeighborEdge);

				continue;
			}

			neighborEdge++;
		}

		// A singular vertex. If we reach this point with more than one edge 
		// in the vertex neighboring list, we have a singular vertex (more 
		// than one circuit including the vertex). 
		if ((int)neighborEdgeArray->size() > 1)
		{
			MarkError();

			return false;
		}

		// If $first$ is equal to $second$, we complete the circuit, and the 
		// vertex $i$ is on the mesh boundary.
		if (neighborEdgeArray->front().first == neighborEdgeArray->front().second)
			neighborEdgeArray->clear();
	}

	// Sort the boundary vertices.
	firstVertex = 0;
	
	// BEGIN OF TEST (find the first boundary vertex).
	for (neighborEdgeArray=m_neighborArray.begin(); neighborEdgeArray!=m_neighborArray.end(); neighborEdgeArray++)
	{
		if (!neighborEdgeArray->empty())
			break;

		firstVertex++;
	}
	// END OF TEST (find the first boundary vertex).

	// Traverses the vertices on the boundary.
	while (neighborEdgeArray != m_neighborArray.end())
	{
		// Adds an empty border vertex id list.
		m_boundaryVertexArray.push_back(vector<unsigned int>());

		neighborEdgeArray = m_neighborArray.begin() + m_neighborArray[firstVertex].front().first;

		currentVertex = neighborEdgeArray->front().second;

		traversedEdgeArray.clear();

		do {
			if (neighborEdgeArray->front().first == currentVertex)
			{
				// Vértice para o qual eu me movi.
				currentVertex = m_neighborArray[currentVertex].front().first;

				// Ele passa a ser o vértice atual.
				m_boundaryVertexArray.back().push_back(currentVertex);

				// Próximo vértice, no sentido do movimento.
				if (m_neighborArray[m_neighborArray[currentVertex].front().second].empty())
				{
					MarkError();

					return false;
				}

				traversedEdgeArray.push_back(neighborEdgeArray);

				neighborEdgeArray = m_neighborArray.begin() + m_neighborArray[currentVertex].front().second;
			}
			else if (neighborEdgeArray->front().second == currentVertex)
			{
				// Vértice para o qual eu me movi.
				currentVertex = m_neighborArray[currentVertex].front().first;

				// Ele passa a ser o vértice atual.
				m_boundaryVertexArray.back().push_back(currentVertex);

				// Próximo vértice, no sentido do movimento.
				if (m_neighborArray[m_neighborArray[currentVertex].front().first].empty())
				{
					MarkError();

					return false;
				}

				traversedEdgeArray.push_back(neighborEdgeArray);

				neighborEdgeArray = m_neighborArray.begin() + m_neighborArray[currentVertex].front().first;
			}
			else 
			{
				MarkError();

				return false;
			}
		} while (currentVertex != firstVertex);

		// Remove the traversed edges.
		for ( ; !traversedEdgeArray.empty(); )
		{
			traversedEdgeArray.back()->clear();

			traversedEdgeArray.pop_back();
		}

		firstVertex = 0;
		
		// BEGIN OF TEST (try to find another boundary).
		for (neighborEdgeArray=m_neighborArray.begin(); neighborEdgeArray!=m_neighborArray.end(); neighborEdgeArray++)
		{
			if (!neighborEdgeArray->empty())
				break;

			firstVertex++;
		}
		// END OF TEST (try to find another boundary).
	}

	if (!m_boundaryVertexArray.empty())
		m_cursor = m_boundaryVertexArray.front().begin();

	return true;
}

/**
*/
void CBoundaryVertexIterator::Initialize(int borderId)
{
	if (m_borderId < m_boundaryVertexArray.size())
	{
		m_cursor = m_boundaryVertexArray[m_borderId].begin();

		m_borderId = borderId;
	}
}

/**
*/
bool CBoundaryVertexIterator::NotFinished(void) const
{
	if (m_borderId < m_boundaryVertexArray.size())
		if (m_boundaryVertexArray[m_borderId].empty())
			return false;

	return (m_cursor != m_boundaryVertexArray[m_borderId].end());
}

/**
*/
unsigned int CBoundaryVertexIterator::GetId(void) const
{
	if (m_borderId < m_boundaryVertexArray.size())
		if (m_boundaryVertexArray[m_borderId].empty())
			return false;

	if (m_cursor == m_boundaryVertexArray[m_borderId].end())
		return 0;

	return *m_cursor;
}

/**
*/
int CBoundaryVertexIterator::GetBoundaryVertexCount(void) const
{
	if (m_borderId < m_boundaryVertexArray.size())
		if (m_boundaryVertexArray[m_borderId].empty())
			return 0;

	return (int)m_boundaryVertexArray[m_borderId].size();
}

/**
*/
int CBoundaryVertexIterator::GetBoundaryCount(void) const
{
	return (int)m_boundaryVertexArray.size();
}

/**
*/
bool CBoundaryVertexIterator::IsOnBoundary(int vertexId)
{
	std::vector<std::vector<unsigned int> >::const_iterator boundaryVertexArray;

	for (boundaryVertexArray=m_boundaryVertexArray.begin(); boundaryVertexArray!=m_boundaryVertexArray.end(); boundaryVertexArray++)
		if (find(boundaryVertexArray->begin(), boundaryVertexArray->end(), vertexId) != boundaryVertexArray->end())
			return true;

	return false;
}

/**
*/
void CBoundaryVertexIterator::Create(void)
{
	m_neighborArray.clear();

	m_boundaryVertexArray.clear();

	//m_cursor = 0;

	m_borderId = 0;
}

