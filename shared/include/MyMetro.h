#ifndef MYMETRO_INCLUDED
#define MYMETRO_INCLUDED

#include <string>

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

#include <GageAdaptor.h>
#include <VertexPool.h>

class CMyMetro
	: boost::noncopyable
{
public:
	CMyMetro(void);
	virtual ~CMyMetro(void);
	bool Load(boost::shared_ptr<CGageAdaptor>& image, float isoLevel, boost::shared_ptr<CVertexPool> mesh);
	bool MeasureError(void);
	float GetMinError(void) const;
	float GetMaxError(void) const;
	bool SaveErrorHistogram(const std::string& fileName, float minError, float maxError);
protected:
	float GetSampleError(const float* vertexCoord) const;
	bool MeasureErrorOfTriangleMesh(void);
	bool MeasureErrorOfQuadMesh(void);
	void Create(void);
private:
	boost::shared_ptr<CGageAdaptor> m_image;
	float m_isoLevel;
	boost::shared_ptr<CVertexPool> m_mesh;
	float m_minError;
	float m_maxError;
};

#endif // MYMETRO_INCLUDED

