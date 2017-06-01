#include <MyMath.h>
#include <ErrorManager.h>

#include <BoundingBox.h>

using namespace std;

/**
*/
CBoundingBox::CBoundingBox(void)
{
	Create();
}

/**
*/
CBoundingBox::~CBoundingBox(void)
{
}

/**
*/
bool CBoundingBox::SetMesh(const CVertexPool *mesh)
{
	int vertexCount,
		i;
	const float *vertex;

	if (mesh == 0)
	{
		MarkError();

		return false;
	}

	vertexCount = mesh->GetVertexCount();

	m_xMin = MyMath::GetMaximumPositiveValue<float>();
	m_xMax = -MyMath::GetMaximumPositiveValue<float>();
	m_yMin = MyMath::GetMaximumPositiveValue<float>();
	m_yMax = -MyMath::GetMaximumPositiveValue<float>();
	m_zMin = MyMath::GetMaximumPositiveValue<float>();
	m_zMax = -MyMath::GetMaximumPositiveValue<float>();

	vertex = mesh->GetVertexArray();

	for (i=0; i!=vertexCount; i++)
	{
		if (m_xMin > *vertex)
			m_xMin = *vertex;
		if (m_xMax < *vertex)
			m_xMax = *vertex;

		vertex++;

		if (m_yMin > *vertex)
			m_yMin = *vertex;
		if (m_yMax < *vertex)
			m_yMax = *vertex;

		vertex++;

		if (m_zMin > *vertex)
			m_zMin = *vertex;
		if (m_zMax < *vertex)
			m_zMax = *vertex;

		vertex++;
	}

	return true;
}

bool CBoundingBox::SetMesh(boost::shared_ptr<CVertexPool> mesh)
{
	if (!mesh.get())
	{
		MarkError();

		return false;
	}

	return SetMesh(mesh.get());
}

/**
*/
void CBoundingBox::Create(void)
{
	m_xMin = 0.0f;
	m_xMax = 0.0f;
	m_yMin = 0.0f;
	m_yMax = 0.0f;
	m_zMin = 0.0f;
	m_zMax = 0.0f;
}

