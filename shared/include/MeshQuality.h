#ifndef MESHQUALITY_INCLUDED
#define MESHQUALITY_INCLUDED

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

#include <VertexPool.h>

class CMeshQuality
	: boost::noncopyable
{
public:
	enum TriangleQualityEnum {
		RADII_RATIO,
		EDGE_RATIO,
		ASPECT_RATIO,
		CIRCUMRADIUS_TO_MAX_EDGE_RATIO,
		CIRCUMRADIUS_TO_SEMIPERIMETER_RATIO,
		NORMALIZED_AREA,
		MINIMUM_INTERNAL_ANGLE,
		MAXIMUM_INTERNAL_ANGLE,
	};
	CMeshQuality(void);
	virtual ~CMeshQuality(void);
	bool Load(boost::shared_ptr<CVertexPool> mesh);
	bool Load(int vertexCount, int vertexSize, int vertexOffset, const float *vertexArray, int elementCount, const unsigned int *elementArray, CVertexPool::POLYGON_MODE polygonMode);
	static bool SetMeasure(TriangleQualityEnum qualityMeasure);
	bool MeasureQuality(void);
	float GetVertexQuality(int vertexName) const;
	float GetMinQuality(void) const;
	float GetMaxQuality(void) const;
	float GetAverageQuality(void) const;
	int GetDegenerateTriangleCount(void) const;
	static float GetTriangleQuality(const float *A, const float *B, const float *C);
	static float GetMinimumTriangleAngle(const float *A, const float *B, const float *C);
	static float GetMaximumTriangleAngle(const float *A, const float *B, const float *C);
protected:
	void Create(void);
	bool MeasureQualityOfTriangleMesh(void);
	bool MeasureQualityOfQuadMesh(void);
private:
	boost::shared_ptr<CVertexPool> m_mesh;
	static TriangleQualityEnum m_qualityMeasure;
	std::vector<float> m_qualityArray;
	float m_minQuality;
	float m_maxQuality;
	float m_averageQuality;
	int m_degenerateTriangleCount;
};

#endif // MESHQUALITY_INCLUDED

