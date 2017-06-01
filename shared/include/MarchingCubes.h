#ifndef MARCHINGCUBES_INCLUDED
#define MARCHINGCUBES_INCLUDED

//#define MY_USING_NEW_EDGE_CASES
//#define MY_COUNTING_MC_CASES
//#define MY_COUNTING_MC_AMBIGUOUS_CASES

#include <map>
#include <vector>

#include <IMarchingMethod.h>

class CMarchingCubes
	: public IMarchingMethod
{
public:
	CMarchingCubes(void);
	~CMarchingCubes(void);
	bool GenerateMesh(float isoLevel);
protected:
	bool GenerateMeshFromSpanSpace(float isoLevel);
	void SetupVertexPool(void);
private:
	int GenerateVertex(float x, float y, float z, int edgeName);
	inline int GetEdgeId(int x, int y, int z, int edgeName) const;
	inline int GetVertexId(int x, int y, int z) const;
	void RenameElements(void);
public:
	static const int m_cubeCaseToEdgeBitFieldArray[256];
	static const int m_triangleTable[256][16];
#ifdef MY_COUNTING_MC_AMBIGUOUS_CASES
	static const int m_isAmbiguousCubeCaseArray[256];
#endif // #ifdef MY_COUNTING_MC_AMBIGUOUS_CASES
protected:
	std::map<int, int> m_vertexToEdgeMap;
#ifdef MY_COUNTING_MC_AMBIGUOUS_CASES
	int m_ambiguousCubeCaseCount;
	int m_activeCellCount;
#endif // #ifdef MY_COUNTING_MC_AMBIGUOUS_CASES
};

#endif // MARCHINGCUBES_INCLUDED

