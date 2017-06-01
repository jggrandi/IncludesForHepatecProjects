#ifndef DUALCONTOURING_INCLUDED
#define DUALCONTOURING_INCLUDED

#ifdef MY_USING_QUAD_MESH
#define MY_ENABLE_QUAD_MESH
#endif // MY_USING_QUAD_MESH

#include <map>
#include <vector>

#include <IMarchingMethod.h>

class CDualContouring
	: public IMarchingMethod
{
public:
	CDualContouring(void);
	~CDualContouring(void);
	bool SetTransform(EDGE_TRANSFORM edgeTransform);
	bool GenerateMesh(float isoLevel);
protected:
	bool GenerateMeshFromSpanSpace(float isoLevel);
	void SetupVertexPool(void);
private:
	int GenerateVertex(float x, float y, float z, int edgeName);
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

#endif // DUALCONTOURING_INCLUDED

