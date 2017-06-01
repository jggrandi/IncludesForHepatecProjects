#ifndef IMARCHINGMETHOD_INCLUDED
#define IMARCHINGMETHOD_INCLUDED

//#define MY_USING_DISPLACEMENT_TRANSFORM

#include <string>
#include <vector>

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

#include <GageAdaptor.h>
#include <VertexPool.h>

class IMarchingMethod
	: boost::noncopyable
{
public:
	enum EDGE_TRANSFORM {
		NONE,
		GRADIENT_TRANSFORM,
		TANGENTIAL_TRANSFORM,
		COMBINED_TRANSFORM,
		PROJECTION_TRANSFORM
	};
	IMarchingMethod(void);
	virtual ~IMarchingMethod(void);
	bool Load(boost::shared_ptr<CGageAdaptor>& image);
	bool LoadSpanSpace(const std::string& spanSpaceFileName);
	virtual bool SetTransform(EDGE_TRANSFORM edgeTransform);
	EDGE_TRANSFORM GetTransform(void) const;
	virtual bool SetCellSize(float width, float height, float depth);
	void GetCellSize(float *width, float *height, float *depth) const;
	virtual bool SetGridSize(int width, int height, int depth);
	void GetGridSize(int *width, int *height, int *depth) const;
	virtual bool GenerateMesh(float isoLevel) = 0;
	float GetIsoLevel(void) const;
	boost::shared_ptr<CVertexPool> GetMesh(void) const;
	boost::shared_ptr<CVertexPool> GetActiveEdges(void) const;
	float GetAlgebraicError(void) const;
	bool SetTransformationStepCount(int stepCount);
	bool IsValid(void) const;
protected:
	virtual bool GenerateMeshFromSpanSpace(float isoLevel);
	virtual void SetupVertexPool(void) = 0;
	void DeleteMesh(void);
	void TransformEdge(float *orgCoord, float *destCoord);// const;
	void ApplyGradientTransform(float *orgCoord, float *destCoord);//; const;
	void ApplyTangentialTransform(float *orgCoord, float *destCoord);//; const;
	void ApplyProjectionTransform(float *orgCoord, float *destCoord) const;
	int FindIntersectionByLinearInterpolation(float *orgCoord, float *destCoord);
	int FindIntersectionByBisection(float *orgCoord, float *destCoord);
	virtual int FindIntersection(float *orgCoord, float *destCoord);
private:
	void Create(void);
protected:
	boost::shared_ptr<CGageAdaptor> m_image;
	float m_cellWidth;
	float m_cellHeight;
	float m_cellDepth;
	int m_gridWidth;
	int m_gridHeight;
	int m_gridDepth;
	// Thesis only!
public:
	boost::shared_ptr<CVertexPool> m_triangleMesh;
protected:
	boost::shared_ptr<CVertexPool> m_activeEdges;
	int m_bisectionStepsMax;
	EDGE_TRANSFORM m_edgeTransform;
	int m_transformStepsMax;
	float m_isoLevel;
	float m_algebraicError;
	// TEST ONLY!
	bool m_isUsingSpanSpace;
	int m_spanSpaceWidth;
	int m_spanSpaceHeight;
	float m_imageValueMin;
	float m_imageValueMax;
	std::vector<std::vector<long> > m_spanSpace;
	// Thesis only!
	//int m_tangentialTransformFailureCount;
	//int m_gradientTransformFailureCount;
};

#endif // IMARCHINGMETHOD_INCLUDED

