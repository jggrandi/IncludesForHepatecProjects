#ifndef SIMPLEMARCHINGCUBES_INCLUDED
#define SIMPLEMARCHINGCUBES_INCLUDED

#define MY_USING_NEW_EDGE_CASES
#define MY_REMOVING_EDGE_GROUP_2

#include <map>
#include <vector>
#include <set>

#include <ErrorManager.h>
#include <MyMath.h>
#include <MyMeshUtils.h>
#include <IMarchingMethod.h>

template <typename T>
class CSimpleMarchingCubes
	: public IMarchingMethod
{
public:
	CSimpleMarchingCubes(void);
	~CSimpleMarchingCubes(void);
	bool SetCellSize(float width, float height, float depth);
	bool SetGridSize(int width, int height, int depth);
	bool GenerateMesh(float isoLevel);
protected:
	bool GenerateMeshFromSpanSpace(float isoLevel);
	void SetupVertexPool(void);
private:
	inline int GetEdgeId(int x, int y, int z, int edgeName) const;
	int GenerateVertex(int x, int y, int z, int edgeName);
	inline int GetVertexId(int x, int y, int z) const;
	int FindIntersection(float *orgCoord, float *destCoord);
	void RenameElements(void);
	bool GenerateNormals(void);
public:
	std::map<int, int> m_vertexToEdgeMap;
	static const int m_cubeCaseToEdgeBitFieldArray[256];
	static const int m_triangleTable[256][22];
#ifdef MY_REMOVING_EDGE_GROUP_2
	static const int m_needCutVertexOnCellCentroidArray[256];
#endif // #define MY_REMOVING_EDGE_GROUP_2
};

