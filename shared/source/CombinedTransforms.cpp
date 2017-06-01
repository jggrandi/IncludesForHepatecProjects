#include <cmath>

#include <ErrorManager.h>
#include <MyMath.h>
#include <Timer.h>

#include <CombinedTransforms.h>

using namespace std;

const int CCombinedTransforms::m_cubeCaseToEdgeBitFieldArray[256] = {
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

const int CCombinedTransforms::m_triangleTable[256][16] = {
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 0
	{ 0,  8,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 1
	{ 0,  1,  9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 2
	{ 1,  8,  3,  9,  8,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 3
	{ 1,  2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 4
	{ 0,  8,  3,  1,  2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 5
	{ 9,  2, 10,  0,  2,  9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 6
#ifdef MY_USING_NEW_EDGE_CASES
	{ 3,  9,  8,  3,  2,  9,  2, 10,  9, -1, -1, -1, -1, -1, -1, -1}, // (new) 7
#else // !MY_USING_NEW_EDGE_CASES
	{ 2,  8,  3,  2, 10,  8, 10,  9,  8, -1, -1, -1, -1, -1, -1, -1}, // 7
#endif // MY_USING_NEW_EDGE_CASES
	{ 3, 11,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 8
	{ 0, 11,  2,  8, 11,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 9
	{ 1,  9,  0,  2,  3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 10
#ifdef MY_USING_NEW_EDGE_CASES
	{ 2,  8, 11,  2,  1,  8,  1,  9,  8, -1, -1, -1, -1, -1, -1, -1}, // (new) 11
#else // !MY_USING_NEW_EDGE_CASES
	{ 1, 11,  2,  1,  9, 11,  9,  8, 11, -1, -1, -1, -1, -1, -1, -1}, // 11
#endif // MY_USING_NEW_EDGE_CASES
	{ 3, 10,  1, 11, 10,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 12
#ifdef MY_USING_NEW_EDGE_CASES
	{ 1, 11, 10,  1,  0, 11,  0,  8, 11, -1, -1, -1, -1, -1, -1, -1}, // (new) 13
#else // !MY_USING_NEW_EDGE_CASES
	{ 0, 10,  1,  0,  8, 10,  8, 11, 10, -1, -1, -1, -1, -1, -1, -1}, // 13
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 0, 10,  9,  0,  3, 10,  3, 11, 10, -1, -1, -1, -1, -1, -1, -1}, // (new) 14
#else // !MY_USING_NEW_EDGE_CASES
	{ 3,  9,  0,  3, 11,  9, 11, 10,  9, -1, -1, -1, -1, -1, -1, -1}, // 14
#endif // MY_USING_NEW_EDGE_CASES
	{ 9,  8, 10, 10,  8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 15
	{ 4,  7,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 16
	{ 4,  3,  0,  7,  3,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 17
	{ 0,  1,  9,  8,  4,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 18
#ifdef MY_USING_NEW_EDGE_CASES
	{ 9,  3,  1,  9,  4,  3,  4,  7,  3, -1, -1, -1, -1, -1, -1, -1}, // (new) 19
#else // !MY_USING_NEW_EDGE_CASES
	{ 4,  1,  9,  4,  7,  1,  7,  3,  1, -1, -1, -1, -1, -1, -1, -1}, // 19
#endif // MY_USING_NEW_EDGE_CASES
	{ 1,  2, 10,  8,  4,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 20 
	{ 3,  4,  7,  3,  0,  4,  1,  2, 10, -1, -1, -1, -1, -1, -1, -1}, // 21
	{ 9,  2, 10,  9,  0,  2,  8,  4,  7, -1, -1, -1, -1, -1, -1, -1}, // 22
#ifdef MY_USING_NEW_EDGE_CASES
	{ 2, 10,  9,  4,  7,  2,  2,  7,  3,  4,  2,  9, -1, -1, -1, -1}, // (new) 23
#else // !MY_USING_NEW_EDGE_CASES
	{ 2, 10,  9,  2,  9,  7,  2,  7,  3,  7,  9,  4, -1, -1, -1, -1}, // 23
#endif // MY_USING_NEW_EDGE_CASES
	{ 8,  4,  7,  3, 11,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 24
#ifdef MY_USING_NEW_EDGE_CASES
	{ 7,  0,  4,  7, 11,  0, 11,  2,  0, -1, -1, -1, -1, -1, -1, -1}, // (new) 25
#else // !MY_USING_NEW_EDGE_CASES
	{11,  4,  7, 11,  2,  4,  2,  0,  4, -1, -1, -1, -1, -1, -1, -1}, // 25
#endif // MY_USING_NEW_EDGE_CASES
	{ 9,  0,  1,  8,  4,  7,  2,  3, 11, -1, -1, -1, -1, -1, -1, -1}, // 26
	{ 4,  7, 11,  9,  4, 11,  9, 11,  2,  9,  2,  1, -1, -1, -1, -1}, // 27
	{ 3, 10,  1,  3, 11, 10,  7,  8,  4, -1, -1, -1, -1, -1, -1, -1}, // 28
#ifdef MY_USING_NEW_EDGE_CASES
	{ 1, 11, 10,  7, 11,  1,  1,  0,  4,  7,  1,  4, -1, -1, -1, -1}, // (new) 29
#else // !MY_USING_NEW_EDGE_CASES
	{ 1, 11, 10,  1,  4, 11,  1,  0,  4,  7, 11,  4, -1, -1, -1, -1}, // 29
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 4,  7,  8,  3, 11, 10,  3, 10,  0,  0, 10,  9, -1, -1, -1, -1}, // (new) 30
#else // !MY_USING_NEW_EDGE_CASES
	{ 4,  7,  8,  9,  0, 11,  9, 11, 10, 11,  0,  3, -1, -1, -1, -1}, // 30
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 7, 11, 10,  7, 10,  4,  4, 10,  9, -1, -1, -1, -1, -1, -1, -1}, // (new) 31
#else // !MY_USING_NEW_EDGE_CASES
	{ 4,  7, 11,  4, 11,  9,  9, 11, 10, -1, -1, -1, -1, -1, -1, -1}, // 31
#endif // MY_USING_NEW_EDGE_CASES
	{ 9,  5,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 32
	{ 9,  5,  4,  0,  8,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 33
	{ 0,  5,  4,  1,  5,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 34
#ifdef MY_USING_NEW_EDGE_CASES
	{ 4,  1,  5,  4,  8,  1,  8,  3,  1, -1, -1, -1, -1, -1, -1, -1}, // (new) 35
#else // !MY_USING_NEW_EDGE_CASES
	{ 8,  5,  4,  8,  3,  5,  3,  1,  5, -1, -1, -1, -1, -1, -1, -1}, // 35
#endif // MY_USING_NEW_EDGE_CASES
	{ 1,  2, 10,  9,  5,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 36
	{ 3,  0,  8,  1,  2, 10,  4,  9,  5, -1, -1, -1, -1, -1, -1, -1}, // 37
#ifdef MY_USING_NEW_EDGE_CASES
	{10,  0,  2, 10,  5,  0,  5,  4,  0, -1, -1, -1, -1, -1, -1, -1}, // (new) 38
#else // !MY_USING_NEW_EDGE_CASES
	{ 5,  2, 10,  5,  4,  2,  4,  0,  2, -1, -1, -1, -1, -1, -1, -1}, // 38
#endif // MY_USING_NEW_EDGE_CASES
	{ 2, 10,  5,  3,  2,  5,  3,  5,  4,  3,  4,  8, -1, -1, -1, -1}, // 39
	{ 9,  5,  4,  2,  3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 40
	{ 0, 11,  2,  0,  8, 11,  4,  9,  5, -1, -1, -1, -1, -1, -1, -1}, // 41
	{ 0,  5,  4,  0,  1,  5,  2,  3, 11, -1, -1, -1, -1, -1, -1, -1}, // 42
#ifdef MY_USING_NEW_EDGE_CASES
	{ 2,  1,  5,  4,  8,  2,  2,  8, 11,  4,  2,  5, -1, -1, -1, -1}, // (new) 43
#else // !MY_USING_NEW_EDGE_CASES
	{ 2,  1,  5,  2,  5,  8,  2,  8, 11,  4,  8,  5, -1, -1, -1, -1}, // 43
#endif // MY_USING_NEW_EDGE_CASES
	{10,  3, 11, 10,  1,  3,  9,  5,  4, -1, -1, -1, -1, -1, -1, -1}, // 44
#ifdef MY_USING_NEW_EDGE_CASES
	{ 4,  9,  5,  0,  8, 11,  0, 11,  1,  1, 11, 10, -1, -1, -1, -1}, // (new) 45
#else // !MY_USING_NEW_EDGE_CASES
	{ 4,  9,  5,  0,  8,  1,  8, 10,  1,  8, 11, 10, -1, -1, -1, -1}, // 45
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 5,  4,  0,  3, 11,  5,  5, 11, 10,  3,  5,  0, -1, -1, -1, -1}, // (new) 46
#else // !MY_USING_NEW_EDGE_CASES
	{ 5,  4,  0,  5,  0, 11,  5, 11, 10, 11,  0,  3, -1, -1, -1, -1}, // 46
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 4,  8, 11,  4, 11,  5,  5, 11, 10, -1, -1, -1, -1, -1, -1, -1}, // (new) 47
#else // !MY_USING_NEW_EDGE_CASES
	{ 5,  4,  8,  5,  8, 10, 10,  8, 11, -1, -1, -1, -1, -1, -1, -1}, // 47
#endif // MY_USING_NEW_EDGE_CASES
	{ 9,  7,  8,  5,  7,  9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 48
#ifdef MY_USING_NEW_EDGE_CASES
	{ 0,  7,  3,  0,  9,  7,  9,  5,  7, -1, -1, -1, -1, -1, -1, -1}, // (new) 49
#else // !MY_USING_NEW_EDGE_CASES
	{ 9,  3,  0,  9,  5,  3,  5,  7,  3, -1, -1, -1, -1, -1, -1, -1}, // 49
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 8,  5,  7,  8,  0,  5,  0,  1,  5, -1, -1, -1, -1, -1, -1, -1}, // (new) 50
#else // !MY_USING_NEW_EDGE_CASES
	{ 0,  7,  8,  0,  1,  7,  1,  5,  7, -1, -1, -1, -1, -1, -1, -1}, // 50
#endif // MY_USING_NEW_EDGE_CASES
	{ 1,  5,  3,  3,  5,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 51
	{ 9,  7,  8,  9,  5,  7, 10,  1,  2, -1, -1, -1, -1, -1, -1, -1}, // 52
#ifdef MY_USING_NEW_EDGE_CASES
	{10,  1,  2,  9,  5,  7,  9,  7,  0,  0,  7,  3, -1, -1, -1, -1}, // (new) 53
#else // !MY_USING_NEW_EDGE_CASES
	{10,  1,  2,  9,  5,  0,  5,  3,  0,  5,  7,  3, -1, -1, -1, -1}, // 53
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 8,  0,  2, 10,  5,  8,  8,  5,  7, 10,  8,  2, -1, -1, -1, -1}, // (new) 54
#else // !MY_USING_NEW_EDGE_CASES
	{ 8,  0,  2,  8,  2,  5,  8,  5,  7, 10,  5,  2, -1, -1, -1, -1}, // 54
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{10,  5,  7, 10,  7,  2,  2,  7,  3, -1, -1, -1, -1, -1, -1, -1}, // (new) 55
#else // !MY_USING_NEW_EDGE_CASES
	{ 2, 10,  5,  2,  5,  3,  3,  5,  7, -1, -1, -1, -1, -1, -1, -1}, // 55
#endif // MY_USING_NEW_EDGE_CASES
	{ 7,  9,  5,  7,  8,  9,  3, 11,  2, -1, -1, -1, -1, -1, -1, -1}, // 56
#ifdef MY_USING_NEW_EDGE_CASES
	{ 9,  5,  7, 11,  2,  9,  9,  2,  0, 11,  9,  7, -1, -1, -1, -1}, // (new) 57
#else // !MY_USING_NEW_EDGE_CASES
	{ 9,  5,  7,  9,  7,  2,  9,  2,  0,  2,  7, 11, -1, -1, -1, -1}, // 57
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 2,  3, 11,  0,  1,  5,  0,  5,  8,  8,  5,  7, -1, -1, -1, -1}, // (new) 58
#else // !MY_USING_NEW_EDGE_CASES
	{ 2,  3, 11,  0,  1,  8,  1,  7,  8,  1,  5,  7, -1, -1, -1, -1}, // 58
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 2,  1,  5,  2,  5, 11, 11,  5,  7, -1, -1, -1, -1, -1, -1, -1}, // (new) 59
#else // !MY_USING_NEW_EDGE_CASES
	{11,  2,  1, 11,  1,  7,  7,  1,  5, -1, -1, -1, -1, -1, -1, -1}, // 59
#endif // MY_USING_NEW_EDGE_CASES
	{ 9,  5,  8,  8,  5,  7, 10,  1,  3, 10,  3, 11, -1, -1, -1, -1}, // 60
#ifdef MY_USING_NEW_EDGE_CASES
	{ 9,  5,  7,  9,  7,  0,  7, 11,  0,  0, 11,  1,  1, 11, 10, -1}, // (new) 61
#else // !MY_USING_NEW_EDGE_CASES
	{ 5,  7,  0,  5,  0,  9,  7, 11,  0,  1,  0, 10, 11, 10,  0, -1}, // 61
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 3, 11, 10,  3, 10,  0, 10,  5,  0,  0,  5,  8,  8,  5,  7, -1}, // (new) 62
#else // !MY_USING_NEW_EDGE_CASES
	{11, 10,  0, 11,  0,  3, 10,  5,  0,  8,  0,  7,  5,  7,  0, -1}, // 62
#endif // MY_USING_NEW_EDGE_CASES
	{11, 10,  5,  7, 11,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 63
	{10,  6,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 64
	{ 0,  8,  3,  5, 10,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 65
	{ 9,  0,  1,  5, 10,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 66
	{ 1,  8,  3,  1,  9,  8,  5, 10,  6, -1, -1, -1, -1, -1, -1, -1}, // 67
	{ 1,  6,  5,  2,  6,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 68
	{ 1,  6,  5,  1,  2,  6,  3,  0,  8, -1, -1, -1, -1, -1, -1, -1}, // 69
#ifdef MY_USING_NEW_EDGE_CASES
	{ 5,  2,  6,  5,  9,  2,  9,  0,  2, -1, -1, -1, -1, -1, -1, -1}, // (new) 70
#else // !MY_USING_NEW_EDGE_CASES
	{ 9,  6,  5,  9,  0,  6,  0,  2,  6, -1, -1, -1, -1, -1, -1, -1}, // 70
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 5,  9,  8,  3,  2,  5,  5,  2,  6,  3,  5,  8, -1, -1, -1, -1}, // (new) 71
#else // !MY_USING_NEW_EDGE_CASES
	{ 5,  9,  8,  5,  8,  2,  5,  2,  6,  3,  2,  8, -1, -1, -1, -1}, // 71
#endif // MY_USING_NEW_EDGE_CASES
	{ 2,  3, 11, 10,  6,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 72
	{11,  0,  8, 11,  2,  0, 10,  6,  5, -1, -1, -1, -1, -1, -1, -1}, // 73
	{ 0,  1,  9,  2,  3, 11,  5, 10,  6, -1, -1, -1, -1, -1, -1, -1}, // 74
#ifdef MY_USING_NEW_EDGE_CASES
	{ 5, 10,  6,  1,  9,  8,  1,  8,  2,  2,  8, 11, -1, -1, -1, -1}, // (new) 75
#else // !MY_USING_NEW_EDGE_CASES
	{ 5, 10,  6,  1,  9,  2,  9, 11,  2,  9,  8, 11, -1, -1, -1, -1}, // 75
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{11,  1,  3, 11,  6,  1,  6,  5,  1, -1, -1, -1, -1, -1, -1, -1}, // (new) 76
#else // !MY_USING_NEW_EDGE_CASES
	{ 6,  3, 11,  6,  5,  3,  5,  1,  3, -1, -1, -1, -1, -1, -1, -1}, // 76
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 0,  8, 11,  6,  5,  0,  0,  5,  1,  6,  0, 11, -1, -1, -1, -1}, // (new) 77
#else // !MY_USING_NEW_EDGE_CASES
	{ 0,  8, 11,  0, 11,  5,  0,  5,  1,  5, 11,  6, -1, -1, -1, -1}, // 77
#endif // MY_USING_NEW_EDGE_CASES
	{ 3, 11,  6,  0,  3,  6,  0,  6,  5,  0,  5,  9, -1, -1, -1, -1}, // 78
#ifdef MY_USING_NEW_EDGE_CASES
	{ 5,  9,  8,  5,  8,  6,  6,  8, 11, -1, -1, -1, -1, -1, -1, -1}, // (new) 79
#else // !MY_USING_NEW_EDGE_CASES
	{ 6,  5,  9,  6,  9, 11, 11,  9,  8, -1, -1, -1, -1, -1, -1, -1}, // 79
#endif // MY_USING_NEW_EDGE_CASES
	{ 5, 10,  6,  4,  7,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 80
	{ 4,  3,  0,  4,  7,  3,  6,  5, 10, -1, -1, -1, -1, -1, -1, -1}, // 81
	{ 1,  9,  0,  5, 10,  6,  8,  4,  7, -1, -1, -1, -1, -1, -1, -1}, // 82
#ifdef MY_USING_NEW_EDGE_CASES
	{10,  6,  5,  4,  7,  3,  4,  3,  9,  9,  3,  1, -1, -1, -1, -1}, // (new) 83
#else // !MY_USING_NEW_EDGE_CASES
	{10,  6,  5,  1,  9,  7,  1,  7,  3,  7,  9,  4, -1, -1, -1, -1}, // 83
#endif // MY_USING_NEW_EDGE_CASES
	{ 6,  1,  2,  6,  5,  1,  4,  7,  8, -1, -1, -1, -1, -1, -1, -1}, // 84
	{ 1,  2,  5,  5,  2,  6,  3,  0,  4,  3,  4,  7, -1, -1, -1, -1}, // 85
#ifdef MY_USING_NEW_EDGE_CASES
	{ 8,  4,  7,  9,  0,  2,  9,  2,  5,  5,  2,  6, -1, -1, -1, -1}, // (new) 86
#else // !MY_USING_NEW_EDGE_CASES
	{ 8,  4,  7,  9,  0,  5,  0,  6,  5,  0,  2,  6, -1, -1, -1, -1}, // 86
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 4,  7,  3,  4,  3,  9,  3,  2,  9,  9,  2,  5,  5,  2,  6, -1}, // (new) 87
#else // !MY_USING_NEW_EDGE_CASES
	{ 7,  3,  9,  7,  9,  4,  3,  2,  9,  5,  9,  6,  2,  6,  9, -1}, // 87
#endif // MY_USING_NEW_EDGE_CASES
	{ 3, 11,  2,  7,  8,  4, 10,  6,  5, -1, -1, -1, -1, -1, -1, -1}, // 88
#ifdef MY_USING_NEW_EDGE_CASES
	{ 5, 10,  6, 11,  2,  0, 11,  0,  7,  7,  0,  4, -1, -1, -1, -1}, // (new) 89
#else // !MY_USING_NEW_EDGE_CASES
	{ 5, 10,  6,  4,  7,  2,  4,  2,  0,  2,  7, 11, -1, -1, -1, -1}, // 89
#endif // MY_USING_NEW_EDGE_CASES
	{ 0,  1,  9,  4,  7,  8,  2,  3, 11,  5, 10,  6, -1, -1, -1, -1}, // 90
	{ 9,  2,  1,  9, 11,  2,  9,  4, 11,  7, 11,  4,  5, 10,  6, -1}, // 91
#ifdef MY_USING_NEW_EDGE_CASES
	{ 8,  4,  7,  6,  5,  1,  6,  1, 11, 11,  1,  3, -1, -1, -1, -1}, // (new) 92
#else // !MY_USING_NEW_EDGE_CASES
	{ 8,  4,  7,  3, 11,  5,  3,  5,  1,  5, 11,  6, -1, -1, -1, -1}, // 92
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 6,  5,  1,  6,  1, 11,  1,  0, 11, 11,  0,  7,  7,  0,  4, -1}, // (new) 93
#else // !MY_USING_NEW_EDGE_CASES
	{ 5,  1, 11,  5, 11,  6,  1,  0, 11,  7, 11,  4,  0,  4, 11, -1}, // 93
#endif // MY_USING_NEW_EDGE_CASES
	{ 0,  5,  9,  0,  6,  5,  0,  3,  6, 11,  6,  3,  8,  4,  7, -1}, // 94
	{ 6,  5,  9,  6,  9, 11,  4,  7,  9,  7, 11,  9, -1, -1, -1, -1}, // 95
	{10,  4,  9,  6,  4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 96
	{ 4, 10,  6,  4,  9, 10,  0,  8,  3, -1, -1, -1, -1, -1, -1, -1}, // 97
#ifdef MY_USING_NEW_EDGE_CASES
	{ 1,  4,  0,  1, 10,  4, 10,  6,  4, -1, -1, -1, -1, -1, -1, -1}, // (new) 98
#else // !MY_USING_NEW_EDGE_CASES
	{10,  0,  1, 10,  6,  0,  6,  4,  0, -1, -1, -1, -1, -1, -1, -1}, // 98
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 8,  3,  1, 10,  6,  8,  8,  6,  4, 10,  8,  1, -1, -1, -1, -1}, // (new) 99
#else // !MY_USING_NEW_EDGE_CASES
	{ 8,  3,  1,  8,  1,  6,  8,  6,  4,  6,  1, 10, -1, -1, -1, -1}, // 99
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 9,  6,  4,  9,  1,  6,  1,  2,  6, -1, -1, -1, -1, -1, -1, -1}, // (new) 100
#else // !MY_USING_NEW_EDGE_CASES
	{ 1,  4,  9,  1,  2,  4,  2,  6,  4, -1, -1, -1, -1, -1, -1, -1}, // 100
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 3,  0,  8,  1,  2,  6,  1,  6,  9,  9,  6,  4, -1, -1, -1, -1}, // (new) 101
#else // !MY_USING_NEW_EDGE_CASES
	{ 3,  0,  8,  1,  2,  9,  2,  4,  9,  2,  6,  4, -1, -1, -1, -1}, // 101
#endif // MY_USING_NEW_EDGE_CASES
	{ 0,  2,  4,  4,  2,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 102
#ifdef MY_USING_NEW_EDGE_CASES
	{ 3,  2,  6,  3,  6,  8,  8,  6,  4, -1, -1, -1, -1, -1, -1, -1}, // (new) 103
#else // !MY_USING_NEW_EDGE_CASES
	{ 8,  3,  2,  8,  2,  4,  4,  2,  6, -1, -1, -1, -1, -1, -1, -1}, // 103
#endif // MY_USING_NEW_EDGE_CASES
	{10,  4,  9, 10,  6,  4, 11,  2,  3, -1, -1, -1, -1, -1, -1, -1}, // 104
	{ 0,  8,  2,  2,  8, 11,  4,  9, 10,  4, 10,  6, -1, -1, -1, -1}, // 105
#ifdef MY_USING_NEW_EDGE_CASES
	{ 3, 11,  2, 10,  6,  4, 10,  4,  1,  1,  4,  0, -1, -1, -1, -1}, // (new) 106
#else // !MY_USING_NEW_EDGE_CASES
	{ 3, 11,  2,  0,  1,  6,  0,  6,  4,  6,  1, 10, -1, -1, -1, -1}, // 106
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{10,  6,  4, 10,  4,  1,  4,  8,  1,  1,  8,  2,  2,  8, 11, -1}, // (new) 107
#else // !MY_USING_NEW_EDGE_CASES
	{ 6,  4,  1,  6,  1, 10,  4,  8,  1,  2,  1, 11,  8, 11,  1, -1}, // 107
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 9,  6,  4, 11,  6,  9,  9,  1,  3, 11,  9,  3, -1, -1, -1, -1}, // (new) 108
#else // !MY_USING_NEW_EDGE_CASES
	{ 9,  6,  4,  9,  3,  6,  9,  1,  3, 11,  6,  3, -1, -1, -1, -1}, // 108
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 0,  8, 11,  0, 11,  1, 11,  6,  1,  1,  6,  9,  9,  6,  4, -1}, // (new) 109
#else // !MY_USING_NEW_EDGE_CASES
	{ 8, 11,  1,  8,  1,  0, 11,  6,  1,  9,  1,  4,  6,  4,  1, -1}, // 109
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{11,  6,  4, 11,  4,  3,  3,  4,  0, -1, -1, -1, -1, -1, -1, -1}, // (new) 110
#else // !MY_USING_NEW_EDGE_CASES
	{ 3, 11,  6,  3,  6,  0,  0,  6,  4, -1, -1, -1, -1, -1, -1, -1}, // 110
#endif // MY_USING_NEW_EDGE_CASES
	{ 6,  4,  8, 11,  6,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 111
#ifdef MY_USING_NEW_EDGE_CASES
	{ 6,  9, 10,  6,  7,  9,  7,  8,  9, -1, -1, -1, -1, -1, -1, -1}, // (new) 112
#else // !MY_USING_NEW_EDGE_CASES
	{ 7, 10,  6,  7,  8, 10,  8,  9, 10, -1, -1, -1, -1, -1, -1, -1}, // 112
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 0,  7,  3,  6,  7,  0,  0,  9, 10,  6,  0, 10, -1, -1, -1, -1}, // (new) 113
#else // !MY_USING_NEW_EDGE_CASES
	{ 0,  7,  3,  0, 10,  7,  0,  9, 10,  6,  7, 10, -1, -1, -1, -1}, // 113
#endif // MY_USING_NEW_EDGE_CASES
	{10,  6,  7,  1, 10,  7,  1,  7,  8,  1,  8,  0, -1, -1, -1, -1}, // 114
#ifdef MY_USING_NEW_EDGE_CASES
	{ 6,  7,  3,  6,  3, 10, 10,  3,  1, -1, -1, -1, -1, -1, -1, -1}, // (new) 115
#else // !MY_USING_NEW_EDGE_CASES
	{10,  6,  7, 10,  7,  1,  1,  7,  3, -1, -1, -1, -1, -1, -1, -1}, // 115
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 1,  2,  6,  7,  8,  1,  1,  8,  9,  7,  1,  6, -1, -1, -1, -1}, // (new) 116
#else // !MY_USING_NEW_EDGE_CASES
	{ 1,  2,  6,  1,  6,  8,  1,  8,  9,  8,  6,  7, -1, -1, -1, -1}, // 116
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 1,  2,  6,  1,  6,  9,  6,  7,  9,  9,  7,  0,  0,  7,  3, -1}, // (new) 117
#else // !MY_USING_NEW_EDGE_CASES
	{ 2,  6,  9,  2,  9,  1,  6,  7,  9,  0,  9,  3,  7,  3,  9, -1}, // 117
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 8,  0,  2,  8,  2,  7,  7,  2,  6, -1, -1, -1, -1, -1, -1, -1}, // (new) 118
#else // !MY_USING_NEW_EDGE_CASES
	{ 7,  8,  0,  7,  0,  6,  6,  0,  2, -1, -1, -1, -1, -1, -1, -1}, // 118
#endif // MY_USING_NEW_EDGE_CASES
	{ 7,  3,  2,  6,  7,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 119
#ifdef MY_USING_NEW_EDGE_CASES
	{ 2,  3, 11,  7,  8,  9,  7,  9,  6,  6,  9, 10, -1, -1, -1, -1}, // (new) 120
#else // !MY_USING_NEW_EDGE_CASES
	{ 2,  3, 11, 10,  6,  8, 10,  8,  9,  8,  6,  7, -1, -1, -1, -1}, // 120
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{11,  2,  0, 11,  0,  7,  0,  9,  7,  7,  9,  6,  6,  9, 10, -1}, // (new) 121
#else // !MY_USING_NEW_EDGE_CASES
	{ 2,  0,  7,  2,  7, 11,  0,  9,  7,  6,  7, 10,  9, 10,  7, -1}, // 121
#endif // MY_USING_NEW_EDGE_CASES
	{ 1,  8,  0,  1,  7,  8,  1, 10,  7,  6,  7, 10,  2,  3, 11, -1}, // 122
	{11,  2,  1, 11,  1,  7, 10,  6,  1,  6,  7,  1, -1, -1, -1, -1}, // 123
#ifdef MY_USING_NEW_EDGE_CASES
	{ 7,  8,  9,  7,  9,  6,  9,  1,  6,  6,  1, 11, 11,  1,  3, -1}, // (new) 124
#else // !MY_USING_NEW_EDGE_CASES
	{ 8,  9,  6,  8,  6,  7,  9,  1,  6, 11,  6,  3,  1,  3,  6, -1}, // 124
#endif // MY_USING_NEW_EDGE_CASES
	{ 0,  9,  1, 11,  6,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 125
	{ 7,  8,  0,  7,  0,  6,  3, 11,  0, 11,  6,  0, -1, -1, -1, -1}, // 126
	{ 7, 11,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 127
	{ 7,  6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 128
	{ 3,  0,  8, 11,  7,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 129
	{ 0,  1,  9, 11,  7,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 130
	{ 8,  1,  9,  8,  3,  1, 11,  7,  6, -1, -1, -1, -1, -1, -1, -1}, // 131
	{10,  1,  2,  6, 11,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 132
	{ 1,  2, 10,  3,  0,  8,  6, 11,  7, -1, -1, -1, -1, -1, -1, -1}, // 133
	{ 2,  9,  0,  2, 10,  9,  6, 11,  7, -1, -1, -1, -1, -1, -1, -1}, // 134
#ifdef MY_USING_NEW_EDGE_CASES
	{ 6, 11,  7,  2, 10,  9,  2,  9,  3,  3,  9,  8, -1, -1, -1, -1}, // (new) 135
#else // !MY_USING_NEW_EDGE_CASES
	{ 6, 11,  7,  2, 10,  3, 10,  8,  3, 10,  9,  8, -1, -1, -1, -1}, // 135
#endif // MY_USING_NEW_EDGE_CASES
	{ 7,  2,  3,  6,  2,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 136
#ifdef MY_USING_NEW_EDGE_CASES
	{ 8,  2,  0,  8,  7,  2,  7,  6,  2, -1, -1, -1, -1, -1, -1, -1}, // (new) 137
#else // !MY_USING_NEW_EDGE_CASES
	{ 7,  0,  8,  7,  6,  0,  6,  2,  0, -1, -1, -1, -1, -1, -1, -1}, // 137
#endif // MY_USING_NEW_EDGE_CASES
	{ 2,  7,  6,  2,  3,  7,  0,  1,  9, -1, -1, -1, -1, -1, -1, -1}, // 138
#ifdef MY_USING_NEW_EDGE_CASES
	{ 1,  6,  2,  7,  6,  1,  1,  9,  8,  7,  1,  8, -1, -1, -1, -1}, // (new) 139
#else // !MY_USING_NEW_EDGE_CASES
	{ 1,  6,  2,  1,  8,  6,  1,  9,  8,  8,  7,  6, -1, -1, -1, -1}, // 139
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 6,  3,  7,  6, 10,  3, 10,  1,  3, -1, -1, -1, -1, -1, -1, -1}, // (new) 140
#else // !MY_USING_NEW_EDGE_CASES
	{10,  7,  6, 10,  1,  7,  1,  3,  7, -1, -1, -1, -1, -1, -1, -1}, // 140
#endif // MY_USING_NEW_EDGE_CASES
	{10,  7,  6,  1,  7, 10,  1,  8,  7,  1,  0,  8, -1, -1, -1, -1}, // 141
#ifdef MY_USING_NEW_EDGE_CASES
	{ 0,  3,  7,  6, 10,  0,  0, 10,  9,  6,  0,  7, -1, -1, -1, -1}, // (new) 142
#else // !MY_USING_NEW_EDGE_CASES
	{ 0,  3,  7,  0,  7, 10,  0, 10,  9,  6, 10,  7, -1, -1, -1, -1}, // 142
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 6, 10,  9,  6,  9,  7,  7,  9,  8, -1, -1, -1, -1, -1, -1, -1}, // (new) 143
#else // !MY_USING_NEW_EDGE_CASES
	{ 7,  6, 10,  7, 10,  8,  8, 10,  9, -1, -1, -1, -1, -1, -1, -1}, // 143
#endif // MY_USING_NEW_EDGE_CASES
	{ 6,  8,  4, 11,  8,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 144
#ifdef MY_USING_NEW_EDGE_CASES
	{11,  4,  6, 11,  3,  4,  3,  0,  4, -1, -1, -1, -1, -1, -1, -1}, // (new) 145
#else // !MY_USING_NEW_EDGE_CASES
	{ 3,  6, 11,  3,  0,  6,  0,  4,  6, -1, -1, -1, -1, -1, -1, -1}, // 145
#endif // MY_USING_NEW_EDGE_CASES
	{ 8,  6, 11,  8,  4,  6,  9,  0,  1, -1, -1, -1, -1, -1, -1, -1}, // 146
#ifdef MY_USING_NEW_EDGE_CASES
	{ 9,  4,  6, 11,  3,  9,  9,  3,  1, 11,  9,  6, -1, -1, -1, -1}, // (new) 147
#else // !MY_USING_NEW_EDGE_CASES
	{ 9,  4,  6,  9,  6,  3,  9,  3,  1, 11,  3,  6, -1, -1, -1, -1}, // 147
#endif // MY_USING_NEW_EDGE_CASES
	{ 6,  8,  4,  6, 11,  8,  2, 10,  1, -1, -1, -1, -1, -1, -1, -1}, // 148
#ifdef MY_USING_NEW_EDGE_CASES
	{ 1,  2, 10,  3,  0,  4,  3,  4, 11, 11,  4,  6, -1, -1, -1, -1}, // (new) 149
#else // !MY_USING_NEW_EDGE_CASES
	{ 1,  2, 10,  3,  0, 11,  0,  6, 11,  0,  4,  6, -1, -1, -1, -1}, // 149
#endif // MY_USING_NEW_EDGE_CASES
	{ 4, 11,  8,  4,  6, 11,  0,  2,  9,  2, 10,  9, -1, -1, -1, -1}, // 150
#ifdef MY_USING_NEW_EDGE_CASES
	{ 2, 10,  9,  2,  9,  3,  9,  4,  3,  3,  4, 11, 11,  4,  6, -1}, // (new) 151
#else // !MY_USING_NEW_EDGE_CASES
	{10,  9,  3, 10,  3,  2,  9,  4,  3, 11,  3,  6,  4,  6,  3, -1}, // 151
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 3,  6,  2,  3,  8,  6,  8,  4,  6, -1, -1, -1, -1, -1, -1, -1}, // (new) 152
#else // !MY_USING_NEW_EDGE_CASES
	{ 8,  2,  3,  8,  4,  2,  4,  6,  2, -1, -1, -1, -1, -1, -1, -1}, // 152
#endif // MY_USING_NEW_EDGE_CASES
	{ 0,  4,  2,  4,  6,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 153
#ifdef MY_USING_NEW_EDGE_CASES
	{ 1,  9,  0,  8,  4,  6,  8,  6,  3,  3,  6,  2, -1, -1, -1, -1}, // (new) 154
#else // !MY_USING_NEW_EDGE_CASES
	{ 1,  9,  0,  2,  3,  4,  2,  4,  6,  4,  3,  8, -1, -1, -1, -1}, // 154
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 9,  4,  6,  9,  6,  1,  1,  6,  2, -1, -1, -1, -1, -1, -1, -1}, // (new) 155
#else // !MY_USING_NEW_EDGE_CASES
	{ 1,  9,  4,  1,  4,  2,  2,  4,  6, -1, -1, -1, -1, -1, -1, -1}, // 155
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 8,  1,  3, 10,  1,  8,  8,  4,  6, 10,  8,  6, -1, -1, -1, -1}, // (new) 156
#else // !MY_USING_NEW_EDGE_CASES
	{ 8,  1,  3,  8,  6,  1,  8,  4,  6,  6, 10,  1, -1, -1, -1, -1}, // 156
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 1,  0,  4,  1,  4, 10, 10,  4,  6, -1, -1, -1, -1, -1, -1, -1}, // (new) 157
#else // !MY_USING_NEW_EDGE_CASES
	{10,  1,  0, 10,  0,  6,  6,  0,  4, -1, -1, -1, -1, -1, -1, -1}, // 157
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 8,  4,  6,  8,  6,  3,  6, 10,  3,  3, 10,  0,  0, 10,  9, -1}, // (new) 158
#else // !MY_USING_NEW_EDGE_CASES
	{ 4,  6,  3,  4,  3,  8,  6, 10,  3,  0,  3,  9, 10,  9,  3, -1}, // 158
#endif // MY_USING_NEW_EDGE_CASES
	{10,  9,  4,  6, 10,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 159
	{ 4,  9,  5,  7,  6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 160
	{ 0,  8,  3,  4,  9,  5, 11,  7,  6, -1, -1, -1, -1, -1, -1, -1}, // 161
	{ 5,  0,  1,  5,  4,  0,  7,  6, 11, -1, -1, -1, -1, -1, -1, -1}, // 162
#ifdef MY_USING_NEW_EDGE_CASES
	{11,  7,  6,  8,  3,  1,  8,  1,  4,  4,  1,  5, -1, -1, -1, -1}, // (new) 163
#else // !MY_USING_NEW_EDGE_CASES
	{11,  7,  6,  8,  3,  4,  3,  5,  4,  3,  1,  5, -1, -1, -1, -1}, // 163
#endif // MY_USING_NEW_EDGE_CASES
	{ 9,  5,  4, 10,  1,  2,  7,  6, 11, -1, -1, -1, -1, -1, -1, -1}, // 164
	{ 6, 11,  7,  1,  2, 10,  0,  8,  3,  4,  9,  5, -1, -1, -1, -1}, // 165
#ifdef MY_USING_NEW_EDGE_CASES
	{ 7,  6, 11,  5,  4,  0,  5,  0, 10, 10,  0,  2, -1, -1, -1, -1}, // (new) 166
#else // !MY_USING_NEW_EDGE_CASES
	{ 7,  6, 11,  5,  4, 10,  4,  2, 10,  4,  0,  2, -1, -1, -1, -1}, // 166
#endif // MY_USING_NEW_EDGE_CASES
	{ 3,  4,  8,  3,  5,  4,  3,  2,  5, 10,  5,  2, 11,  7,  6, -1}, // 167
	{ 7,  2,  3,  7,  6,  2,  5,  4,  9, -1, -1, -1, -1, -1, -1, -1}, // 168
#ifdef MY_USING_NEW_EDGE_CASES
	{ 9,  5,  4,  7,  6,  2,  7,  2,  8,  8,  2,  0, -1, -1, -1, -1}, // (new) 169
#else // !MY_USING_NEW_EDGE_CASES
	{ 9,  5,  4,  0,  8,  6,  0,  6,  2,  6,  8,  7, -1, -1, -1, -1}, // 169
#endif // MY_USING_NEW_EDGE_CASES
	{ 3,  6,  2,  3,  7,  6,  1,  5,  0,  5,  4,  0, -1, -1, -1, -1}, // 170
#ifdef MY_USING_NEW_EDGE_CASES
	{ 6,  2,  7,  7,  2,  8,  2,  1,  8,  8,  1,  4,  4,  1,  5, -1}, // (new) 171
#else // !MY_USING_NEW_EDGE_CASES
	{ 6,  2,  8,  6,  8,  7,  2,  1,  8,  4,  8,  5,  1,  5,  8, -1}, // 171
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 9,  5,  4, 10,  1,  3, 10,  3,  6,  6,  3,  7, -1, -1, -1, -1}, // (new) 172
#else // !MY_USING_NEW_EDGE_CASES
	{ 9,  5,  4, 10,  1,  6,  1,  7,  6,  1,  3,  7, -1, -1, -1, -1}, // 172
#endif // MY_USING_NEW_EDGE_CASES
	{ 1,  6, 10,  1,  7,  6,  1,  0,  7,  8,  7,  0,  9,  5,  4, -1}, // 173
#ifdef MY_USING_NEW_EDGE_CASES
	{ 4,  0,  5,  5,  0, 10,  0,  3, 10, 10,  3,  6,  6,  3,  7, -1}, // (new) 174
#else // !MY_USING_NEW_EDGE_CASES
	{ 4,  0, 10,  4, 10,  5,  0,  3, 10,  6, 10,  7,  3,  7, 10, -1}, // 174
#endif // MY_USING_NEW_EDGE_CASES
	{ 7,  6, 10,  7, 10,  8,  5,  4, 10,  4,  8, 10, -1, -1, -1, -1}, // 175
#ifdef MY_USING_NEW_EDGE_CASES
	{ 5,  8,  9,  5,  6,  8,  6, 11,  8, -1, -1, -1, -1, -1, -1, -1}, // (new) 176
#else // !MY_USING_NEW_EDGE_CASES
	{ 6,  9,  5,  6, 11,  9, 11,  8,  9, -1, -1, -1, -1, -1, -1, -1}, // 176
#endif // MY_USING_NEW_EDGE_CASES
	{ 3,  6, 11,  0,  6,  3,  0,  5,  6,  0,  9,  5, -1, -1, -1, -1}, // 177
#ifdef MY_USING_NEW_EDGE_CASES
	{ 0, 11,  8,  6, 11,  0,  0,  1,  5,  6,  0,  5, -1, -1, -1, -1}, // (new) 178
#else // !MY_USING_NEW_EDGE_CASES
	{ 0, 11,  8,  0,  5, 11,  0,  1,  5,  5,  6, 11, -1, -1, -1, -1}, // 178
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{11,  3,  1, 11,  1,  6,  6,  1,  5, -1, -1, -1, -1, -1, -1, -1}, // (new) 179
#else // !MY_USING_NEW_EDGE_CASES
	{ 6, 11,  3,  6,  3,  5,  5,  3,  1, -1, -1, -1, -1, -1, -1, -1}, // 179
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 1,  2, 10,  6, 11,  8,  6,  8,  5,  5,  8,  9, -1, -1, -1, -1}, // (new) 180
#else // !MY_USING_NEW_EDGE_CASES
	{ 1,  2, 10,  9,  5, 11,  9, 11,  8, 11,  5,  6, -1, -1, -1, -1}, // 180
#endif // MY_USING_NEW_EDGE_CASES
	{ 0, 11,  3,  0,  6, 11,  0,  9,  6,  5,  6,  9,  1,  2, 10, -1}, // 181
#ifdef MY_USING_NEW_EDGE_CASES
	{ 6, 11,  8,  6,  8,  5,  8,  0,  5,  5,  0, 10, 10,  0,  2, -1}, // (new) 182
#else // !MY_USING_NEW_EDGE_CASES
	{11,  8,  5, 11,  5,  6,  8,  0,  5, 10,  5,  2,  0,  2,  5, -1}, // 182
#endif // MY_USING_NEW_EDGE_CASES
	{ 6, 11,  3,  6,  3,  5,  2, 10,  3, 10,  5,  3, -1, -1, -1, -1}, // 183
#ifdef MY_USING_NEW_EDGE_CASES
	{ 5,  8,  9,  3,  8,  5,  5,  6,  2,  3,  5,  2, -1, -1, -1, -1}, // (new) 184
#else // !MY_USING_NEW_EDGE_CASES
	{ 5,  8,  9,  5,  2,  8,  5,  6,  2,  3,  8,  2, -1, -1, -1, -1}, // 184
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 5,  6,  2,  5,  2,  9,  9,  2,  0, -1, -1, -1, -1, -1, -1, -1}, // (new) 185
#else // !MY_USING_NEW_EDGE_CASES
	{ 9,  5,  6,  9,  6,  0,  0,  6,  2, -1, -1, -1, -1, -1, -1, -1}, // 185
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 0,  1,  5,  0,  5,  8,  5,  6,  8,  8,  6,  3,  3,  6,  2, -1}, // (new) 186
#else // !MY_USING_NEW_EDGE_CASES
	{ 1,  5,  8,  1,  8,  0,  5,  6,  8,  3,  8,  2,  6,  2,  8, -1}, // 186
#endif // MY_USING_NEW_EDGE_CASES
	{ 1,  5,  6,  2,  1,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 187
#ifdef MY_USING_NEW_EDGE_CASES
	{ 1,  3, 10, 10,  3,  6,  3,  8,  6,  6,  8,  5,  5,  8,  9, -1}, // (new) 188
#else // !MY_USING_NEW_EDGE_CASES
	{ 1,  3,  6,  1,  6, 10,  3,  8,  6,  5,  6,  9,  8,  9,  6, -1}, // 188
#endif // MY_USING_NEW_EDGE_CASES
	{10,  1,  0, 10,  0,  6,  9,  5,  0,  5,  6,  0, -1, -1, -1, -1}, // 189
	{ 0,  3,  8,  5,  6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 190
	{10,  5,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 191
	{11,  5, 10,  7,  5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 192
	{11,  5, 10, 11,  7,  5,  8,  3,  0, -1, -1, -1, -1, -1, -1, -1}, // 193
	{ 5, 11,  7,  5, 10, 11,  1,  9,  0, -1, -1, -1, -1, -1, -1, -1}, // 194
	{10,  7,  5, 10, 11,  7,  9,  8,  1,  8,  3,  1, -1, -1, -1, -1}, // 195
#ifdef MY_USING_NEW_EDGE_CASES
	{ 2,  5,  1,  2, 11,  5, 11,  7,  5, -1, -1, -1, -1, -1, -1, -1}, // (new) 196
#else // !MY_USING_NEW_EDGE_CASES
	{11,  1,  2, 11,  7,  1,  7,  5,  1, -1, -1, -1, -1, -1, -1, -1}, // 196
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 0,  8,  3, 11,  7,  5, 11,  5,  2,  2,  5,  1, -1, -1, -1, -1}, // (new) 197
#else // !MY_USING_NEW_EDGE_CASES
	{ 0,  8,  3,  1,  2,  7,  1,  7,  5,  7,  2, 11, -1, -1, -1, -1}, // 197
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 9,  7,  5, 11,  7,  9,  9,  0,  2, 11,  9,  2, -1, -1, -1, -1}, // (new) 198
#else // !MY_USING_NEW_EDGE_CASES
	{ 9,  7,  5,  9,  2,  7,  9,  0,  2,  2, 11,  7, -1, -1, -1, -1}, // 198
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{11,  7,  5, 11,  5,  2,  5,  9,  2,  2,  9,  3,  3,  9,  8, -1}, // (new) 199
#else // !MY_USING_NEW_EDGE_CASES
	{ 7,  5,  2,  7,  2, 11,  5,  9,  2,  3,  2,  8,  9,  8,  2, -1}, // 199
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{10,  7,  5, 10,  2,  7,  2,  3,  7, -1, -1, -1, -1, -1, -1, -1}, // (new) 200
#else // !MY_USING_NEW_EDGE_CASES
	{ 2,  5, 10,  2,  3,  5,  3,  7,  5, -1, -1, -1, -1, -1, -1, -1}, // 200
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 8,  2,  0, 10,  2,  8,  8,  7,  5, 10,  8,  5, -1, -1, -1, -1}, // (new) 201
#else // !MY_USING_NEW_EDGE_CASES
	{ 8,  2,  0,  8,  5,  2,  8,  7,  5, 10,  2,  5, -1, -1, -1, -1}, // 201
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 9,  0,  1,  2,  3,  7,  2,  7, 10, 10,  7,  5, -1, -1, -1, -1}, // (new) 202
#else // !MY_USING_NEW_EDGE_CASES
	{ 9,  0,  1,  5, 10,  3,  5,  3,  7,  3, 10,  2, -1, -1, -1, -1}, // 202
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 1,  9,  8,  1,  8,  2,  8,  7,  2,  2,  7, 10, 10,  7,  5, -1}, // (new) 203
#else // !MY_USING_NEW_EDGE_CASES
	{ 9,  8,  2,  9,  2,  1,  8,  7,  2, 10,  2,  5,  7,  5,  2, -1}, // 203
#endif // MY_USING_NEW_EDGE_CASES
	{ 1,  3,  5,  3,  7,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 204
#ifdef MY_USING_NEW_EDGE_CASES
	{ 8,  7,  5,  8,  5,  0,  0,  5,  1, -1, -1, -1, -1, -1, -1, -1}, // (new) 205
#else // !MY_USING_NEW_EDGE_CASES
	{ 0,  8,  7,  0,  7,  1,  1,  7,  5, -1, -1, -1, -1, -1, -1, -1}, // 205
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 0,  3,  7,  0,  7,  9,  9,  7,  5, -1, -1, -1, -1, -1, -1, -1}, // (new) 206
#else // !MY_USING_NEW_EDGE_CASES
	{ 9,  0,  3,  9,  3,  5,  5,  3,  7, -1, -1, -1, -1, -1, -1, -1}, // 206
#endif // MY_USING_NEW_EDGE_CASES
	{ 9,  8,  7,  5,  9,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 207
#ifdef MY_USING_NEW_EDGE_CASES
	{ 4, 11,  8,  4,  5, 11,  5, 10, 11, -1, -1, -1, -1, -1, -1, -1}, // (new) 208
#else // !MY_USING_NEW_EDGE_CASES
	{ 5,  8,  4,  5, 10,  8, 10, 11,  8, -1, -1, -1, -1, -1, -1, -1}, // 208
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 5,  0,  4,  3,  0,  5,  5, 10, 11,  3,  5, 11, -1, -1, -1, -1}, // (new) 209
#else // !MY_USING_NEW_EDGE_CASES
	{ 5,  0,  4,  5, 11,  0,  5, 10, 11, 11,  3,  0, -1, -1, -1, -1}, // 209
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 0,  1,  9,  5, 10, 11,  5, 11,  4,  4, 11,  8, -1, -1, -1, -1}, // (new) 210
#else // !MY_USING_NEW_EDGE_CASES
	{ 0,  1,  9,  8,  4, 10,  8, 10, 11, 10,  4,  5, -1, -1, -1, -1}, // 210
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 5, 10, 11,  5, 11,  4, 11,  3,  4,  4,  3,  9,  9,  3,  1, -1}, // (new) 211
#else // !MY_USING_NEW_EDGE_CASES
	{10, 11,  4, 10,  4,  5, 11,  3,  4,  9,  4,  1,  3,  1,  4, -1}, // 211
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 2,  5,  1,  4,  5,  2,  2, 11,  8,  4,  2,  8, -1, -1, -1, -1}, // (new) 212
#else // !MY_USING_NEW_EDGE_CASES
	{ 2,  5,  1,  2,  8,  5,  2, 11,  8,  4,  5,  8, -1, -1, -1, -1}, // 212
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 3,  0,  4,  3,  4, 11,  4,  5, 11, 11,  5,  2,  2,  5,  1, -1}, // (new) 213
#else // !MY_USING_NEW_EDGE_CASES
	{ 0,  4, 11,  0, 11,  3,  4,  5, 11,  2, 11,  1,  5,  1, 11, -1}, // 213
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 9,  0,  2,  9,  2,  5,  2, 11,  5,  5, 11,  4,  4, 11,  8, -1}, // (new) 214
#else // !MY_USING_NEW_EDGE_CASES
	{ 0,  2,  5,  0,  5,  9,  2, 11,  5,  4,  5,  8, 11,  8,  5, -1}, // 214
#endif // MY_USING_NEW_EDGE_CASES
	{ 9,  4,  5,  2, 11,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 215
	{ 2,  5, 10,  3,  5,  2,  3,  4,  5,  3,  8,  4, -1, -1, -1, -1}, // 216
#ifdef MY_USING_NEW_EDGE_CASES
	{10,  2,  0, 10,  0,  5,  5,  0,  4, -1, -1, -1, -1, -1, -1, -1}, // (new) 217
#else // !MY_USING_NEW_EDGE_CASES
	{ 5, 10,  2,  5,  2,  4,  4,  2,  0, -1, -1, -1, -1, -1, -1, -1}, // 217
#endif // MY_USING_NEW_EDGE_CASES
	{ 3, 10,  2,  3,  5, 10,  3,  8,  5,  4,  5,  8,  0,  1,  9, -1}, // 218
	{ 5, 10,  2,  5,  2,  4,  1,  9,  2,  9,  4,  2, -1, -1, -1, -1}, // 219
#ifdef MY_USING_NEW_EDGE_CASES
	{ 4,  5,  1,  4,  1,  8,  8,  1,  3, -1, -1, -1, -1, -1, -1, -1}, // (new) 220
#else // !MY_USING_NEW_EDGE_CASES
	{ 8,  4,  5,  8,  5,  3,  3,  5,  1, -1, -1, -1, -1, -1, -1, -1}, // 220
#endif // MY_USING_NEW_EDGE_CASES
	{ 0,  4,  5,  1,  0,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 221
	{ 8,  4,  5,  8,  5,  3,  9,  0,  5,  0,  3,  5, -1, -1, -1, -1}, // 222
	{ 9,  4,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 223
#ifdef MY_USING_NEW_EDGE_CASES
	{ 7, 10, 11,  7,  4, 10,  4,  9, 10, -1, -1, -1, -1, -1, -1, -1}, // (new) 224
#else // !MY_USING_NEW_EDGE_CASES
	{ 4, 11,  7,  4,  9, 11,  9, 10, 11, -1, -1, -1, -1, -1, -1, -1}, // 224
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 0,  8,  3,  4,  9, 10,  4, 10,  7,  7, 10, 11, -1, -1, -1, -1}, // (new) 225
#else // !MY_USING_NEW_EDGE_CASES
	{ 0,  8,  3,  4,  9,  7,  9, 11,  7,  9, 10, 11, -1, -1, -1, -1}, // 225
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 1, 10, 11,  1, 11,  7,  1,  7,  4,  1,  4,  0, -1, -1, -1, -1}, // (new) 226
#else // !MY_USING_NEW_EDGE_CASES
	{ 1, 10, 11,  1, 11,  4,  1,  4,  0,  7,  4, 11, -1, -1, -1, -1}, // 226
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 8,  3,  1,  8,  1,  4,  1, 10,  4,  4, 10,  7,  7, 10, 11, -1}, // (new) 227
#else // !MY_USING_NEW_EDGE_CASES
	{ 3,  1,  4,  3,  4,  8,  1, 10,  4,  7,  4, 11, 10, 11,  4, -1}, // 227
#endif // MY_USING_NEW_EDGE_CASES
	{ 4, 11,  7,  9, 11,  4,  9,  2, 11,  9,  1,  2, -1, -1, -1, -1}, // 228
	{ 9,  7,  4,  9, 11,  7,  9,  1, 11,  2, 11,  1,  0,  8,  3, -1}, // 229
#ifdef MY_USING_NEW_EDGE_CASES
	{ 7,  4,  0,  7,  0, 11, 11,  0,  2, -1, -1, -1, -1, -1, -1, -1}, // (new) 230
#else // !MY_USING_NEW_EDGE_CASES
	{11,  7,  4, 11,  4,  2,  2,  4,  0, -1, -1, -1, -1, -1, -1, -1}, // 230
#endif // MY_USING_NEW_EDGE_CASES
	{11,  7,  4, 11,  4,  2,  8,  3,  4,  3,  2,  4, -1, -1, -1, -1}, // 231
#ifdef MY_USING_NEW_EDGE_CASES
	{ 2,  9, 10,  2,  7,  4,  2,  3,  7,  4,  9,  2, -1, -1, -1, -1}, // (new) 232
#else // !MY_USING_NEW_EDGE_CASES
	{ 2,  9, 10,  2,  7,  9,  2,  3,  7,  7,  4,  9, -1, -1, -1, -1}, // 232
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 4,  9, 10,  4, 10,  7, 10,  2,  7,  7,  2,  8,  8,  2,  0, -1}, // (new) 233
#else // !MY_USING_NEW_EDGE_CASES
	{ 9, 10,  7,  9,  7,  4, 10,  2,  7,  8,  7,  0,  2,  0,  7, -1}, // 233
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 2,  3,  7,  2,  7, 10,  7,  4, 10, 10,  4,  1,  1,  4,  0, -1}, // (new) 234
#else // !MY_USING_NEW_EDGE_CASES
	{ 3,  7, 10,  3, 10,  2,  7,  4, 10,  1, 10,  0,  4,  0, 10, -1}, // 234
#endif // MY_USING_NEW_EDGE_CASES
	{ 1, 10,  2,  8,  7,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 235
#ifdef MY_USING_NEW_EDGE_CASES
	{ 9,  1,  3,  9,  3,  4,  4,  3,  7, -1, -1, -1, -1, -1, -1, -1}, // (new) 236
#else // !MY_USING_NEW_EDGE_CASES
	{ 4,  9,  1,  4,  1,  7,  7,  1,  3, -1, -1, -1, -1, -1, -1, -1}, // 236
#endif // MY_USING_NEW_EDGE_CASES
	{ 4,  9,  1,  4,  1,  7,  0,  8,  1,  8,  7,  1, -1, -1, -1, -1}, // 237
	{ 4,  0,  3,  7,  4,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 238
	{ 4,  8,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 239
	{ 9, 10,  8, 10, 11,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 240 
#ifdef MY_USING_NEW_EDGE_CASES
	{ 0,  9, 10,  0, 10,  3,  3, 10, 11, -1, -1, -1, -1, -1, -1, -1}, // (new) 241
#else // !MY_USING_NEW_EDGE_CASES
	{ 3,  0,  9,  3,  9, 11, 11,  9, 10, -1, -1, -1, -1, -1, -1, -1}, // 241
#endif // MY_USING_NEW_EDGE_CASES
#ifdef MY_USING_NEW_EDGE_CASES
	{ 1, 10, 11,  1, 11,  0,  0, 11,  8, -1, -1, -1, -1, -1, -1, -1}, // (new) 242
#else // !MY_USING_NEW_EDGE_CASES
	{ 0,  1, 10,  0, 10,  8,  8, 10, 11, -1, -1, -1, -1, -1, -1, -1}, // 242
#endif // MY_USING_NEW_EDGE_CASES
	{ 3,  1, 10, 11,  3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 243
#ifdef MY_USING_NEW_EDGE_CASES
	{ 2, 11,  8,  2,  8,  1,  1,  8,  9, -1, -1, -1, -1, -1, -1, -1}, // (new) 244
#else // !MY_USING_NEW_EDGE_CASES
	{ 1,  2, 11,  1, 11,  9,  9, 11,  8, -1, -1, -1, -1, -1, -1, -1}, // 244
#endif // MY_USING_NEW_EDGE_CASES
	{ 3,  0,  9,  3,  9, 11,  1,  2,  9,  2, 11,  9, -1, -1, -1, -1}, // 245
	{ 0,  2, 11,  8,  0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 246
	{ 3,  2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 247
#ifdef MY_USING_NEW_EDGE_CASES
	{ 3,  8,  9,  3,  9,  2,  2,  9, 10, -1, -1, -1, -1, -1, -1, -1}, // (new) 248
#else // !MY_USING_NEW_EDGE_CASES
	{ 2,  3,  8,  2,  8, 10, 10,  8,  9, -1, -1, -1, -1, -1, -1, -1}, // 248
#endif // MY_USING_NEW_EDGE_CASES
	{ 9, 10,  2,  0,  9,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 249
	{ 2,  3,  8,  2,  8, 10,  0,  1,  8,  1, 10,  8, -1, -1, -1, -1}, // 250
	{ 1, 10,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 251
	{ 1,  3,  8,  9,  1,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 252
	{ 0,  9,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 253
	{ 0,  3,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 254
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1} // 255
};

#ifndef NDEBUG
// The ambiguous Marching Cubes cases were identified based on Lewiner et al. 
// (http://jgt.akpeters.com/papers/LewinerEtAl03/index.html) extended Marching 
// Cubes table. For each non-ambiguous case (zero valued table entry), the 
// case classification as proposed by Lewiner is displayed in the comments.
const int CCombinedTransforms::m_isAmbiguousCubeCaseArray[256] = {
	0, // 0, case 0 
	0, // 1, case 1
	0, // 2, case 1
	0, // 3, case 2 
	0, // 4, case 1
	1, // 5
	0, // 6, case 2 
	0, // 7, case 5
	0, // 8, case 1
	0, // 9, case 2 
	1, // 10
	0, // 11, case 5
	0, // 12, case 2 
	0, // 13, case 5
	0, // 14, case 5
	0, // 15, case 8
	0, // 16, case 1
	0, // 17, case 2 
	1, // 18
	0, // 19, case 5
	1, // 20
	1, // 21
	1, // 22
	0, // 23, case 11
	1, // 24
	0, // 25, case 5
	1, // 26
	0, // 27, case 9
	1, // 28
	0, // 29, case 11
	1, // 30
	0, // 31, case 5
	0, // 32, case 1
	1, // 33
	0, // 34, case 2 
	0, // 35, case 5
	1, // 36
	1, // 37
	0, // 38, case 5
	0, // 39, case 9
	1, // 40
	1, // 41
	1, // 42
	0, // 43, case 11
	1, // 44
	1, // 45
	0, // 46, case 11
	0, // 47, case 5
	0, // 48, case 2 
	0, // 49, case 5
	0, // 50, case 5
	0, // 51, case 8
	1, // 52
	1, // 53
	0, // 54, case 11
	0, // 55, case 5
	1, // 56
	0, // 57, case 11
	1, // 58
	0, // 59, case 5
	1, // 60
	1, // 61
	1, // 62
	0, // 63, case 2 
	0, // 64, case 1
	1, // 65
	1, // 66
	1, // 67
	0, // 68, case 2 
	1, // 69
	0, // 70, case 5
	0, // 71, case 11
	1, // 72
	1, // 73
	1, // 74
	1, // 75
	0, // 76, case 5
	0, // 77, case 11
	0, // 78, case 9
	0, // 79, case 5
	1, // 80
	1, // 81
	1, // 82
	1, // 83
	1, // 84
	1, // 85
	1, // 86
	1, // 87
	1, // 88
	1, // 89
	1, // 90
	1, // 91
	1, // 92
	1, // 93
	1, // 94
	1, // 95
	0, // 96, case 2 
	1, // 97
	0, // 98, case 5
	0, // 99, case 11
	0, // 100, case 5
	1, // 101
	0, // 102, case 8
	0, // 103, case 5
	1, // 104
	1, // 105
	1, // 106
	1, // 107
	0, // 108, case 11
	1, // 109
	0, // 110, case 5
	0, // 111, case 2 
	0, // 112, case 5
	0, // 113, case 11
	0, // 114, case 9
	0, // 115, case 5
	0, // 116, case 11
	1, // 117
	0, // 118, case 5
	0, // 119, case 2 
	1, // 120
	1, // 121
	1, // 122
	1, // 123
	1, // 124
	1, // 125
	1, // 126
	0, // 127, case 1
	0, // 128, case 1
	1, // 129
	1, // 130
	1, // 131
	1, // 132
	1, // 133
	1, // 134
	1, // 135
	0, // 136, case 2 
	0, // 137, case 5
	1, // 138
	0, // 139, case 11
	0, // 140, case 5
	0, // 141, case 9
	0, // 142, case 11
	0, // 143, case 5
	0, // 144, case 2 
	0, // 145, case 5
	1, // 146
	0, // 147, case 11
	1, // 148
	1, // 149
	1, // 150
	1, // 151
	0, // 152, case 5
	0, // 153, case 8
	1, // 154
	0, // 155, case 5
	0, // 156, case 11
	0, // 157, case 5
	1, // 158
	0, // 159, case 2 
	1, // 160
	1, // 161
	1, // 162
	1, // 163
	1, // 164
	1, // 165
	1, // 166
	1, // 167
	1, // 168
	1, // 169
	1, // 170
	1, // 171
	1, // 172
	1, // 173
	1, // 174
	1, // 175
	0, // 176, case 5
	0, // 177, case 9
	0, // 178, case 11
	0, // 179, case 5
	1, // 180
	1, // 181
	1, // 182
	1, // 183
	1, // 184
	0, // 185, case 5
	1, // 186
	0, // 187, case 2 
	1, // 188
	1, // 189
	1, // 190
	0, // 191, case 1
	0, // 192, case 2 
	1, // 193
	1, // 194
	1, // 195
	0, // 196, case 5
	1, // 197
	0, // 198, case 11
	1, // 199
	0, // 200, case 5
	0, // 201, case 11
	1, // 202
	1, // 203
	0, // 204, case 8
	0, // 205, case 5
	0, // 206, case 5
	0, // 207, case 2 
	0, // 208, case 5
	0, // 209, case 11
	1, // 210
	1, // 211
	0, // 212, case 11
	1, // 213
	1, // 214
	1, // 215
	0, // 216, case 9
	0, // 217, case 5
	1, // 218
	1, // 219
	0, // 220, case 5
	0, // 221, case 2 
	1, // 222
	0, // 223, case 1
	0, // 224, case 5
	1, // 225
	0, // 226, case 11
	1, // 227
	0, // 228, case 9
	1, // 229
	0, // 230, case 5
	1, // 231
	0, // 232, case 11
	1, // 233
	1, // 234
	1, // 235
	0, // 236, case 5
	1, // 237
	0, // 238, case 2 
	0, // 239, case 1
	0, // 240, case 8
	0, // 241, case 5
	0, // 242, case 5
	0, // 243, case 2 
	0, // 244, case 5
	1, // 245
	0, // 246, case 2 
	0, // 247, case 1
	0, // 248, case 5
	0, // 249, case 2 
	1, // 250
	0, // 251, case 1
	0, // 252, case 2 
	0, // 253, case 1
	0, // 254, case 1
	0, // 255, case 0
};
#endif // #ifndef NDEBUG

/**
*/
CCombinedTransforms::CCombinedTransforms(void)
{
	// Move to a new 'Create' function?
	SetupVertexPool();
}

/**
*/
CCombinedTransforms::~CCombinedTransforms(void)
{
	// I must be careful with the destruction of this object. It is possible 
	// that the object is sharing the triangle mesh with other objects, and thus 
	// I don't have to clear it.
	//DeleteMesh();
}

/**
*/
bool CCombinedTransforms::GenerateMesh(float isoLevel)
{
	int cellType, 
		edgeType,
		effectiveWidth,
		effectiveHeight,
		effectiveDepth,
		x, 
		y, 
		z,
		i;
	float valueCache[4],
		xCoord,
		yCoord,
		zCoord;

	if (!m_image.get())
	{ 
		MarkError();

		return false;
	}

#ifndef NDEBUG
	m_ambiguousCubeCaseCount = 0;
#endif // #ifndef NDEBUG

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
		{
			MarkError();

			return false;
		}

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

				edgeType = m_cubeCaseToEdgeBitFieldArray[cellType];

				if (edgeType != 0) 
				{
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

#ifndef NDEBUG
					m_ambiguousCubeCaseCount += ((m_isAmbiguousCubeCaseArray[cellType]) ? 1 : 0);
#endif // #ifndef NDEBUG

					for (i=0; m_triangleTable[cellType][i]!=-1; i++)
						m_triangleMesh->m_elementArray.push_back(GetEdgeId(x, y, z, m_triangleTable[cellType][i]));
				}
			}
		}
	}

	RenameElements();
	
	if (!MeshChoose())
	{
		MarkError();

		return false;
	}

	RemoveUnreferencedVertices();

#ifndef NDEBUG
	cout << "(number of ambiguous cells founded: " << m_ambiguousCubeCaseCount << ")" << endl;
#endif // #ifndef NDEBUG

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
bool CCombinedTransforms::GenerateMeshFromSpanSpace(float isoLevel)
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
		j,
		k;
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

				edgeType = m_cubeCaseToEdgeBitFieldArray[cellType];

				if (edgeType != 0) 
				{
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

#ifndef NDEBUG
					m_ambiguousCubeCaseCount += ((m_isAmbiguousCubeCaseArray[cellType]) ? 1 : 0);
					
					if (m_isAmbiguousCubeCaseArray[cellType])
						continue;
#endif // #ifndef NDEBUG

					for (k=0; m_triangleTable[cellType][k]!=-1; k++)
						m_triangleMesh->m_elementArray.push_back(GetEdgeId(x, y, z, m_triangleTable[cellType][k]));
				}
			}
		}

	RenameElements();

	if (!MeshChoose())
	{
		MarkError();

		return false;
	}

	RemoveUnreferencedVertices();

#ifndef NDEBUG
	cout << "(number of ambiguous cells founded: " << m_ambiguousCubeCaseCount << ")" << endl;
#endif // #ifndef NDEBUG

	return true;
}

/**
*/
void CCombinedTransforms::SetupVertexPool(void)
{
	m_triangleMesh->m_vertexSize = 3;

	m_triangleMesh->m_mode = CVertexPool::TRIANGLES;

	m_activeEdges->m_vertexSize = 3;

	m_activeEdges->m_mode = CVertexPool::LINES;
}

/**
*/
void CCombinedTransforms::RemoveUnreferencedVertices(void)
{
	int vertexCount,
		elementCount,
		usedVertexCount,
		replacedVertexCount,
		i;
	vector<bool> isVertexUsedArray;
	unsigned int *element;
	map<unsigned int, unsigned int> vertexNameMap;
	const float *vertexCoord,
		*normalCoord;

	vertexCount = m_triangleMesh->GetVertexCount();

	isVertexUsedArray.assign(vertexCount, false);

	elementCount = m_triangleMesh->GetElementCount();

	if (elementCount)
	{
		element = &m_triangleMesh->m_elementArray[0];

		for (i=0; i<elementCount; i++)
			isVertexUsedArray[*element++] = true;

		usedVertexCount = 0;

		for (i=0; i<vertexCount; i++)
			if (isVertexUsedArray[i])
				vertexNameMap[i] = usedVertexCount++;

		replacedVertexCount = 0;

		for (i=0; i<vertexCount; i++)
			if (isVertexUsedArray[i])
			{
				normalCoord = m_triangleMesh->GetNormal(i);

				m_triangleMesh->SetNormal(replacedVertexCount, normalCoord[0], normalCoord[1], normalCoord[2]);

				vertexCoord = m_triangleMesh->GetVertex(i);

				m_triangleMesh->SetVertex(replacedVertexCount, vertexCoord[0], vertexCoord[1], vertexCoord[2]);

				replacedVertexCount++;
			}

		m_triangleMesh->m_normalArray.erase(m_triangleMesh->m_normalArray.begin() + 3*replacedVertexCount, m_triangleMesh->m_normalArray.end());

		m_triangleMesh->m_vertexArray.erase(m_triangleMesh->m_vertexArray.begin() + 3*replacedVertexCount, m_triangleMesh->m_vertexArray.end());

		element = &m_triangleMesh->m_elementArray[0];

		for (i=0; i<elementCount; i++)
		{
			*element = vertexNameMap[*element];

			element++;
		}
	}
}

/**
*/
float CCombinedTransforms::GetTriangleArrayQuality(const float *vertexArray, const int *triangleArray, int triangleCount) const
{
	const int *triangle;
	float qualityMin,
		a,
		b,
		c,
		semiperimeter,
		area,
		inradius,
		circumradius,
		quality;
	const float *A, 
		*B, 
		*C;
	int i,
		j;

	triangle = triangleArray;

	qualityMin = 1.0f;

	for (j=0; j<triangleCount; j++)
	{
		A = vertexArray + 3*triangle[0];
		B = vertexArray + 3*triangle[1];
		C = vertexArray + 3*triangle[2];
		
		a = 0.0f;
		b = 0.0f;
		c = 0.0f;
			
		for (i=3; i--; )
		{
			a += (C[i] - B[i])*(C[i] - B[i]);
			b += (A[i] - C[i])*(A[i] - C[i]);
			c += (B[i] - A[i])*(B[i] - A[i]);
		}
			
		a = sqrtf(a);
		b = sqrtf(b);
		c = sqrtf(c);

		semiperimeter = (a + b + c)*0.5f;

		area = sqrtf(semiperimeter*(semiperimeter - a)*(semiperimeter - b)*(semiperimeter - c));

		/*// Returns 0 in case or error.
		if (area < CMyMath::FloatEpsilon())
			return 0.0f;*/

		inradius = 2.0f*area/(a + b + c);

		circumradius = (a*b*c)/(4.0f*inradius*semiperimeter);

		quality = inradius/(0.5f*circumradius);

		qualityMin = min(quality, qualityMin);

		triangle += 3;
	}

	return qualityMin;
}

/**
*/
bool CCombinedTransforms::MeshChoose(void)
{
	unsigned int vertexCount,
		triangleCount,
		vertexId,
		iterationMax,
		i,
		j;
	vector<vector<int> > incidentTriangleArray;
	const unsigned int *triangle;
	vector<float> vertexQualityArray;
	float *vertexArray,
		*vertex,
		swapQuality,
		swapValue;

	vertexCount = m_triangleMesh->GetVertexCount();

	incidentTriangleArray.assign(vertexCount, vector<int>());

	triangle = m_triangleMesh->GetElementArray();
	triangleCount = m_triangleMesh->GetElementCount()/3;

	for (i=0; i<triangleCount; i++)
	{
		incidentTriangleArray[triangle[0]].push_back(triangle[0]);
		incidentTriangleArray[triangle[0]].push_back(triangle[1]);
		incidentTriangleArray[triangle[0]].push_back(triangle[2]);

		incidentTriangleArray[triangle[1]].push_back(triangle[0]);
		incidentTriangleArray[triangle[1]].push_back(triangle[1]);
		incidentTriangleArray[triangle[1]].push_back(triangle[2]);

		incidentTriangleArray[triangle[2]].push_back(triangle[0]);
		incidentTriangleArray[triangle[2]].push_back(triangle[1]);
		incidentTriangleArray[triangle[2]].push_back(triangle[2]);

		triangle += 3;
	}

	vertexQualityArray.assign(vertexCount, 0.0f);

	vertexArray = &m_triangleMesh->m_vertexArray[0];

	for (i=0; i<vertexCount/2; i++)
	{
		vertexId = 2*i + 1;

		vertexQualityArray[vertexId] = GetTriangleArrayQuality(vertexArray, &incidentTriangleArray[vertexId][0], (int)incidentTriangleArray[vertexId].size()/3);
	}

	iterationMax = 6;

	for (j=0; j<iterationMax; j++)
		for (i=0; i<vertexCount/2; i++)
		{
			vertexId = 2*i + 1;

			vertex = vertexArray + 3*vertexId;

			swapValue = *vertex;
			*vertex = *(vertex - 3);
			*(vertex - 3) = swapValue;

			vertex++;

			swapValue = *vertex;
			*vertex = *(vertex - 3);
			*(vertex - 3) = swapValue;

			vertex++;

			swapValue = *vertex;
			*vertex = *(vertex - 3);
			*(vertex - 3) = swapValue;

			swapQuality = GetTriangleArrayQuality(vertexArray, &incidentTriangleArray[vertexId][0], (int)incidentTriangleArray[vertexId].size()/3);

			if (swapQuality < vertexQualityArray[vertexId])
			{
				vertex = vertexArray + 3*vertexId;

				swapValue = *vertex;
				*vertex = *(vertex - 3);
				*(vertex - 3) = swapValue;

				vertex++;

				swapValue = *vertex;
				*vertex = *(vertex - 3);
				*(vertex - 3) = swapValue;

				vertex++;

				swapValue = *vertex;
				*vertex = *(vertex - 3);
				*(vertex - 3) = swapValue;
			}
			else
				vertexQualityArray[vertexId] = swapQuality;
		}

	return true;
}

/**
*/
int CCombinedTransforms::GenerateVertex(float x, float y, float z, int edgeName)
{
	float orgCoord[3],
		destCoord[3],
		secondOrgCoord[3],
		secondDestCoord[3];

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
	
	secondOrgCoord[0] = orgCoord[0];
	secondOrgCoord[1] = orgCoord[1];
	secondOrgCoord[2] = orgCoord[2];

	secondDestCoord[0] = destCoord[0];
	secondDestCoord[1] = destCoord[1];
	secondDestCoord[2] = destCoord[2];

	ApplyGradientTransform(orgCoord, destCoord);

	// Add the first vertex, resulting from gradient transform, which will 
	// not be indexed in the triangle list.
	FindIntersection(orgCoord, destCoord);

	ApplyTangentialTransform(secondOrgCoord, secondDestCoord);

	// Add the second vertex, resulting from tangential transform.
	return FindIntersection(secondOrgCoord, secondDestCoord);
}

/**
*/
int CCombinedTransforms::GetEdgeId(int x, int y, int z, int edgeName) const
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
int CCombinedTransforms::GetVertexId(int x, int y, int z) const
{
	return 3*(m_gridWidth*(z*m_gridHeight + y) + x);
}

/**
*/
void CCombinedTransforms::RenameElements(void)
{
	vector<unsigned int>::iterator element;

	for (element=m_triangleMesh->m_elementArray.begin(); element!=m_triangleMesh->m_elementArray.end(); element++)
		(*element) = m_vertexToEdgeMap[(*element)];

	m_vertexToEdgeMap.clear();
}

