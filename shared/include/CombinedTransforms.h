#ifndef COMBINEDTRANSFORMS_INCLUDED
#define COMBINEDTRANSFORMS_INCLUDED

#define MY_USING_NEW_EDGE_CASES

#include <map>
#include <vector>

#include <IMarchingMethod.h>

class CCombinedTransforms
	: public IMarchingMethod
{
public:
	CCombinedTransforms(void);
	~CCombinedTransforms(void);
	bool GenerateMesh(float isoLevel);
protected:
	bool GenerateMeshFromSpanSpace(float isoLevel);
	void SetupVertexPool(void);
private:
	void RemoveUnreferencedVertices(void);
	float GetTriangleArrayQuality(const float *vertexArray, const int *triangleArray, int triangleCount) const;
	bool MeshChoose(void);
	int GenerateVertex(float x, float y, float z, int edgeName);
	inline int GetEdgeId(int x, int y, int z, int edgeName) const;
	inline int GetVertexId(int x, int y, int z) const;
	void RenameElements(void);
protected:
	std::map<int, int> m_vertexToEdgeMap;
#ifndef NDEBUG
	int m_ambiguousCubeCaseCount;
#endif // #ifndef NDEBUG
	static const int m_cubeCaseToEdgeBitFieldArray[256];
	static const int m_triangleTable[256][16];
#ifndef NDEBUG
	static const int m_isAmbiguousCubeCaseArray[256];
#endif // #ifndef NDEBUG
};

#endif // COMBINEDTRANSFORMS_INCLUDED