/**
*/
template <typename T>
const int CSimpleMarchingCubes<T>::m_cubeCaseToEdgeBitFieldArray[256] = {
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

template <typename T>
const int CSimpleMarchingCubes<T>::m_triangleTable[256][22] = {
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 0
	{ 0,  8,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 1
	{ 0,  1,  9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 2
	{ 1,  8,  3,  9,  8,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 3
	{ 1,  2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 4
	{ 0,  8,  3,  1,  2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 5
	{ 9,  2, 10,  0,  2,  9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 6
#ifdef MY_USING_NEW_EDGE_CASES
	{ 3,  9,  8,  3,  2,  9,  2, 10,  9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 7
#else // !MY_USING_NEW_EDGE_CASES
	{ 2,  8,  3,  2, 10,  8, 10,  9,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 7
#endif // MY_USING_NEW_EDGE_CASES
	{ 3, 11,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 8
	{ 0, 11,  2,  8, 11,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 9
	{ 1,  9,  0,  2,  3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 10
#ifdef MY_USING_NEW_EDGE_CASES
	{ 2,  8, 11,  2,  1,  8,  1,  9,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 11
#else // !MY_USING_NEW_EDGE_CASES
	{ 1, 11,  2,  1,  9, 11,  9,  8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 11
#endif // MY_USING_NEW_EDGE_CASES
	{ 3, 10,  1, 11, 10,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 12
#ifdef MY_USING_NEW_EDGE_CASES
	{ 1, 11, 10,  1,  0, 11,  0,  8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 13
#else // !MY_USING_NEW_EDGE_CASES
	{ 0, 10,  1,  0,  8, 10,  8, 11, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 13
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 0, 10,  9,  0,  3, 10,  3, 11, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 14
#else // !MY_USING_NEW_EDGE_CASES
	{ 3,  9,  0,  3, 11,  9, 11, 10,  9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 14
#endif // MY_USING_NEW_EDGE_CASES
	{ 9,  8, 10, 10,  8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 15
	{ 4,  7,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 16
	{ 4,  3,  0,  7,  3,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 17
	{ 0,  1,  9,  8,  4,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 18
#ifdef MY_USING_NEW_EDGE_CASES
	{ 9,  3,  1,  9,  4,  3,  4,  7,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 19
#else // !MY_USING_NEW_EDGE_CASES
	{ 4,  1,  9,  4,  7,  1,  7,  3,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 19
#endif // MY_USING_NEW_EDGE_CASES
	{ 1,  2, 10,  8,  4,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 20 
	{ 3,  4,  7,  3,  0,  4,  1,  2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 21
	{ 9,  2, 10,  9,  0,  2,  8,  4,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 22
#ifdef MY_USING_NEW_EDGE_CASES
	{ 2, 10,  9,  4,  7,  2,  2,  7,  3,  4,  2,  9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 23
#else // !MY_USING_NEW_EDGE_CASES
	{ 2, 10,  9,  2,  9,  7,  2,  7,  3,  7,  9,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 23
#endif // MY_USING_NEW_EDGE_CASES
	{ 8,  4,  7,  3, 11,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 24
#ifdef MY_USING_NEW_EDGE_CASES
	{ 7,  0,  4,  7, 11,  0, 11,  2,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 25
#else // !MY_USING_NEW_EDGE_CASES
	{11,  4,  7, 11,  2,  4,  2,  0,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 25
#endif // MY_USING_NEW_EDGE_CASES
	{ 9,  0,  1,  8,  4,  7,  2,  3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 26
#ifndef MY_REMOVING_EDGE_GROUP_2
	{ 4,  7, 11,  9,  4, 11,  9, 11,  2,  9,  2,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 27
#else // MY_REMOVING_EDGE_GROUP_2
	{ 9,  4, 12,  4,  7, 12,  7, 11, 12, 11,  2, 12,  2,  1, 12,  1,  9, 12, -1, -1, -1, -1}, // 27 (Hexagon).
#endif // #ifndef MY_REMOVING_EDGE_GROUP_2
	{ 3, 10,  1,  3, 11, 10,  7,  8,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 28
#ifdef MY_USING_NEW_EDGE_CASES
	{ 1, 11, 10,  7, 11,  1,  1,  0,  4,  7,  1,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 29
#else // !MY_USING_NEW_EDGE_CASES
	{ 1, 11, 10,  1,  4, 11,  1,  0,  4,  7, 11,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 29
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 4,  7,  8,  3, 11, 10,  3, 10,  0,  0, 10,  9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 30
#else // !MY_USING_NEW_EDGE_CASES
	{ 4,  7,  8,  9,  0, 11,  9, 11, 10, 11,  0,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 30
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 7, 11, 10,  7, 10,  4,  4, 10,  9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 31
#else // !MY_USING_NEW_EDGE_CASES
	{ 4,  7, 11,  4, 11,  9,  9, 11, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 31
#endif // MY_USING_NEW_EDGE_CASES
	{ 9,  5,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 32
	{ 9,  5,  4,  0,  8,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 33
	{ 0,  5,  4,  1,  5,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 34
#ifdef MY_USING_NEW_EDGE_CASES
	{ 4,  1,  5,  4,  8,  1,  8,  3,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 35
#else // !MY_USING_NEW_EDGE_CASES
	{ 8,  5,  4,  8,  3,  5,  3,  1,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 35
#endif // MY_USING_NEW_EDGE_CASES
	{ 1,  2, 10,  9,  5,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 36
	{ 3,  0,  8,  1,  2, 10,  4,  9,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 37
#ifdef MY_USING_NEW_EDGE_CASES
	{10,  0,  2, 10,  5,  0,  5,  4,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 38
#else // !MY_USING_NEW_EDGE_CASES
	{ 5,  2, 10,  5,  4,  2,  4,  0,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 38
#endif // MY_USING_NEW_EDGE_CASES
#ifndef MY_REMOVING_EDGE_GROUP_2
	{ 2, 10,  5,  3,  2,  5,  3,  5,  4,  3,  4,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 39
#else // MY_REMOVING_EDGE_GROUP_2
	{10,  5, 12,  5,  4, 12,  4,  8, 12,  8,  3, 12,  3,  2, 12,  2, 10, 12, -1, -1, -1, -1}, // 39 (Hexagon).
#endif // #ifndef MY_REMOVING_EDGE_GROUP_2
	{ 9,  5,  4,  2,  3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 40
	{ 0, 11,  2,  0,  8, 11,  4,  9,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 41
	{ 0,  5,  4,  0,  1,  5,  2,  3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 42
#ifdef MY_USING_NEW_EDGE_CASES
	{ 2,  1,  5,  4,  8,  2,  2,  8, 11,  4,  2,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 43
#else // !MY_USING_NEW_EDGE_CASES
	{ 2,  1,  5,  2,  5,  8,  2,  8, 11,  4,  8,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 43
#endif // MY_USING_NEW_EDGE_CASES
	{10,  3, 11, 10,  1,  3,  9,  5,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 44
#ifdef MY_USING_NEW_EDGE_CASES
	{ 4,  9,  5,  0,  8, 11,  0, 11,  1,  1, 11, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 45
#else // !MY_USING_NEW_EDGE_CASES
	{ 4,  9,  5,  0,  8,  1,  8, 10,  1,  8, 11, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 45
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 5,  4,  0,  3, 11,  5,  5, 11, 10,  3,  5,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 46
#else // !MY_USING_NEW_EDGE_CASES
	{ 5,  4,  0,  5,  0, 11,  5, 11, 10, 11,  0,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 46
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 4,  8, 11,  4, 11,  5,  5, 11, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 47
#else // !MY_USING_NEW_EDGE_CASES
	{ 5,  4,  8,  5,  8, 10, 10,  8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 47
#endif // MY_USING_NEW_EDGE_CASES
	{ 9,  7,  8,  5,  7,  9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 48
#ifdef MY_USING_NEW_EDGE_CASES
	{ 0,  7,  3,  0,  9,  7,  9,  5,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 49
#else // !MY_USING_NEW_EDGE_CASES
	{ 9,  3,  0,  9,  5,  3,  5,  7,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 49
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 8,  5,  7,  8,  0,  5,  0,  1,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 50
#else // !MY_USING_NEW_EDGE_CASES
	{ 0,  7,  8,  0,  1,  7,  1,  5,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 50
#endif // MY_USING_NEW_EDGE_CASES
	{ 1,  5,  3,  3,  5,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 51
	{ 9,  7,  8,  9,  5,  7, 10,  1,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 52
#ifdef MY_USING_NEW_EDGE_CASES
	{10,  1,  2,  9,  5,  7,  9,  7,  0,  0,  7,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 53
#else // !MY_USING_NEW_EDGE_CASES
	{10,  1,  2,  9,  5,  0,  5,  3,  0,  5,  7,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 53
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 8,  0,  2, 10,  5,  8,  8,  5,  7, 10,  8,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 54
#else // !MY_USING_NEW_EDGE_CASES
	{ 8,  0,  2,  8,  2,  5,  8,  5,  7, 10,  5,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 54
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{10,  5,  7, 10,  7,  2,  2,  7,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 55
#else // !MY_USING_NEW_EDGE_CASES
	{ 2, 10,  5,  2,  5,  3,  3,  5,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 55
#endif // MY_USING_NEW_EDGE_CASES
	{ 7,  9,  5,  7,  8,  9,  3, 11,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 56
#ifdef MY_USING_NEW_EDGE_CASES
	{ 9,  5,  7, 11,  2,  9,  9,  2,  0, 11,  9,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 57
#else // !MY_USING_NEW_EDGE_CASES
	{ 9,  5,  7,  9,  7,  2,  9,  2,  0,  2,  7, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 57
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 2,  3, 11,  0,  1,  5,  0,  5,  8,  8,  5,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 58
#else // !MY_USING_NEW_EDGE_CASES
	{ 2,  3, 11,  0,  1,  8,  1,  7,  8,  1,  5,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 58
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 2,  1,  5,  2,  5, 11, 11,  5,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 59
#else // !MY_USING_NEW_EDGE_CASES
	{11,  2,  1, 11,  1,  7,  7,  1,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 59
#endif // MY_USING_NEW_EDGE_CASES
	{ 9,  5,  8,  8,  5,  7, 10,  1,  3, 10,  3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 60
#ifdef MY_USING_NEW_EDGE_CASES
	{ 9,  5,  7,  9,  7,  0,  7, 11,  0,  0, 11,  1,  1, 11, 10, -1, -1, -1, -1, -1, -1, -1}, // (new) 61
#else // !MY_USING_NEW_EDGE_CASES
	{ 5,  7,  0,  5,  0,  9,  7, 11,  0,  1,  0, 10, 11, 10,  0, -1, -1, -1, -1, -1, -1, -1}, // 61
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 3, 11, 10,  3, 10,  0, 10,  5,  0,  0,  5,  8,  8,  5,  7, -1, -1, -1, -1, -1, -1, -1}, // (new) 62
#else // !MY_USING_NEW_EDGE_CASES
	{11, 10,  0, 11,  0,  3, 10,  5,  0,  8,  0,  7,  5,  7,  0, -1, -1, -1, -1, -1, -1, -1}, // 62
#endif // MY_USING_NEW_EDGE_CASES
	{11, 10,  5,  7, 11,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 63
	{10,  6,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 64
	{ 0,  8,  3,  5, 10,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 65
	{ 9,  0,  1,  5, 10,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 66
	{ 1,  8,  3,  1,  9,  8,  5, 10,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 67
	{ 1,  6,  5,  2,  6,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 68
	{ 1,  6,  5,  1,  2,  6,  3,  0,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 69
#ifdef MY_USING_NEW_EDGE_CASES
	{ 5,  2,  6,  5,  9,  2,  9,  0,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 70
#else // !MY_USING_NEW_EDGE_CASES
	{ 9,  6,  5,  9,  0,  6,  0,  2,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 70
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 5,  9,  8,  3,  2,  5,  5,  2,  6,  3,  5,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 71
#else // !MY_USING_NEW_EDGE_CASES
	{ 5,  9,  8,  5,  8,  2,  5,  2,  6,  3,  2,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 71
#endif // MY_USING_NEW_EDGE_CASES
	{ 2,  3, 11, 10,  6,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 72
	{11,  0,  8, 11,  2,  0, 10,  6,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 73
	{ 0,  1,  9,  2,  3, 11,  5, 10,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 74
#ifdef MY_USING_NEW_EDGE_CASES
	{ 5, 10,  6,  1,  9,  8,  1,  8,  2,  2,  8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 75
#else // !MY_USING_NEW_EDGE_CASES
	{ 5, 10,  6,  1,  9,  2,  9, 11,  2,  9,  8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 75
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{11,  1,  3, 11,  6,  1,  6,  5,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 76
#else // !MY_USING_NEW_EDGE_CASES
	{ 6,  3, 11,  6,  5,  3,  5,  1,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 76
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 0,  8, 11,  6,  5,  0,  0,  5,  1,  6,  0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 77
#else // !MY_USING_NEW_EDGE_CASES
	{ 0,  8, 11,  0, 11,  5,  0,  5,  1,  5, 11,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 77
#endif // MY_USING_NEW_EDGE_CASES
#ifndef MY_REMOVING_EDGE_GROUP_2
	{ 3, 11,  6,  0,  3,  6,  0,  6,  5,  0,  5,  9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 78
#else // MY_REMOVING_EDGE_GROUP_2
	{ 0,  3, 12,  3, 11, 12, 11,  6, 12,  6,  5, 12,  5,  9, 12,  9,  0, 12, -1, -1, -1, -1}, // 78 (Hexagon).
#endif // #ifndef MY_REMOVING_EDGE_GROUP_2
#ifdef MY_USING_NEW_EDGE_CASES
	{ 5,  9,  8,  5,  8,  6,  6,  8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 79
#else // !MY_USING_NEW_EDGE_CASES
	{ 6,  5,  9,  6,  9, 11, 11,  9,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 79
#endif // MY_USING_NEW_EDGE_CASES
	{ 5, 10,  6,  4,  7,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 80
	{ 4,  3,  0,  4,  7,  3,  6,  5, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 81
	{ 1,  9,  0,  5, 10,  6,  8,  4,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 82
#ifdef MY_USING_NEW_EDGE_CASES
	{10,  6,  5,  4,  7,  3,  4,  3,  9,  9,  3,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 83
#else // !MY_USING_NEW_EDGE_CASES
	{10,  6,  5,  1,  9,  7,  1,  7,  3,  7,  9,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 83
#endif // MY_USING_NEW_EDGE_CASES
	{ 6,  1,  2,  6,  5,  1,  4,  7,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 84
	{ 1,  2,  5,  5,  2,  6,  3,  0,  4,  3,  4,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 85
#ifdef MY_USING_NEW_EDGE_CASES
	{ 8,  4,  7,  9,  0,  2,  9,  2,  5,  5,  2,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 86
#else // !MY_USING_NEW_EDGE_CASES
	{ 8,  4,  7,  9,  0,  5,  0,  6,  5,  0,  2,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 86
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 4,  7,  3,  4,  3,  9,  3,  2,  9,  9,  2,  5,  5,  2,  6, -1, -1, -1, -1, -1, -1, -1}, // (new) 87
#else // !MY_USING_NEW_EDGE_CASES
	{ 7,  3,  9,  7,  9,  4,  3,  2,  9,  5,  9,  6,  2,  6,  9, -1, -1, -1, -1, -1, -1, -1}, // 87
#endif // MY_USING_NEW_EDGE_CASES
	{ 3, 11,  2,  7,  8,  4, 10,  6,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 88
#ifdef MY_USING_NEW_EDGE_CASES
	{ 5, 10,  6, 11,  2,  0, 11,  0,  7,  7,  0,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 89
#else // !MY_USING_NEW_EDGE_CASES
	{ 5, 10,  6,  4,  7,  2,  4,  2,  0,  2,  7, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 89
#endif // MY_USING_NEW_EDGE_CASES
	{ 0,  1,  9,  4,  7,  8,  2,  3, 11,  5, 10,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 90
#ifndef MY_REMOVING_EDGE_GROUP_2
	{ 9,  2,  1,  9, 11,  2,  9,  4, 11,  7, 11,  4,  5, 10,  6, -1, -1, -1, -1, -1, -1, -1}, // 91
#else // MY_REMOVING_EDGE_GROUP_2
	{ 2,  1, 12, 11,  2, 12,  7, 11, 12,  4,  7, 12,  9,  4, 12,  1,  9, 12,  6,  5, 10, -1}, // 91 (Hexagon and triangle).
#endif // #ifndef MY_REMOVING_EDGE_GROUP_2
#ifdef MY_USING_NEW_EDGE_CASES
	{ 8,  4,  7,  6,  5,  1,  6,  1, 11, 11,  1,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 92
#else // !MY_USING_NEW_EDGE_CASES
	{ 8,  4,  7,  3, 11,  5,  3,  5,  1,  5, 11,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 92
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 6,  5,  1,  6,  1, 11,  1,  0, 11, 11,  0,  7,  7,  0,  4, -1, -1, -1, -1, -1, -1, -1}, // (new) 93
#else // !MY_USING_NEW_EDGE_CASES
	{ 5,  1, 11,  5, 11,  6,  1,  0, 11,  7, 11,  4,  0,  4, 11, -1, -1, -1, -1, -1, -1, -1}, // 93
#endif // MY_USING_NEW_EDGE_CASES
#ifndef MY_REMOVING_EDGE_GROUP_2
	{ 0,  5,  9,  0,  6,  5,  0,  3,  6, 11,  6,  3,  8,  4,  7, -1, -1, -1, -1, -1, -1, -1}, // 94 (Hexagon).
#else // MY_REMOVING_EDGE_GROUP_2
	{ 0,  3, 12,  3, 11, 12, 11,  6, 12,  6,  5, 12,  5,  9, 12,  9,  0, 12,  8,  4,  7, -1}, // 94 (Hexagon and triangle).
#endif // #ifndef MY_REMOVING_EDGE_GROUP_2
#ifndef MY_REMOVING_EDGE_GROUP_2
	{ 6,  5,  9,  6,  9, 11,  4,  7,  9,  7, 11,  9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 95
#else // MY_REMOVING_EDGE_GROUP_2
	{ 6,  5, 12,  6, 12, 11,  4,  7, 12,  7, 11, 12,  5,  9, 12,  4, 12,  9, -1, -1, -1, -1}, // 95 (new folded-hexagon with an additional vertex).
#endif // #ifndef MY_REMOVING_EDGE_GROUP_2
	{10,  4,  9,  6,  4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 96
	{ 4, 10,  6,  4,  9, 10,  0,  8,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 97
#ifdef MY_USING_NEW_EDGE_CASES
	{ 1,  4,  0,  1, 10,  4, 10,  6,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 98
#else // !MY_USING_NEW_EDGE_CASES
	{10,  0,  1, 10,  6,  0,  6,  4,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 98
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 8,  3,  1, 10,  6,  8,  8,  6,  4, 10,  8,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 99
#else // !MY_USING_NEW_EDGE_CASES
	{ 8,  3,  1,  8,  1,  6,  8,  6,  4,  6,  1, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 99
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 9,  6,  4,  9,  1,  6,  1,  2,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 100
#else // !MY_USING_NEW_EDGE_CASES
	{ 1,  4,  9,  1,  2,  4,  2,  6,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 100
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 3,  0,  8,  1,  2,  6,  1,  6,  9,  9,  6,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 101
#else // !MY_USING_NEW_EDGE_CASES
	{ 3,  0,  8,  1,  2,  9,  2,  4,  9,  2,  6,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 101
#endif // MY_USING_NEW_EDGE_CASES
	{ 0,  2,  4,  4,  2,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 102
#ifdef MY_USING_NEW_EDGE_CASES
	{ 3,  2,  6,  3,  6,  8,  8,  6,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 103
#else // !MY_USING_NEW_EDGE_CASES
	{ 8,  3,  2,  8,  2,  4,  4,  2,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 103
#endif // MY_USING_NEW_EDGE_CASES
	{10,  4,  9, 10,  6,  4, 11,  2,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 104
	{ 0,  8,  2,  2,  8, 11,  4,  9, 10,  4, 10,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 105
#ifdef MY_USING_NEW_EDGE_CASES
	{ 3, 11,  2, 10,  6,  4, 10,  4,  1,  1,  4,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 106
#else // !MY_USING_NEW_EDGE_CASES
	{ 3, 11,  2,  0,  1,  6,  0,  6,  4,  6,  1, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 106
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{10,  6,  4, 10,  4,  1,  4,  8,  1,  1,  8,  2,  2,  8, 11, -1, -1, -1, -1, -1, -1, -1}, // (new) 107
#else // !MY_USING_NEW_EDGE_CASES
	{ 6,  4,  1,  6,  1, 10,  4,  8,  1,  2,  1, 11,  8, 11,  1, -1, -1, -1, -1, -1, -1, -1}, // 107
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 9,  6,  4, 11,  6,  9,  9,  1,  3, 11,  9,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 108
#else // !MY_USING_NEW_EDGE_CASES
	{ 9,  6,  4,  9,  3,  6,  9,  1,  3, 11,  6,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 108
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 0,  8, 11,  0, 11,  1, 11,  6,  1,  1,  6,  9,  9,  6,  4, -1, -1, -1, -1, -1, -1, -1}, // (new) 109
#else // !MY_USING_NEW_EDGE_CASES
	{ 8, 11,  1,  8,  1,  0, 11,  6,  1,  9,  1,  4,  6,  4,  1, -1, -1, -1, -1, -1, -1, -1}, // 109
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{11,  6,  4, 11,  4,  3,  3,  4,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 110
#else // !MY_USING_NEW_EDGE_CASES
	{ 3, 11,  6,  3,  6,  0,  0,  6,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 110
#endif // MY_USING_NEW_EDGE_CASES
	{ 6,  4,  8, 11,  6,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 111
#ifdef MY_USING_NEW_EDGE_CASES
	{ 6,  9, 10,  6,  7,  9,  7,  8,  9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 112
#else // !MY_USING_NEW_EDGE_CASES
	{ 7, 10,  6,  7,  8, 10,  8,  9, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 112
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 0,  7,  3,  6,  7,  0,  0,  9, 10,  6,  0, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 113
#else // !MY_USING_NEW_EDGE_CASES
	{ 0,  7,  3,  0, 10,  7,  0,  9, 10,  6,  7, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 113
#endif // MY_USING_NEW_EDGE_CASES
#ifndef MY_REMOVING_EDGE_GROUP_2
	{10,  6,  7,  1, 10,  7,  1,  7,  8,  1,  8,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 114
#else // MY_REMOVING_EDGE_GROUP_2
	{ 0,  1, 12,  1, 10, 12, 10,  6, 12,  6,  7, 12,  7,  8, 12,  8,  0, 12, -1, -1, -1, -1}, // 114 (Hexagon).
#endif // #ifndef MY_REMOVING_EDGE_GROUP_2
#ifdef MY_USING_NEW_EDGE_CASES
	{ 6,  7,  3,  6,  3, 10, 10,  3,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 115
#else // !MY_USING_NEW_EDGE_CASES
	{10,  6,  7, 10,  7,  1,  1,  7,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 115
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 1,  2,  6,  7,  8,  1,  1,  8,  9,  7,  1,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 116
#else // !MY_USING_NEW_EDGE_CASES
	{ 1,  2,  6,  1,  6,  8,  1,  8,  9,  8,  6,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 116
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 1,  2,  6,  1,  6,  9,  6,  7,  9,  9,  7,  0,  0,  7,  3, -1, -1, -1, -1, -1, -1, -1}, // (new) 117
#else // !MY_USING_NEW_EDGE_CASES
	{ 2,  6,  9,  2,  9,  1,  6,  7,  9,  0,  9,  3,  7,  3,  9, -1, -1, -1, -1, -1, -1, -1}, // 117
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 8,  0,  2,  8,  2,  7,  7,  2,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 118
#else // !MY_USING_NEW_EDGE_CASES
	{ 7,  8,  0,  7,  0,  6,  6,  0,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 118
#endif // MY_USING_NEW_EDGE_CASES
	{ 7,  3,  2,  6,  7,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 119
#ifdef MY_USING_NEW_EDGE_CASES
	{ 2,  3, 11,  7,  8,  9,  7,  9,  6,  6,  9, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 120
#else // !MY_USING_NEW_EDGE_CASES
	{ 2,  3, 11, 10,  6,  8, 10,  8,  9,  8,  6,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 120
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{11,  2,  0, 11,  0,  7,  0,  9,  7,  7,  9,  6,  6,  9, 10, -1, -1, -1, -1, -1, -1, -1}, // (new) 121
#else // !MY_USING_NEW_EDGE_CASES
	{ 2,  0,  7,  2,  7, 11,  0,  9,  7,  6,  7, 10,  9, 10,  7, -1, -1, -1, -1, -1, -1, -1}, // 121
#endif // MY_USING_NEW_EDGE_CASES
#ifndef MY_REMOVING_EDGE_GROUP_2
	{ 1,  8,  0,  1,  7,  8,  1, 10,  7,  6,  7, 10,  2,  3, 11, -1, -1, -1, -1, -1, -1, -1}, // 122
#else // MY_REMOVING_EDGE_GROUP_2
	{ 0,  1, 12,  1, 10, 12, 10,  6, 12,  6,  7, 12,  7,  8, 12,  8,  0, 12,  2,  3, 11, -1}, // 122 (Hexagon and triangle).
#endif // ifndef MY_REMOVING_EDGE_GROUP_2
#ifndef MY_REMOVING_EDGE_GROUP_2
	{11,  2,  1, 11,  1,  7, 10,  6,  1,  6,  7,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 123
#else // MY_REMOVING_EDGE_GROUP_2
	{11,  2, 12, 11, 12,  7, 10,  6, 12,  6,  7, 12,  2,  1, 12, 10, 12,  1, -1, -1, -1, -1}, // 123 (new folded-hexagon with an additional vertex).
#endif // #ifndef MY_REMOVING_EDGE_GROUP_2
#ifdef MY_USING_NEW_EDGE_CASES
	{ 7,  8,  9,  7,  9,  6,  9,  1,  6,  6,  1, 11, 11,  1,  3, -1, -1, -1, -1, -1, -1, -1}, // (new) 124
#else // !MY_USING_NEW_EDGE_CASES
	{ 8,  9,  6,  8,  6,  7,  9,  1,  6, 11,  6,  3,  1,  3,  6, -1, -1, -1, -1, -1, -1, -1}, // 124
#endif // MY_USING_NEW_EDGE_CASES
	{ 0,  9,  1, 11,  6,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 125
#ifndef MY_REMOVING_EDGE_GROUP_2
	{ 7,  8,  0,  7,  0,  6,  3, 11,  0, 11,  6,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 126
#else // MY_REMOVING_EDGE_GROUP_2
	{ 7,  8, 12,  7, 12,  6,  3, 11, 12, 11,  6, 12,  8,  0, 12,  3, 12,  0, -1, -1, -1, -1}, // 126 (new folded-hexagon with an additional vertex).
#endif // #ifndef MY_REMOVING_EDGE_GROUP_2
	{ 7, 11,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 127
	{ 7,  6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 128
	{ 3,  0,  8, 11,  7,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 129
	{ 0,  1,  9, 11,  7,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 130
	{ 8,  1,  9,  8,  3,  1, 11,  7,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 131
	{10,  1,  2,  6, 11,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 132
	{ 1,  2, 10,  3,  0,  8,  6, 11,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 133
	{ 2,  9,  0,  2, 10,  9,  6, 11,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 134
#ifdef MY_USING_NEW_EDGE_CASES
	{ 6, 11,  7,  2, 10,  9,  2,  9,  3,  3,  9,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 135
#else // !MY_USING_NEW_EDGE_CASES
	{ 6, 11,  7,  2, 10,  3, 10,  8,  3, 10,  9,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 135
#endif // MY_USING_NEW_EDGE_CASES
	{ 7,  2,  3,  6,  2,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 136
#ifdef MY_USING_NEW_EDGE_CASES
	{ 8,  2,  0,  8,  7,  2,  7,  6,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 137
#else // !MY_USING_NEW_EDGE_CASES
	{ 7,  0,  8,  7,  6,  0,  6,  2,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 137
#endif // MY_USING_NEW_EDGE_CASES
	{ 2,  7,  6,  2,  3,  7,  0,  1,  9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 138
#ifdef MY_USING_NEW_EDGE_CASES
	{ 1,  6,  2,  7,  6,  1,  1,  9,  8,  7,  1,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 139
#else // !MY_USING_NEW_EDGE_CASES
	{ 1,  6,  2,  1,  8,  6,  1,  9,  8,  8,  7,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 139
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 6,  3,  7,  6, 10,  3, 10,  1,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 140
#else // !MY_USING_NEW_EDGE_CASES
	{10,  7,  6, 10,  1,  7,  1,  3,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 140
#endif // MY_USING_NEW_EDGE_CASES
#ifndef MY_REMOVING_EDGE_GROUP_2
	{10,  7,  6,  1,  7, 10,  1,  8,  7,  1,  0,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 141
#else // MY_REMOVING_EDGE_GROUP_2
	{ 8,  7, 12,  7,  6, 12,  6, 10, 12, 10,  1, 12,  1,  0, 12,  0,  8, 12, -1, -1, -1, -1}, // 141 (Hexagon).
#endif // #ifndef MY_REMOVING_EDGE_GROUP_2
#ifdef MY_USING_NEW_EDGE_CASES
	{ 0,  3,  7,  6, 10,  0,  0, 10,  9,  6,  0,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 142
#else // !MY_USING_NEW_EDGE_CASES
	{ 0,  3,  7,  0,  7, 10,  0, 10,  9,  6, 10,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 142
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 6, 10,  9,  6,  9,  7,  7,  9,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 143
#else // !MY_USING_NEW_EDGE_CASES
	{ 7,  6, 10,  7, 10,  8,  8, 10,  9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 143
#endif // MY_USING_NEW_EDGE_CASES
	{ 6,  8,  4, 11,  8,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 144
#ifdef MY_USING_NEW_EDGE_CASES
	{11,  4,  6, 11,  3,  4,  3,  0,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 145
#else // !MY_USING_NEW_EDGE_CASES
	{ 3,  6, 11,  3,  0,  6,  0,  4,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 145
#endif // MY_USING_NEW_EDGE_CASES
	{ 8,  6, 11,  8,  4,  6,  9,  0,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 146
#ifdef MY_USING_NEW_EDGE_CASES
	{ 9,  4,  6, 11,  3,  9,  9,  3,  1, 11,  9,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 147
#else // !MY_USING_NEW_EDGE_CASES
	{ 9,  4,  6,  9,  6,  3,  9,  3,  1, 11,  3,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 147
#endif // MY_USING_NEW_EDGE_CASES
	{ 6,  8,  4,  6, 11,  8,  2, 10,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 148
#ifdef MY_USING_NEW_EDGE_CASES
	{ 1,  2, 10,  3,  0,  4,  3,  4, 11, 11,  4,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 149
#else // !MY_USING_NEW_EDGE_CASES
	{ 1,  2, 10,  3,  0, 11,  0,  6, 11,  0,  4,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 149
#endif // MY_USING_NEW_EDGE_CASES
	{ 4, 11,  8,  4,  6, 11,  0,  2,  9,  2, 10,  9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 150
#ifdef MY_USING_NEW_EDGE_CASES
	{ 2, 10,  9,  2,  9,  3,  9,  4,  3,  3,  4, 11, 11,  4,  6, -1, -1, -1, -1, -1, -1, -1}, // (new) 151
#else // !MY_USING_NEW_EDGE_CASES
	{10,  9,  3, 10,  3,  2,  9,  4,  3, 11,  3,  6,  4,  6,  3, -1, -1, -1, -1, -1, -1, -1}, // 151
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 3,  6,  2,  3,  8,  6,  8,  4,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 152
#else // !MY_USING_NEW_EDGE_CASES
	{ 8,  2,  3,  8,  4,  2,  4,  6,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 152
#endif // MY_USING_NEW_EDGE_CASES
	{ 0,  4,  2,  4,  6,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 153
#ifdef MY_USING_NEW_EDGE_CASES
	{ 1,  9,  0,  8,  4,  6,  8,  6,  3,  3,  6,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 154
#else // !MY_USING_NEW_EDGE_CASES
	{ 1,  9,  0,  2,  3,  4,  2,  4,  6,  4,  3,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 154
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 9,  4,  6,  9,  6,  1,  1,  6,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 155
#else // !MY_USING_NEW_EDGE_CASES
	{ 1,  9,  4,  1,  4,  2,  2,  4,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 155
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 8,  1,  3, 10,  1,  8,  8,  4,  6, 10,  8,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 156
#else // !MY_USING_NEW_EDGE_CASES
	{ 8,  1,  3,  8,  6,  1,  8,  4,  6,  6, 10,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 156
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 1,  0,  4,  1,  4, 10, 10,  4,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 157
#else // !MY_USING_NEW_EDGE_CASES
	{10,  1,  0, 10,  0,  6,  6,  0,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 157
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 8,  4,  6,  8,  6,  3,  6, 10,  3,  3, 10,  0,  0, 10,  9, -1, -1, -1, -1, -1, -1, -1}, // (new) 158
#else // !MY_USING_NEW_EDGE_CASES
	{ 4,  6,  3,  4,  3,  8,  6, 10,  3,  0,  3,  9, 10,  9,  3, -1, -1, -1, -1, -1, -1, -1}, // 158
#endif // MY_USING_NEW_EDGE_CASES
	{10,  9,  4,  6, 10,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 159
	{ 4,  9,  5,  7,  6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 160
	{ 0,  8,  3,  4,  9,  5, 11,  7,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 161
	{ 5,  0,  1,  5,  4,  0,  7,  6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 162
#ifdef MY_USING_NEW_EDGE_CASES
	{11,  7,  6,  8,  3,  1,  8,  1,  4,  4,  1,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 163
#else // !MY_USING_NEW_EDGE_CASES
	{11,  7,  6,  8,  3,  4,  3,  5,  4,  3,  1,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 163
#endif // MY_USING_NEW_EDGE_CASES
	{ 9,  5,  4, 10,  1,  2,  7,  6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 164
	{ 6, 11,  7,  1,  2, 10,  0,  8,  3,  4,  9,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 165
#ifdef MY_USING_NEW_EDGE_CASES
	{ 7,  6, 11,  5,  4,  0,  5,  0, 10, 10,  0,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 166
#else // !MY_USING_NEW_EDGE_CASES
	{ 7,  6, 11,  5,  4, 10,  4,  2, 10,  4,  0,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 166
#endif // MY_USING_NEW_EDGE_CASES
#ifndef MY_REMOVING_EDGE_GROUP_2
	{ 3,  4,  8,  3,  5,  4,  3,  2,  5, 10,  5,  2, 11,  7,  6, -1, -1, -1, -1, -1, -1, -1}, // 167
#else // MY_REMOVING_EDGE_GROUP_2
	{ 3,  2, 12,  8,  3, 12,  4,  8, 12,  5,  4, 12, 10,  5, 12,  2, 10, 12,  7,  6, 11, -1}, // 167 (Hexagon and triangle).
#endif // #ifndef MY_REMOVING_EDGE_GROUP_2
	{ 7,  2,  3,  7,  6,  2,  5,  4,  9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 168
#ifdef MY_USING_NEW_EDGE_CASES
	{ 9,  5,  4,  7,  6,  2,  7,  2,  8,  8,  2,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 169
#else // !MY_USING_NEW_EDGE_CASES
	{ 9,  5,  4,  0,  8,  6,  0,  6,  2,  6,  8,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 169
#endif // MY_USING_NEW_EDGE_CASES
	{ 3,  6,  2,  3,  7,  6,  1,  5,  0,  5,  4,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 170
#ifdef MY_USING_NEW_EDGE_CASES
	{ 6,  2,  7,  7,  2,  8,  2,  1,  8,  8,  1,  4,  4,  1,  5, -1, -1, -1, -1, -1, -1, -1}, // (new) 171
#else // !MY_USING_NEW_EDGE_CASES
	{ 6,  2,  8,  6,  8,  7,  2,  1,  8,  4,  8,  5,  1,  5,  8, -1, -1, -1, -1, -1, -1, -1}, // 171
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 9,  5,  4, 10,  1,  3, 10,  3,  6,  6,  3,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 172
#else // !MY_USING_NEW_EDGE_CASES
	{ 9,  5,  4, 10,  1,  6,  1,  7,  6,  1,  3,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 172
#endif // MY_USING_NEW_EDGE_CASES
#ifndef MY_REMOVING_EDGE_GROUP_2
	{ 1,  6, 10,  1,  7,  6,  1,  0,  7,  8,  7,  0,  9,  5,  4, -1, -1, -1, -1, -1, -1, -1}, // 173
#else // MY_REMOVING_EDGE_GROUP_2
	{ 1,  0, 12, 10,  1, 12,  6, 10, 12,  7,  6, 12,  8,  7, 12,  0,  8, 12,  5,  4,  9, -1}, // 173 (Hexagon and triangle).
#endif // #ifndef MY_REMOVING_EDGE_GROUP_2
#ifdef MY_USING_NEW_EDGE_CASES
	{ 4,  0,  5,  5,  0, 10,  0,  3, 10, 10,  3,  6,  6,  3,  7, -1, -1, -1, -1, -1, -1, -1}, // (new) 174
#else // !MY_USING_NEW_EDGE_CASES
	{ 4,  0, 10,  4, 10,  5,  0,  3, 10,  6, 10,  7,  3,  7, 10, -1, -1, -1, -1, -1, -1, -1}, // 174
#endif // MY_USING_NEW_EDGE_CASES
#ifndef MY_REMOVING_EDGE_GROUP_2
	{ 7,  6, 10,  7, 10,  8,  5,  4, 10,  4,  8, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 175
#else // MY_REMOVING_EDGE_GROUP_2
	{ 7,  6, 12,  7, 12,  8,  5,  4, 12,  4,  8, 12,  6, 10, 12,  5, 12, 10, -1, -1, -1, -1}, // 175 (new folded-hexagon with an additional vertex).
#endif // #ifndef MY_REMOVING_EDGE_GROUP_2
#ifdef MY_USING_NEW_EDGE_CASES
	{ 5,  8,  9,  5,  6,  8,  6, 11,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 176
#else // !MY_USING_NEW_EDGE_CASES
	{ 6,  9,  5,  6, 11,  9, 11,  8,  9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 176
#endif // MY_USING_NEW_EDGE_CASES
#ifndef MY_REMOVING_EDGE_GROUP_2
	{ 3,  6, 11,  0,  6,  3,  0,  5,  6,  0,  9,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 177
#else // MY_REMOVING_EDGE_GROUP_2
	{ 9,  5, 12,  5,  6, 12,  6, 11, 12, 11,  3, 12,  3,  0, 12,  0,  9, 12, -1, -1, -1, -1}, // 177 (Hexagon).
#endif // #ifndef MY_REMOVING_EDGE_GROUP_2
#ifdef MY_USING_NEW_EDGE_CASES
	{ 0, 11,  8,  6, 11,  0,  0,  1,  5,  6,  0,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 178
#else // !MY_USING_NEW_EDGE_CASES
	{ 0, 11,  8,  0,  5, 11,  0,  1,  5,  5,  6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 178
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{11,  3,  1, 11,  1,  6,  6,  1,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 179
#else // !MY_USING_NEW_EDGE_CASES
	{ 6, 11,  3,  6,  3,  5,  5,  3,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 179
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 1,  2, 10,  6, 11,  8,  6,  8,  5,  5,  8,  9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 180
#else // !MY_USING_NEW_EDGE_CASES
	{ 1,  2, 10,  9,  5, 11,  9, 11,  8, 11,  5,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 180
#endif // MY_USING_NEW_EDGE_CASES
#ifndef MY_REMOVING_EDGE_GROUP_2
	{ 0, 11,  3,  0,  6, 11,  0,  9,  6,  5,  6,  9,  1,  2, 10, -1, -1, -1, -1, -1, -1, -1}, // 181
#else // MY_REMOVING_EDGE_GROUP_2
	{ 0,  3, 12,  3, 11, 12, 11,  6, 12,  6,  5, 12,  5,  9, 12,  9,  0, 12,  1,  2, 10, -1}, // 181 (Hexagon and triangle).
#endif // #ifndef MY_REMOVING_EDGE_GROUP_2
#ifdef MY_USING_NEW_EDGE_CASES
	{ 6, 11,  8,  6,  8,  5,  8,  0,  5,  5,  0, 10, 10,  0,  2, -1, -1, -1, -1, -1, -1, -1}, // (new) 182
#else // !MY_USING_NEW_EDGE_CASES
	{11,  8,  5, 11,  5,  6,  8,  0,  5, 10,  5,  2,  0,  2,  5, -1, -1, -1, -1, -1, -1, -1}, // 182
#endif // MY_USING_NEW_EDGE_CASES
#ifndef MY_REMOVING_EDGE_GROUP_2
	{ 6, 11,  3,  6,  3,  5,  2, 10,  3, 10,  5,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 183
#else // MY_REMOVING_EDGE_GROUP_2
	{ 6, 11, 12,  6, 12,  5,  2, 10, 12, 10,  5, 12, 11,  3, 12,  2, 12,  3, -1, -1, -1, -1}, // 183 (new folded-hexagon with an additional vertex).
#endif // #ifndef MY_REMOVING_EDGE_GROUP_2
#ifdef MY_USING_NEW_EDGE_CASES
	{ 5,  8,  9,  3,  8,  5,  5,  6,  2,  3,  5,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 184
#else // !MY_USING_NEW_EDGE_CASES
	{ 5,  8,  9,  5,  2,  8,  5,  6,  2,  3,  8,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 184
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 5,  6,  2,  5,  2,  9,  9,  2,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 185
#else // !MY_USING_NEW_EDGE_CASES
	{ 9,  5,  6,  9,  6,  0,  0,  6,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 185
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 0,  1,  5,  0,  5,  8,  5,  6,  8,  8,  6,  3,  3,  6,  2, -1, -1, -1, -1, -1, -1, -1}, // (new) 186
#else // !MY_USING_NEW_EDGE_CASES
	{ 1,  5,  8,  1,  8,  0,  5,  6,  8,  3,  8,  2,  6,  2,  8, -1, -1, -1, -1, -1, -1, -1}, // 186
#endif // MY_USING_NEW_EDGE_CASES
	{ 1,  5,  6,  2,  1,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 187
#ifdef MY_USING_NEW_EDGE_CASES
	{ 1,  3, 10, 10,  3,  6,  3,  8,  6,  6,  8,  5,  5,  8,  9, -1, -1, -1, -1, -1, -1, -1}, // (new) 188
#else // !MY_USING_NEW_EDGE_CASES
	{ 1,  3,  6,  1,  6, 10,  3,  8,  6,  5,  6,  9,  8,  9,  6, -1, -1, -1, -1, -1, -1, -1}, // 188
#endif // MY_USING_NEW_EDGE_CASES
#ifndef MY_REMOVING_EDGE_GROUP_2
	{10,  1,  0, 10,  0,  6,  9,  5,  0,  5,  6,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 189
#else // MY_REMOVING_EDGE_GROUP_2
	{10,  1, 12, 10, 12,  6,  9,  5, 12,  5,  6, 12,  1,  0, 12,  9, 12,  0, -1, -1, -1, -1}, // 189 (new folded-hexagon with an additional vertex).
#endif // #ifndef MY_REMOVING_EDGE_GROUP_2
	{ 0,  3,  8,  5,  6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 190
	{10,  5,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 191
	{11,  5, 10,  7,  5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 192
	{11,  5, 10, 11,  7,  5,  8,  3,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 193
	{ 5, 11,  7,  5, 10, 11,  1,  9,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 194
	{10,  7,  5, 10, 11,  7,  9,  8,  1,  8,  3,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 195
#ifdef MY_USING_NEW_EDGE_CASES
	{ 2,  5,  1,  2, 11,  5, 11,  7,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 196
#else // !MY_USING_NEW_EDGE_CASES
	{11,  1,  2, 11,  7,  1,  7,  5,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 196
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 0,  8,  3, 11,  7,  5, 11,  5,  2,  2,  5,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 197
#else // !MY_USING_NEW_EDGE_CASES
	{ 0,  8,  3,  1,  2,  7,  1,  7,  5,  7,  2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 197
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 9,  7,  5, 11,  7,  9,  9,  0,  2, 11,  9,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 198
#else // !MY_USING_NEW_EDGE_CASES
	{ 9,  7,  5,  9,  2,  7,  9,  0,  2,  2, 11,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 198
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{11,  7,  5, 11,  5,  2,  5,  9,  2,  2,  9,  3,  3,  9,  8, -1, -1, -1, -1, -1, -1, -1}, // (new) 199
#else // !MY_USING_NEW_EDGE_CASES
	{ 7,  5,  2,  7,  2, 11,  5,  9,  2,  3,  2,  8,  9,  8,  2, -1, -1, -1, -1, -1, -1, -1}, // 199
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{10,  7,  5, 10,  2,  7,  2,  3,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 200
#else // !MY_USING_NEW_EDGE_CASES
	{ 2,  5, 10,  2,  3,  5,  3,  7,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 200
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 8,  2,  0, 10,  2,  8,  8,  7,  5, 10,  8,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 201
#else // !MY_USING_NEW_EDGE_CASES
	{ 8,  2,  0,  8,  5,  2,  8,  7,  5, 10,  2,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 201
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 9,  0,  1,  2,  3,  7,  2,  7, 10, 10,  7,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 202
#else // !MY_USING_NEW_EDGE_CASES
	{ 9,  0,  1,  5, 10,  3,  5,  3,  7,  3, 10,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 202
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 1,  9,  8,  1,  8,  2,  8,  7,  2,  2,  7, 10, 10,  7,  5, -1, -1, -1, -1, -1, -1, -1}, // (new) 203
#else // !MY_USING_NEW_EDGE_CASES
	{ 9,  8,  2,  9,  2,  1,  8,  7,  2, 10,  2,  5,  7,  5,  2, -1, -1, -1, -1, -1, -1, -1}, // 203
#endif // MY_USING_NEW_EDGE_CASES
	{ 1,  3,  5,  3,  7,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 204
#ifdef MY_USING_NEW_EDGE_CASES
	{ 8,  7,  5,  8,  5,  0,  0,  5,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 205
#else // !MY_USING_NEW_EDGE_CASES
	{ 0,  8,  7,  0,  7,  1,  1,  7,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 205
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 0,  3,  7,  0,  7,  9,  9,  7,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 206
#else // !MY_USING_NEW_EDGE_CASES
	{ 9,  0,  3,  9,  3,  5,  5,  3,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 206
#endif // MY_USING_NEW_EDGE_CASES
	{ 9,  8,  7,  5,  9,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 207
#ifdef MY_USING_NEW_EDGE_CASES
	{ 4, 11,  8,  4,  5, 11,  5, 10, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 208
#else // !MY_USING_NEW_EDGE_CASES
	{ 5,  8,  4,  5, 10,  8, 10, 11,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 208
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 5,  0,  4,  3,  0,  5,  5, 10, 11,  3,  5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 209
#else // !MY_USING_NEW_EDGE_CASES
	{ 5,  0,  4,  5, 11,  0,  5, 10, 11, 11,  3,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 209
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 0,  1,  9,  5, 10, 11,  5, 11,  4,  4, 11,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 210
#else // !MY_USING_NEW_EDGE_CASES
	{ 0,  1,  9,  8,  4, 10,  8, 10, 11, 10,  4,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 210
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 5, 10, 11,  5, 11,  4, 11,  3,  4,  4,  3,  9,  9,  3,  1, -1, -1, -1, -1, -1, -1, -1}, // (new) 211
#else // !MY_USING_NEW_EDGE_CASES
	{10, 11,  4, 10,  4,  5, 11,  3,  4,  9,  4,  1,  3,  1,  4, -1, -1, -1, -1, -1, -1, -1}, // 211
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 2,  5,  1,  4,  5,  2,  2, 11,  8,  4,  2,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 212
#else // !MY_USING_NEW_EDGE_CASES
	{ 2,  5,  1,  2,  8,  5,  2, 11,  8,  4,  5,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 212
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 3,  0,  4,  3,  4, 11,  4,  5, 11, 11,  5,  2,  2,  5,  1, -1, -1, -1, -1, -1, -1, -1}, // (new) 213
#else // !MY_USING_NEW_EDGE_CASES
	{ 0,  4, 11,  0, 11,  3,  4,  5, 11,  2, 11,  1,  5,  1, 11, -1, -1, -1, -1, -1, -1, -1}, // 213
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 9,  0,  2,  9,  2,  5,  2, 11,  5,  5, 11,  4,  4, 11,  8, -1, -1, -1, -1, -1, -1, -1}, // (new) 214
#else // !MY_USING_NEW_EDGE_CASES
	{ 0,  2,  5,  0,  5,  9,  2, 11,  5,  4,  5,  8, 11,  8,  5, -1, -1, -1, -1, -1, -1, -1}, // 214
#endif // MY_USING_NEW_EDGE_CASES
	{ 9,  4,  5,  2, 11,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 215
#ifndef MY_REMOVING_EDGE_GROUP_2
	{ 2,  5, 10,  3,  5,  2,  3,  4,  5,  3,  8,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 216
#else // MY_REMOVING_EDGE_GROUP_2
	{ 2,  3, 12,  3,  8, 12,  8,  4, 12,  4,  5, 12,  5, 10, 12, 10,  2, 12, -1, -1, -1, -1}, // 216 (Hexagon).
#endif // #ifndef MY_REMOVING_EDGE_GROUP_2
#ifdef MY_USING_NEW_EDGE_CASES
	{10,  2,  0, 10,  0,  5,  5,  0,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 217
#else // !MY_USING_NEW_EDGE_CASES
	{ 5, 10,  2,  5,  2,  4,  4,  2,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 217
#endif // MY_USING_NEW_EDGE_CASES
#ifndef MY_REMOVING_EDGE_GROUP_2
	{ 3, 10,  2,  3,  5, 10,  3,  8,  5,  4,  5,  8,  0,  1,  9, -1, -1, -1, -1, -1, -1, -1}, // 218
#else // MY_REMOVING_EDGE_GROUP_2
	{ 2,  3, 12,  3,  8, 12,  8,  4, 12,  4,  5, 12,  5, 10, 12, 10,  2, 12,  0,  1,  9, -1}, // 218 (Hexagon and triangle).
#endif // #ifndef MY_REMOVING_EDGE_GROUP_2
#ifndef MY_REMOVING_EDGE_GROUP_2
	{ 5, 10,  2,  5,  2,  4,  1,  9,  2,  9,  4,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 219
#else // MY_REMOVING_EDGE_GROUP_2
	{ 5, 10, 12,  5, 12,  4,  1,  9, 12,  9,  4, 12, 10,  2, 12,  1, 12,  2, -1, -1, -1, -1}, // 219 (new folded-hexagon with an additional vertex).
#endif // #ifndef MY_REMOVING_EDGE_GROUP_2
#ifdef MY_USING_NEW_EDGE_CASES
	{ 4,  5,  1,  4,  1,  8,  8,  1,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 220
#else // !MY_USING_NEW_EDGE_CASES
	{ 8,  4,  5,  8,  5,  3,  3,  5,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 220
#endif // MY_USING_NEW_EDGE_CASES
	{ 0,  4,  5,  1,  0,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 221
#ifndef MY_REMOVING_EDGE_GROUP_2
	{ 8,  4,  5,  8,  5,  3,  9,  0,  5,  0,  3,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 222
#else // MY_REMOVING_EDGE_GROUP_2
	{ 8,  4, 12,  8, 12,  3,  9,  0, 12,  0,  3, 12,  4,  5, 12,  9, 12,  5, -1, -1, -1, -1}, // 222 (new folded-hexagon with an additional vertex).
#endif // #ifndef MY_REMOVING_EDGE_GROUP_2
	{ 9,  4,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 223
#ifdef MY_USING_NEW_EDGE_CASES
	{ 7, 10, 11,  7,  4, 10,  4,  9, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 224
#else // !MY_USING_NEW_EDGE_CASES
	{ 4, 11,  7,  4,  9, 11,  9, 10, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 224
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 0,  8,  3,  4,  9, 10,  4, 10,  7,  7, 10, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 225
#else // !MY_USING_NEW_EDGE_CASES
	{ 0,  8,  3,  4,  9,  7,  9, 11,  7,  9, 10, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 225
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 1, 10, 11,  1, 11,  7,  1,  7,  4,  1,  4,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 226
#else // !MY_USING_NEW_EDGE_CASES
	{ 1, 10, 11,  1, 11,  4,  1,  4,  0,  7,  4, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 226
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 8,  3,  1,  8,  1,  4,  1, 10,  4,  4, 10,  7,  7, 10, 11, -1, -1, -1, -1, -1, -1, -1}, // (new) 227
#else // !MY_USING_NEW_EDGE_CASES
	{ 3,  1,  4,  3,  4,  8,  1, 10,  4,  7,  4, 11, 10, 11,  4, -1, -1, -1, -1, -1, -1, -1}, // 227
#endif // MY_USING_NEW_EDGE_CASES
#ifndef MY_REMOVING_EDGE_GROUP_2
	{ 4, 11,  7,  9, 11,  4,  9,  2, 11,  9,  1,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 228
#else // MY_REMOVING_EDGE_GROUP_2
	{ 1,  2, 12,  2, 11, 12, 11,  7, 12,  7,  4, 12,  4,  9, 12,  9,  1, 12, -1, -1, -1, -1}, // 228
#endif // #ifndef MY_REMOVING_EDGE_GROUP_2
#ifndef MY_REMOVING_EDGE_GROUP_2
	{ 9,  7,  4,  9, 11,  7,  9,  1, 11,  2, 11,  1,  0,  8,  3, -1, -1, -1, -1, -1, -1, -1}, // 229
#else // MY_REMOVING_EDGE_GROUP_2
	{ 1,  2, 12,  2, 11, 12, 11,  7, 12,  7,  4, 12,  4,  9, 12,  9,  1, 12,  0,  8,  3, -1}, // 229 (Hexagon and triangle).
#endif // #ifndef MY_REMOVING_EDGE_GROUP_2
#ifdef MY_USING_NEW_EDGE_CASES
	{ 7,  4,  0,  7,  0, 11, 11,  0,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 230
#else // !MY_USING_NEW_EDGE_CASES
	{11,  7,  4, 11,  4,  2,  2,  4,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 230
#endif // MY_USING_NEW_EDGE_CASES
#ifndef MY_REMOVING_EDGE_GROUP_2
	{11,  7,  4, 11,  4,  2,  8,  3,  4,  3,  2,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 231
#else // MY_REMOVING_EDGE_GROUP_2
	{11,  7, 12, 11, 12,  2,  8,  3, 12,  3,  2, 12,  7,  4, 12,  8, 12,  4, -1, -1, -1, -1}, // 231 (new folded-hexagon with an additional vertex).
#endif // #ifndef MY_REMOVING_EDGE_GROUP_2
#ifdef MY_USING_NEW_EDGE_CASES
	{ 2,  9, 10,  2,  7,  4,  2,  3,  7,  4,  9,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 232
#else // !MY_USING_NEW_EDGE_CASES
	{ 2,  9, 10,  2,  7,  9,  2,  3,  7,  7,  4,  9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 232
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 4,  9, 10,  4, 10,  7, 10,  2,  7,  7,  2,  8,  8,  2,  0, -1, -1, -1, -1, -1, -1, -1}, // (new) 233
#else // !MY_USING_NEW_EDGE_CASES
	{ 9, 10,  7,  9,  7,  4, 10,  2,  7,  8,  7,  0,  2,  0,  7, -1, -1, -1, -1, -1, -1, -1}, // 233
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 2,  3,  7,  2,  7, 10,  7,  4, 10, 10,  4,  1,  1,  4,  0, -1, -1, -1, -1, -1, -1, -1}, // (new) 234
#else // !MY_USING_NEW_EDGE_CASES
	{ 3,  7, 10,  3, 10,  2,  7,  4, 10,  1, 10,  0,  4,  0, 10, -1, -1, -1, -1, -1, -1, -1}, // 234
#endif // MY_USING_NEW_EDGE_CASES
	{ 1, 10,  2,  8,  7,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 235
#ifdef MY_USING_NEW_EDGE_CASES
	{ 9,  1,  3,  9,  3,  4,  4,  3,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 236
#else // !MY_USING_NEW_EDGE_CASES
	{ 4,  9,  1,  4,  1,  7,  7,  1,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 236
#endif // MY_USING_NEW_EDGE_CASES
#ifndef MY_REMOVING_EDGE_GROUP_2
	{ 4,  9,  1,  4,  1,  7,  0,  8,  1,  8,  7,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 237
#else // MY_REMOVING_EDGE_GROUP_2
	{ 4,  9, 12,  4, 12,  7,  0,  8, 12,  8,  7, 12,  9,  1, 12,  0, 12,  1, -1, -1, -1, -1}, // 237 (new folded-hexagon with an additional vertex).
#endif // #ifndef MY_REMOVING_EDGE_GROUP_2
	{ 4,  0,  3,  7,  4,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 238
	{ 4,  8,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 239
	{ 9, 10,  8, 10, 11,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 240 
#ifdef MY_USING_NEW_EDGE_CASES
	{ 0,  9, 10,  0, 10,  3,  3, 10, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 241
#else // !MY_USING_NEW_EDGE_CASES
	{ 3,  0,  9,  3,  9, 11, 11,  9, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 241
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 1, 10, 11,  1, 11,  0,  0, 11,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 242
#else // !MY_USING_NEW_EDGE_CASES
	{ 0,  1, 10,  0, 10,  8,  8, 10, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 242
#endif // MY_USING_NEW_EDGE_CASES
	{ 3,  1, 10, 11,  3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 243
#ifdef MY_USING_NEW_EDGE_CASES
	{ 2, 11,  8,  2,  8,  1,  1,  8,  9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 244
#else // !MY_USING_NEW_EDGE_CASES
	{ 1,  2, 11,  1, 11,  9,  9, 11,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 244
#endif // MY_USING_NEW_EDGE_CASES
#ifndef MY_REMOVING_EDGE_GROUP_2
	{ 3,  0,  9,  3,  9, 11,  1,  2,  9,  2, 11,  9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 245
#else // MY_REMOVING_EDGE_GROUP_2
	{ 3,  0, 12,  3, 12, 11,  1,  2, 12,  2, 11, 12,  0,  9, 12,  1, 12,  9, -1, -1, -1, -1}, // 245 (new folded-hexagon with an additional vertex).
#endif // #ifndef MY_REMOVING_EDGE_GROUP_2
	{ 0,  2, 11,  8,  0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 246
	{ 3,  2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 247
#ifdef MY_USING_NEW_EDGE_CASES
	{ 3,  8,  9,  3,  9,  2,  2,  9, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // (new) 248
#else // !MY_USING_NEW_EDGE_CASES
	{ 2,  3,  8,  2,  8, 10, 10,  8,  9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 248
#endif // MY_USING_NEW_EDGE_CASES
	{ 9, 10,  2,  0,  9,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 249
#ifndef MY_REMOVING_EDGE_GROUP_2
	{ 2,  3,  8,  2,  8, 10,  0,  1,  8,  1, 10,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 250
#else // MY_REMOVING_EDGE_GROUP_2
	{ 2,  3, 12,  2, 12, 10,  0,  1, 12,  1, 10, 12,  3,  8, 12,  0, 12,  8, -1, -1, -1, -1}, // 250 (new folded-hexagon with an additional vertex).
#endif // #ifndef MY_REMOVING_EDGE_GROUP_2
	{ 1, 10,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 251
	{ 1,  3,  8,  9,  1,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 252
	{ 0,  9,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 253
	{ 0,  3,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 254
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1} // 255
};

#ifdef MY_REMOVING_EDGE_GROUP_2
template <typename T>
const int CSimpleMarchingCubes<T>::m_needCutVertexOnCellCentroidArray[256] = {
	0, // 0
	0, // 1
	0, // 2
	0, // 3
	0, // 4
	0, // 5
	0, // 6
	0, // 7
	0, // 8
	0, // 9
	0, // 10
	0, // 11
	0, // 12
	0, // 13
	0, // 14
	0, // 15
	0, // 16
	0, // 17
	0, // 18
	0, // 19
	0, // 20
	0, // 21
	0, // 22
	0, // 23
	0, // 24
	0, // 25
	0, // 26
	1, // 27 (hexagon).
	0, // 28
	0, // 29
	0, // 30
	0, // 31
	0, // 32
	0, // 33
	0, // 34
	0, // 35
	0, // 36
	0, // 37
	0, // 38
	1, // 39 (hexagon).
	0, // 40
	0, // 41
	0, // 42
	0, // 43
	0, // 44
	0, // 45
	0, // 46
	0, // 47
	0, // 48
	0, // 49
	0, // 50
	0, // 51
	0, // 52
	0, // 53
	0, // 54
	0, // 55
	0, // 56
	0, // 57
	0, // 58
	0, // 59
	0, // 60
	0, // 61
	0, // 62
	0, // 63
	0, // 64
	0, // 65
	0, // 66
	0, // 67
	0, // 68
	0, // 69
	0, // 70
	0, // 71
	0, // 72
	0, // 73
	0, // 74
	0, // 75
	0, // 76
	0, // 77
	1, // 78 (hexagon).
	0, // 79
	0, // 80
	0, // 81
	0, // 82
	0, // 83
	0, // 84
	0, // 85
	0, // 86
	0, // 87
	0, // 88
	0, // 89
	0, // 90
	1, // 91 (hexagon).
	0, // 92
	0, // 93
	1, // 94 (hexagon).
	1, // 95 (folded hexagon).
	0, // 96
	0, // 97
	0, // 98
	0, // 99
	0, // 100
	0, // 101
	0, // 102
	0, // 103
	0, // 104
	0, // 105
	0, // 106
	0, // 107
	0, // 108
	0, // 109
	0, // 110
	0, // 111
	0, // 112
	0, // 113
	1, // 114 (hexagon).
	0, // 115
	0, // 116
	0, // 117
	0, // 118
	0, // 119
	0, // 120
	0, // 121
	1, // 122 (hexagon).
	1, // 123 (folded hexagon).
	0, // 124
	0, // 125
	1, // 126 (folded hexagon).
	0, // 127
	0, // 128
	0, // 129
	0, // 130
	0, // 131
	0, // 132
	0, // 133
	0, // 134
	0, // 135
	0, // 136
	0, // 137
	0, // 138
	0, // 139
	0, // 140
	1, // 141 (hexagon).
	0, // 142
	0, // 143
	0, // 144
	0, // 145
	0, // 146
	0, // 147
	0, // 148
	0, // 149
	0, // 150
	0, // 151
	0, // 152
	0, // 153
	0, // 154
	0, // 155
	0, // 156
	0, // 157
	0, // 158
	0, // 159
	0, // 160
	0, // 161
	0, // 162
	0, // 163
	0, // 164
	0, // 165
	0, // 166
	1, // 167 (hexagon).
	0, // 168
	0, // 169
	0, // 170
	0, // 171
	0, // 172
	1, // 173 (hexagon).
	0, // 174
	1, // 175 (folded hexagon).
	0, // 176
	1, // 177 (hexagon).
	0, // 178
	0, // 179
	0, // 180
	1, // 181 (hexagon).
	0, // 182
	1, // 183 (folded hexagon).
	0, // 184
	0, // 185
	0, // 186
	0, // 187
	0, // 188
	1, // 189 (folded hexagon).
	0, // 190
	0, // 191
	0, // 192
	0, // 193
	0, // 194
	0, // 195
	0, // 196
	0, // 197
	0, // 198
	0, // 199
	0, // 200
	0, // 201
	0, // 202
	0, // 203
	0, // 204
	0, // 205
	0, // 206
	0, // 207
	0, // 208
	0, // 209
	0, // 210
	0, // 211
	0, // 212
	0, // 213
	0, // 214
	0, // 215
	1, // 216 (hexagon).
	0, // 217
	1, // 218 (hexagon).
	1, // 219 (folded hexagon).
	0, // 220
	0, // 221
	1, // 222 (folded hexagon).
	0, // 223
	0, // 224
	0, // 225
	0, // 226
	0, // 227
	1, // 228 (hexagon).
	1, // 229 (hexagon).
	0, // 230
	1, // 231 (folded hexagon).
	0, // 232
	0, // 233
	0, // 234
	0, // 235
	0, // 236
	1, // 237 (folded hexagon).
	0, // 238
	0, // 239
	0, // 240
	0, // 241
	0, // 242
	0, // 243
	0, // 244
	1, // 245 (folded hexagon).
	0, // 246
	0, // 247
	0, // 248
	0, // 249
	1, // 250 (folded hexagon).
	0, // 251
	0, // 252
	0, // 253
	0, // 254
	0, // 255
};
#endif // #ifndef MY_REMOVING_EDGE_GROUP_2

/**
*/
template <typename T>
CSimpleMarchingCubes<T>::CSimpleMarchingCubes(void)
{
	// Move to a new 'Create' function?
	SetupVertexPool();
}

/**
*/
template <typename T>
CSimpleMarchingCubes<T>::~CSimpleMarchingCubes(void)
{
	//DeleteMesh();
}

/**
*/
template <typename T>
bool CSimpleMarchingCubes<T>::SetCellSize(float width, float height, float depth)
{
	MarkError();

	return false;
}

/**
*/
template <typename T>
bool CSimpleMarchingCubes<T>::SetGridSize(int width, int height, int depth)
{
	MarkError();

	return false;
}

/**
*/
template <typename T>
bool CSimpleMarchingCubes<T>::GenerateMesh(float isoLevel)
{
	int width,
		height, 
		depth,
		cellType, 
		edgeType,
		lineSize,
		sliceVoxelCount,
		index,
		x, 
		y, 
		z,
		i;
	const T *valueArray;
	T valueCache[4];

	if (!m_image.get())
	{
		MarkError();

		return false;
	}

	DeleteMesh();

	// Move to a new 'Delete' function?
	m_vertexToEdgeMap.clear();

	m_isoLevel = isoLevel;

	width  = m_image->GetWidth();
	height = m_image->GetHeight();
	depth  = m_image->GetDepth();

	valueArray = (const T*)m_image->GetValueArray();

	lineSize = width;
	sliceVoxelCount = lineSize*height;

	// Thesis only!
	width--;
	height--;
	depth--;

	for (z=1; z!=(depth - 1); z++)
	{
		for (y=1; y!=(height - 1); y++)
		{
			index = z*sliceVoxelCount + y*lineSize + 1;

			valueCache[0] = valueArray[index + lineSize];
			valueCache[1] = valueArray[index];
			valueCache[2] = valueArray[index + sliceVoxelCount + lineSize];
			valueCache[3] = valueArray[index + sliceVoxelCount];

			for (x=1; x!=(width - 1); x++) 
			{
				index = z*sliceVoxelCount + y*lineSize + x;

				cellType = 0;

				if (valueCache[1] < isoLevel)
					cellType |= 1;
				if (valueCache[0] < isoLevel)
					cellType |= 2;

				valueCache[0] = valueArray[index + lineSize + 1];

				if (valueCache[0] < isoLevel)
					cellType |= 4;

				valueCache[1] = valueArray[index + 1];

				if (valueCache[1] < isoLevel)
					cellType |= 8;
				if (valueCache[3] < isoLevel)
					cellType |= 16;
				if (valueCache[2] < isoLevel)
					cellType |= 32;

				valueCache[2] = valueArray[index + sliceVoxelCount + lineSize + 1];

				if (valueCache[2] < isoLevel)
					cellType |= 64;

				valueCache[3] = valueArray[index + sliceVoxelCount + 1];

				if (valueCache[3] < isoLevel)
					cellType |= 128;

				edgeType = m_cubeCaseToEdgeBitFieldArray[cellType];

				if (edgeType != 0) 
				{
					if (edgeType & 8)
						m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 3), GenerateVertex(x, y, z, 3)));
					if (edgeType & 1)
						m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 0), GenerateVertex(x, y, z, 0)));
					if (edgeType & 256)
						m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 8), GenerateVertex(x, y, z, 8)));

					if (x == (width - 2)) 
					{
						if (edgeType & 4) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 2), GenerateVertex(x, y, z, 2)));
						if (edgeType & 2048) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 11), GenerateVertex(x, y, z, 11)));
					}
					if (y == (height - 2)) 
					{
						if (edgeType & 2) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 1), GenerateVertex(x, y, z, 1)));
						if (edgeType & 512) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 9), GenerateVertex(x, y, z, 9)));
					}
					if (z == (depth - 2)) 
					{
						if (edgeType & 16) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 4), GenerateVertex(x, y, z, 4)));
						if (edgeType & 128) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 7), GenerateVertex(x, y, z, 7)));
					}

					if ((x == (width - 2)) && (y == (height - 2)))
					{
						if (edgeType & 1024) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 10), GenerateVertex(x, y, z, 10)));
					}
					if ((x == (width - 2)) && (z == (depth - 2)))
					{
						if (edgeType & 64) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 6), GenerateVertex(x, y, z, 6)));
					}
					if ((y == (height - 2)) && (z == (depth - 2)))
					{
						if (edgeType & 32) 
							m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 5), GenerateVertex(x, y, z, 5)));
					}

					for (i=0; m_triangleTable[cellType][i]!=-1; i++)
						m_triangleMesh->m_elementArray.push_back(GetEdgeId(x, y, z, m_triangleTable[cellType][i]));

#ifdef MY_REMOVING_EDGE_GROUP_2
					if (m_needCutVertexOnCellCentroidArray[cellType])
					{
						m_vertexToEdgeMap.insert(map<int, int>::value_type(GetEdgeId(x, y, z, 12), (const int)m_triangleMesh->m_vertexArray.size()/3));

						m_triangleMesh->m_vertexArray.push_back(float(x) + 0.5f);
						m_triangleMesh->m_vertexArray.push_back(float(y) + 0.5f);
						m_triangleMesh->m_vertexArray.push_back(float(z) + 0.5f);
					}
#endif // #ifdef MY_REMOVING_EDGE_GROUP_2
				}
			}
		}
	}

	if (m_triangleMesh->GetVertexCount() != 0)
	{
		RenameElements();

		if (!MyMeshUtils::GenerateNormals(m_triangleMesh.get()))
		{
			MarkError();

			return false;
		}
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
template <typename T>
bool CSimpleMarchingCubes<T>::GenerateMeshFromSpanSpace(float isoLevel)
{
	MarkError();

	return false;
}

/**
*/
template <typename T>
void CSimpleMarchingCubes<T>::SetupVertexPool(void)
{
	m_triangleMesh->m_vertexSize = 3;

	m_triangleMesh->m_mode = CVertexPool::TRIANGLES;

	m_activeEdges->m_vertexSize = 3;

	m_activeEdges->m_mode = CVertexPool::LINES;
}

/**
*/
template <typename T>
int CSimpleMarchingCubes<T>::GenerateVertex(int x, int y, int z, int edgeName)
{
	float orgCoord[3],
		destCoord[3];

	orgCoord[0] = destCoord[0] = (float)x;
	orgCoord[1] = destCoord[1] = (float)y;
	orgCoord[2] = destCoord[2] = (float)z;

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

	if (m_edgeTransform != IMarchingMethod::NONE)
	{
		TransformEdge(orgCoord, destCoord);
		
		return IMarchingMethod::FindIntersection(orgCoord, destCoord);
	}

	return FindIntersection(orgCoord, destCoord);
}

/**
*/
template <typename T>
int CSimpleMarchingCubes<T>::FindIntersection(float *orgCoord, float *destCoord)
{
	float mu,
		orgValue,
		destValue,
		vertexCoord[3];
	int vertexIndex,
		width,
		height, 
		lineSize,
		sliceVoxelCount,
		i;
	const T *valueArray;

	width  = m_image->GetWidth();
	height = m_image->GetHeight();

	lineSize = width;
	sliceVoxelCount = lineSize*height;

	valueArray = (T*)m_image->GetValueArray();

	orgValue = (float)*(valueArray + (int)(orgCoord[2]*sliceVoxelCount + orgCoord[1]*lineSize + orgCoord[0]));
	destValue = (float)*(valueArray + (int)(destCoord[2]*sliceVoxelCount + destCoord[1]*lineSize + destCoord[0]));

	mu = (m_isoLevel - orgValue)/(destValue - orgValue);

	for (i=0; i<3; i++)
		vertexCoord[i] = orgCoord[i] + mu*(destCoord[i] - orgCoord[i]);

	vertexIndex = (int)m_triangleMesh->m_vertexArray.size()/3;

	m_triangleMesh->m_vertexArray.push_back(vertexCoord[0]);
	m_triangleMesh->m_vertexArray.push_back(vertexCoord[1]);
	m_triangleMesh->m_vertexArray.push_back(vertexCoord[2]);

	/*// BEGIN OF TEST (take normal from Gage).
	const float *vertexNormal;

	vertexNormal = m_image->GetNormal(vertexCoord[0], vertexCoord[1], vertexCoord[2]);

	m_triangleMesh->m_normalArray.push_back(-vertexNormal[0]);
	m_triangleMesh->m_normalArray.push_back(-vertexNormal[1]);
	m_triangleMesh->m_normalArray.push_back(-vertexNormal[2]);
	// END OF TEST (take normal from Gage).*/

	/*// BEGIN OF TEST (store the active edges).
	m_activeEdges->m_vertexArray.push_back((float)orgCoord[0]);
	m_activeEdges->m_vertexArray.push_back((float)orgCoord[1]);
	m_activeEdges->m_vertexArray.push_back((float)orgCoord[2]);

	m_activeEdges->m_vertexArray.push_back((float)destCoord[0]);
	m_activeEdges->m_vertexArray.push_back((float)destCoord[1]);
	m_activeEdges->m_vertexArray.push_back((float)destCoord[2]);

	m_activeEdges->m_elementArray.push_back((unsigned int)m_activeEdges->m_elementArray.size());
	m_activeEdges->m_elementArray.push_back((unsigned int)m_activeEdges->m_elementArray.size());
	// END OF TEST (store the active edges).*/

	return vertexIndex;
}

/**
*/
template <typename T>
int CSimpleMarchingCubes<T>::GetEdgeId(int x, int y, int z, int edgeName) const
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
	case 12:
		return GetVertexId(x, y, z) + 3;
	default:
		return 0;
	}
}

/**
*/
template <typename T>
int CSimpleMarchingCubes<T>::GetVertexId(int x, int y, int z) const
{
	// Some cells can generate a vertex in their centroid.
	return 4*(m_image->GetWidth()*(z*m_image->GetHeight() + y) + x);
}

/**
*/
template <typename T>
void CSimpleMarchingCubes<T>::RenameElements(void)
{
	vector<unsigned int>::iterator element;

	for (element=m_triangleMesh->m_elementArray.begin(); element!=m_triangleMesh->m_elementArray.end(); element++)
		(*element) = m_vertexToEdgeMap[(*element)];

	m_vertexToEdgeMap.clear();
}

#endif // SIMPLEMARCHINGCUBES_INCLUDED

