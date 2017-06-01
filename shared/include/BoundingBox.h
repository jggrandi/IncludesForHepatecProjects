#ifndef BOUNDINGBOX_INCLUDED
#define BOUNDINGBOX_INCLUDED

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

#include <VertexPool.h>

class CBoundingBox 
	/*: boost::noncopyable*/
{
public:
	CBoundingBox(void);
	virtual ~CBoundingBox(void);
	bool SetMesh(const CVertexPool *mesh);
	bool SetMesh(boost::shared_ptr<CVertexPool> mesh);
protected:
	virtual void Create(void);
public:
	float m_xMin;
	float m_xMax;
	float m_yMin;
	float m_yMax;
	float m_zMin;
	float m_zMax;
};

#endif // BOUNDINGBOX_INCLUDED

