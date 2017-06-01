#ifndef SURFACENETS_INCLUDED
#define SURFACENETS_INCLUDED

#include <map>
#include <vector>
#include <set>

#include <IMarchingMethod.h>

class CSurfaceNets
	: public IMarchingMethod
{
public:
	CSurfaceNets(void);
	~CSurfaceNets(void);
	bool GenerateMesh(float isoLevel);
	bool ApplyLaplacianSmoothing(int numberOfIterations);
protected:
	void SetupVertexPool(void);
private:
	void ClampToBoundingBox(const float *oldVertexCoord, float *newVertexCoord, const float boundingBox[2][3]);
	bool LoadVertexNeighborhoodInfo(void);
	int GenerateVertex(float x, float y, float z);
	inline int GetEdgeId(int x, int y, int z, int edgeName) const;
	inline int GetVertexId(int x, int y, int z) const;
	void RenameElements(void);
public:
	static const int m_edgeTable[256];
	static const int m_triangleTable[256][16];
protected:
	std::map<int, int> m_vertexToEdgeMap;
	std::vector<std::set<unsigned int> > m_vertexNeighborArray;
};

#endif // SURFACENETS_INCLUDED

