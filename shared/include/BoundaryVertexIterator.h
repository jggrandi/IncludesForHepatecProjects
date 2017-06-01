#ifndef BOUNDARYVERTEXITERATOR_INCLUDED
#define BOUNDARYVERTEXITERATOR_INCLUDED

#include <vector>
#include <utility>

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

#include <VertexPool.h>

class CBoundaryVertexIterator 
	: boost::noncopyable
{
public:
	CBoundaryVertexIterator(void);
	~CBoundaryVertexIterator(void);
	CBoundaryVertexIterator* operator++(void);
	bool SetMesh(boost::shared_ptr<CVertexPool> vertexPool);
	void Initialize(int borderId = 0);
	bool NotFinished(void) const;
	unsigned int GetId(void) const;
	int GetBoundaryVertexCount(void) const;
	int GetBoundaryCount(void) const;
	bool IsOnBoundary(int vertexId);
protected:
	virtual void Create(void);
public:
	std::vector<std::vector<std::pair<unsigned int, unsigned int> > > m_neighborArray;
	std::vector<std::vector<unsigned int> > m_boundaryVertexArray;
	std::vector<unsigned int>::iterator m_cursor;
	unsigned int m_borderId;
};

#endif // BOUNDARYVERTEXITERATOR_INCLUDED

