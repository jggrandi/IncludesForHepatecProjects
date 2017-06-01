#ifndef MARCHINGTETRAHEDRA_INCLUDED
#define MARCHINGTETRAHEDRA_INCLUDED

#include <map>
#include <vector>

#include <IMarchingMethod.h>

class CMarchingTetrahedra
	: public IMarchingMethod
{
public:
	CMarchingTetrahedra(void);
	~CMarchingTetrahedra(void);
	bool GenerateMesh(float isoLevel);
protected:
	void SetupVertexPool(void);
private:
	int GenerateVertex(float x, float y, float z, int edgeName);
	inline int GetEdgeId(int x, int y, int z, int edgeName) const;
	inline int GetVertexId(int x, int y, int z) const;
	void RenameElements(void);
protected:
	std::map<int, int> m_vertexToEdgeMap;
	static const int m_cubeSubdivisionTable[6][4];
	static const int m_triangleTable[6][16][7];
};

#endif // MARCHINGTETRAHEDRA_INCLUDED

