#define NUMBER_OF_SAMPLES 10
#define ERROR_HISTOGRAM_BINS 100

#include <algorithm>
#include <fstream>
#include <iostream>

#include <cmath>

#include <ErrorManager.h>
#include <MyMath.h>

#include <MyMetro.h>

using namespace std;

/**
*/
template <typename T, int D>
T GetTriangleArea(const T *A, const T *B, const T *C)
{
	T a,
		b,
		c,
		semiperimeter,
		area;
	int i;
		
	a = (C[0] - B[0])*(C[0] - B[0]);
	b = (A[0] - C[0])*(A[0] - C[0]);
	c = (B[0] - A[0])*(B[0] - A[0]);
		
	for (i=1; i<D; i++)
	{
		a += (C[i] - B[i])*(C[i] - B[i]);
		b += (A[i] - C[i])*(A[i] - C[i]);
		c += (B[i] - A[i])*(B[i] - A[i]);
	}
		
    a = T(sqrt(a));
    b = T(sqrt(b));
	c = T(sqrt(c));

	semiperimeter = (a + b + c)*T(0.5);

    area = T(sqrt(semiperimeter*(semiperimeter - a)*(semiperimeter - b)*(semiperimeter - c)));

	return area;
}

/**
*/
template <typename T, int D>
bool GetBarycentricCoords(const T *A, const T *B, const T *C, const T *pointCoord, T *barycentricCoord)
{
	T area;

	area = GetTriangleArea<T, D>(A, B, C);

	if (area > MyMath::GetMinimumPositiveValue<float>())
	{
		barycentricCoord[0] = GetTriangleArea<T, D>(B, C, pointCoord)/area;
		barycentricCoord[1] = GetTriangleArea<T, D>(A, C, pointCoord)/area;
		barycentricCoord[2] = GetTriangleArea<T, D>(A, B, pointCoord)/area;
	}
	// The triangle is degenerate (the three vertices are collinear or are 
	// collapsed to a point).
	else
		return false;

	return true;
}

/**
*/
CMyMetro::CMyMetro(void)
{
	Create();
}

/**
*/
CMyMetro::~CMyMetro(void)
{
}

/**
*/
bool CMyMetro::Load(boost::shared_ptr<CGageAdaptor>& image, float isoLevel, boost::shared_ptr<CVertexPool> mesh)
{
	if (!image.get())
	{
		MarkError();

		return false;
	}

	if (!image->IsOpen())
	{
		MarkError();

		return false;
	}

	if (!mesh.get())
	{
		MarkError();

		return false;
	}

	if (!mesh->IsValid())
	{
		MarkError();

		return false;
	}

	m_image = image;

	m_isoLevel = isoLevel;

	m_mesh = mesh;

	return true;
}

/**
*/
bool CMyMetro::MeasureError(void)
{
	if (!m_mesh.get())
	{
		MarkError();

		return false;
	}

	switch (m_mesh->m_mode)
	{
	case CVertexPool::TRIANGLES:
		MeasureErrorOfTriangleMesh();
		break;
	case CVertexPool::QUADS:
		MeasureErrorOfQuadMesh();
		break;
	default:
		MarkError();

		return false;
	};

	return true;
}

/**
*/
float CMyMetro::GetMinError(void) const
{
	return m_minError;
}

/**
*/
float CMyMetro::GetMaxError(void) const
{
	return m_maxError;
}

/**
*/
float CMyMetro::GetSampleError(const float* vertexCoord) const
{
	return m_image->GetValue(vertexCoord[0], vertexCoord[1], vertexCoord[2]) - m_isoLevel;
}

