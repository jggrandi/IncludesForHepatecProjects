#ifndef MYGLH_INCLUDED
#define MYGLH_INCLUDED

#ifdef MY_USING_POINT_SPRITES
#define MY_ENABLE_POINT_SPRITES

#include <Texture.h>
#endif // MY_USING_POINT_SPRITES

#include <ErrorManager.h>

namespace MyGLH 
{
//#ifdef _WIN32
//	wglSwapIntervalEXT( 0 );
//#else
//	glXSwapIntervalSGI( 0 );
//#endif

	/**
	*/
	bool IsRenderingContextOk(void);

	/**
	*/
	void DrawGrid(void);

	/**
	*/
	float *GetTriangleNormal(const float *A, const float *B, const float *C, float *normal);

	/**
	*/
	void MultiplyTransformMatrices(const float *firstMatrix, const float *secondMatrix, float *productMatrix);
};

class CMyGLH
{
public:
	static void DrawLine(float *originCoord, float *destinationCoord, int widthMax);
	static bool DrawCylinder(float *originCoord, float *destinationCoord, float bottomRadius, float topRadius, int slices);
	static void TransformVertex3(const float *transformMatrix, const float *inputVertex, float *outputVertex);
	static void TransformVertex4(const float *transformMatrix, const float *inputVertex, float *outputVertex);
	static void MultiplyTransformMatrix4x4(const float *firstMatrix, const float *secondMatrix, float *productMatrix);
	static bool InvertTransformMatrix(const float *transformMatrix, float *inverseTransformMatrix);
	static void TransposeTransformMatrix(float *transformMatrix);
	static bool ProjectVertex3(const float viewMatrix[16], const float projectionMatrix[16], const int viewportCoord[4], const float *inputVertex, float *outputVertex);
	static bool UnprojectVertex3(const float viewMatrix[16], const float projectionMatrix[16], const int viewportCoord[4], const float *inputVertex, float *outputVertex);
#ifdef MY_USING_POINT_SPRITES
	static bool EnablePointSprites(void);
	static void DisablePointSprites(void);
#endif
protected:
#ifdef MY_USING_POINT_SPRITES
	static CTexture* m_pointSpriteTexturePointer;
#endif // MY_USING_POINT_SPRITES
};

#endif // MYGLH_INCLUDED

