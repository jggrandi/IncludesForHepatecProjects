#ifndef SIMPLEDUALCONTOURING_INCLUDED
#define SIMPLEDUALCONTOURING_INCLUDED

#ifdef MY_USING_QUAD_MESH
#define MY_ENABLE_QUAD_MESH
#endif // MY_USING_QUAD_MESH

#include <map>
#include <vector>

#include <IMarchingMethod.h>

class CSimpleDualContouring
	: public IMarchingMethod
{
public:
	CSimpleDualContouring(void);
	~CSimpleDualContouring(void);
	bool SetTransform(EDGE_TRANSFORM edgeTransform);
	bool SetCellSize(float width, float height, float depth);
	bool SetGridSize(int width, int height, int depth);
	bool GenerateMesh(float isoLevel);
protected:
	void SetupVertexPool(void);
private:
	int GenerateVertex(int x, int y, int z, int edgeName);
	int FindIntersection(float *orgCoord, float *destCoord);
	inline int GetEdgeId(int x, int y, int z, int edgeName) const;
	inline int GetVertexId(int x, int y, int z) const;
	bool GenerateDual(void);
	void ResetDualVertexSearch(void);
	void AddEdgeIntersectionToDualVertexSearch(int x, int y, int z, float *position, float *normal);
	bool FindDualVertex(int x, int y, int z, float *position);
	void RenameElements(void);
protected:
	std::map<int, int> m_vertexToEdgeMap;
	std::map<int, int> m_cellToVertexMap;
	std::vector<unsigned int> m_activeCellArray;
	std::vector<float> m_cellVertexArray;
	std::vector<float> m_cellGradientArray;
	static const int m_edgeTable[256];
};

#endif // SIMPLEDUALCONTOURING_INCLUDED

