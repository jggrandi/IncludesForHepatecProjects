#ifndef MYMESHUTILS_INCLUDED
#define MYMESHUTILS_INCLUDED

#include <boost/shared_ptr.hpp>

#include <VertexPool.h>

namespace MyMeshUtils
{
	/**
	*/
	bool FitTo(CVertexPool *mesh, float xMin, float yMin, float zMin, float xMax, float yMax, float zMax);

	/**
	*/
	bool GenerateNormals(CVertexPool *mesh);

	/**
	*/
	bool SmoothNormals(CVertexPool *mesh);
};

#endif // MYMESHUTILS_INCLUDED