/**
*/
bool CMyMetro::SaveErrorHistogram(const std::string& fileName, float minError, float maxError)
{
	int histogramBin,
		triangleCount,
		baseBarycentricCoordCount,
		i,
		j,
		k;
	float baseTriangleVertexArray[3][2] = {
		{0.0f, 0.0f},
		{float(NUMBER_OF_SAMPLES), 0.0f},
		{float(NUMBER_OF_SAMPLES), float(NUMBER_OF_SAMPLES)}
	};
	float basePointCoord[2],
		baseBarycentricCoord[3],
		b0,
		b1,
		b2,
		sampleVertexCoord[3],
		value;
	vector<float> barycentricCoordArray;
	const unsigned int *triangleElementArray;
	const float *firstVertexCoord,
		*secondVertexCoord,
		*thirdVertexCoord;
	vector<unsigned int> errorHistogram;
	ofstream errorHistogramFileStream;

	errorHistogram.assign(ERROR_HISTOGRAM_BINS + 1, 0);

	m_minError = MyMath::GetMaximumPositiveValue<float>();
	m_maxError = -MyMath::GetMaximumPositiveValue<float>();
	
	triangleCount = m_mesh->GetPrimitiveCount();

	for (i=0; i<(NUMBER_OF_SAMPLES + 1); ++i)
		for (j=i; j<(NUMBER_OF_SAMPLES + 1); ++j)
		{
			basePointCoord[0] = float(j);
			basePointCoord[1] = float(i);

			if (GetBarycentricCoords<float, 2>(baseTriangleVertexArray[0], baseTriangleVertexArray[1], baseTriangleVertexArray[2], basePointCoord, baseBarycentricCoord))
				barycentricCoordArray.insert(barycentricCoordArray.end(), baseBarycentricCoord, baseBarycentricCoord + 3);
		}

	baseBarycentricCoordCount = int(barycentricCoordArray.size())/3;

	for (k=0; k<triangleCount; ++k)
	{
		triangleElementArray = m_mesh->GetPrimitive(k);

		firstVertexCoord = m_mesh->GetVertex(triangleElementArray[0]);
		secondVertexCoord = m_mesh->GetVertex(triangleElementArray[1]);
		thirdVertexCoord = m_mesh->GetVertex(triangleElementArray[2]);

		for (i=0; i<baseBarycentricCoordCount; ++i)
		{
			b0 = barycentricCoordArray[i*3];
			b1 = barycentricCoordArray[i*3 + 1];
			b2 = barycentricCoordArray[i*3 + 2];

			sampleVertexCoord[0] = b0*firstVertexCoord[0] + b1*secondVertexCoord[0] + b2*thirdVertexCoord[0];
			sampleVertexCoord[1] = b0*firstVertexCoord[1] + b1*secondVertexCoord[1] + b2*thirdVertexCoord[1];
			sampleVertexCoord[2] = b0*firstVertexCoord[2] + b1*secondVertexCoord[2] + b2*thirdVertexCoord[2];

			value = fabs(GetSampleError(sampleVertexCoord));

			histogramBin = int(floorf(((value - minError)/(maxError - minError))*float(ERROR_HISTOGRAM_BINS) + 0.5f));

			errorHistogram[histogramBin]++;
		}
	}

	errorHistogramFileStream.open(fileName.c_str());

	if (!errorHistogramFileStream.is_open())
	{
		MarkError();

		return false;
	}

	for (i=0; i<(ERROR_HISTOGRAM_BINS + 1); i++)
		errorHistogramFileStream << float(i)/float(ERROR_HISTOGRAM_BINS) << " " << errorHistogram[i] << endl;

	return true;
}

/**
*/
bool CMyMetro::MeasureErrorOfTriangleMesh(void)
{
	int triangleCount,
		baseBarycentricCoordCount,
		i,
		j,
		k;
	float baseTriangleVertexArray[3][2] = {
		{0.0f, 0.0f},
		{float(NUMBER_OF_SAMPLES), 0.0f},
		{float(NUMBER_OF_SAMPLES), float(NUMBER_OF_SAMPLES)}
	};
	float basePointCoord[2],
		baseBarycentricCoord[3],
		b0,
		b1,
		b2,
		sampleVertexCoord[3],
		value;
	vector<float> barycentricCoordArray;
	const unsigned int *triangleElementArray;
	const float *firstVertexCoord,
		*secondVertexCoord,
		*thirdVertexCoord;

	m_minError = MyMath::GetMaximumPositiveValue<float>();
	m_maxError = -MyMath::GetMaximumPositiveValue<float>();
	
	triangleCount = m_mesh->GetPrimitiveCount();

	for (i=0; i<(NUMBER_OF_SAMPLES + 1); ++i)
		for (j=i; j<(NUMBER_OF_SAMPLES + 1); ++j)
		{
			basePointCoord[0] = float(j);
			basePointCoord[1] = float(i);

			if (GetBarycentricCoords<float, 2>(baseTriangleVertexArray[0], baseTriangleVertexArray[1], baseTriangleVertexArray[2], basePointCoord, baseBarycentricCoord))
				barycentricCoordArray.insert(barycentricCoordArray.end(), baseBarycentricCoord, baseBarycentricCoord + 3);
		}

	baseBarycentricCoordCount = int(barycentricCoordArray.size())/3;

	for (k=0; k<triangleCount; ++k)
	{
		triangleElementArray = m_mesh->GetPrimitive(k);

		firstVertexCoord = m_mesh->GetVertex(triangleElementArray[0]);
		secondVertexCoord = m_mesh->GetVertex(triangleElementArray[1]);
		thirdVertexCoord = m_mesh->GetVertex(triangleElementArray[2]);

		for (i=0; i<baseBarycentricCoordCount; ++i)
		{
			b0 = barycentricCoordArray[i*3];
			b1 = barycentricCoordArray[i*3 + 1];
			b2 = barycentricCoordArray[i*3 + 2];

			sampleVertexCoord[0] = b0*firstVertexCoord[0] + b1*secondVertexCoord[0] + b2*thirdVertexCoord[0];
			sampleVertexCoord[1] = b0*firstVertexCoord[1] + b1*secondVertexCoord[1] + b2*thirdVertexCoord[1];
			sampleVertexCoord[2] = b0*firstVertexCoord[2] + b1*secondVertexCoord[2] + b2*thirdVertexCoord[2];

			value = GetSampleError(sampleVertexCoord);

			m_minError = min(m_minError, value);
			m_maxError = max(m_maxError, value);
		}
	}

	return true;
}

/**
*/
bool CMyMetro::MeasureErrorOfQuadMesh(void)
{
	return false;
}

/**
*/
void CMyMetro::Create(void)
{
	m_image.reset();

	m_isoLevel = 0.0f;
	
	m_mesh.reset();

	m_minError = 0.0f;
	m_maxError = 0.0f;
}

