#include <cmath>

#include <ErrorManager.h>
#include <MyMath.h>
#include <BoundingBox.h>

#include <MyMeshUtils.h>

using namespace std;

namespace MyMeshUtils 
{
	/**
	*/
	bool FitTo(CVertexPool *mesh, float xMin, float yMin, float zMin, float xMax, float yMax, float zMax)
	{
		CBoundingBox boundingBox;
		int vertexCount,
			i;
		const float *vertexCoord;
		float scale,
			vertexCoordFitted[3];

		if (!mesh)
		{
			MarkError();

			return false;
		}

		if (!mesh->IsValid())
		{
			MarkError();

			return false;
		}

		if (mesh->GetVertexSize() != 3)
		{
			MarkError();

			return false;
		}

		if (!boundingBox.SetMesh(mesh))
		{
			MarkError();

			return false;
		}

		scale = (xMax - xMin)/(boundingBox.m_xMax - boundingBox.m_xMin);
		scale = min(scale, (yMax - yMin)/(boundingBox.m_yMax - boundingBox.m_yMin));
		scale = min(scale, (zMax - zMin)/(boundingBox.m_zMax - boundingBox.m_zMin));

		vertexCount = mesh->GetVertexCount();

		for (i=0; i!=vertexCount; i++)
		{
			vertexCoord = mesh->GetVertex(i);

			vertexCoordFitted[0] = scale*(vertexCoord[0] - boundingBox.m_xMin) + xMin;
			vertexCoordFitted[1] = scale*(vertexCoord[1] - boundingBox.m_yMin) + yMin;
			vertexCoordFitted[2] = scale*(vertexCoord[2] - boundingBox.m_zMin) + zMin;

			mesh->SetVertex(i, vertexCoordFitted[0], vertexCoordFitted[1], vertexCoordFitted[2]);
		}

		return true;
	}

	/**
	*/
	bool GenerateNormals(CVertexPool *mesh)
	{
		int vertexCount,
			triangleCount,
			i,
			j,
			k;
		const unsigned int *triangle;
		const float *firstTriangleVertexCoord,
			*secondTriangleVertexCoord,
			*thirdTriangleVertexCoord,
			*vertexNormal;
		float triangleNormal[3],
			temporaryVertexNormal[3],
			temporaryVertexNormalMagnitude;

		if (!mesh->IsValid())
		{
			MarkError();

			return false;
		}

		if (mesh->GetMode() != CVertexPool::TRIANGLES)
		{
			MarkError();

			return false;
		}

		vertexCount = mesh->GetVertexCount();

		mesh->m_normalArray.assign(3*vertexCount, 0.0f);

		triangleCount = mesh->GetPrimitiveCount();

		triangle = mesh->GetElementArray();

		for (k=0 ; k!=triangleCount; k++)
		{
			if (!triangle)
			{
				MarkError();

				return false;
			}

			firstTriangleVertexCoord = mesh->GetVertex(triangle[0]);

			if (!firstTriangleVertexCoord)
			{
				MarkError();

				return false;
			}

			secondTriangleVertexCoord = mesh->GetVertex(triangle[2]);

			if (!secondTriangleVertexCoord)
			{
				MarkError();

				return false;
			}

			thirdTriangleVertexCoord = mesh->GetVertex(triangle[1]);

			if (!thirdTriangleVertexCoord)
			{
				MarkError();

				return false;
			}

			triangleNormal[0] = (secondTriangleVertexCoord[1] - firstTriangleVertexCoord[1])*(thirdTriangleVertexCoord[2] - firstTriangleVertexCoord[2]) - (secondTriangleVertexCoord[2] - firstTriangleVertexCoord[2])*(thirdTriangleVertexCoord[1] - firstTriangleVertexCoord[1]);
			triangleNormal[1] = (secondTriangleVertexCoord[2] - firstTriangleVertexCoord[2])*(thirdTriangleVertexCoord[0] - firstTriangleVertexCoord[0]) - (secondTriangleVertexCoord[0] - firstTriangleVertexCoord[0])*(thirdTriangleVertexCoord[2] - firstTriangleVertexCoord[2]);
			triangleNormal[2] = (secondTriangleVertexCoord[0] - firstTriangleVertexCoord[0])*(thirdTriangleVertexCoord[1] - firstTriangleVertexCoord[1]) - (secondTriangleVertexCoord[1] - firstTriangleVertexCoord[1])*(thirdTriangleVertexCoord[0] - firstTriangleVertexCoord[0]);

			MyMath::Normalize<float, 3>(triangleNormal);

			for (j=0; j!=3; j++)
			{
				vertexNormal = mesh->GetNormal(triangle[j]);

				if (!vertexNormal)
				{
					MarkError();

					return false;
				}

				for (i=0; i!=3; i++)
					temporaryVertexNormal[i] = vertexNormal[i] + triangleNormal[i];

				mesh->SetNormal(triangle[j], temporaryVertexNormal[0], temporaryVertexNormal[1], temporaryVertexNormal[2]);
			}

			triangle += 3;
		}

		for (j=0 ; j!=vertexCount; j++)
		{
			vertexNormal = mesh->GetNormal(j);

			if (!vertexNormal)
			{
				MarkError();

				return false;
			}

			temporaryVertexNormalMagnitude = 0.0f;

			for (i=0; i!=3; i++)
				temporaryVertexNormalMagnitude += vertexNormal[i]*vertexNormal[i];

			temporaryVertexNormalMagnitude = sqrtf(temporaryVertexNormalMagnitude);

			if (temporaryVertexNormalMagnitude > 1.0e-3)
				for (i=0; i!=3; i++)
					temporaryVertexNormal[i] = vertexNormal[i]/temporaryVertexNormalMagnitude;

			mesh->SetNormal(j, temporaryVertexNormal[0], temporaryVertexNormal[1], temporaryVertexNormal[2]);
		}

		return true;
	}

	/**
	*/
	bool SmoothNormals(CVertexPool *mesh)
	{
		int vertexCount,
			triangleCount,
			j,
			k;
		const unsigned int *triangle;
		const float *firstTriangleNormalCoord,
			*secondTriangleNormalCoord,
			*thirdTriangleNormalCoord;
		float normalAverage[3],
			*vertexNormal;
		vector<float> normalArray;

		if (!mesh->IsValid())
		{
			MarkError();

			return false;
		}

		if (mesh->GetMode() != CVertexPool::TRIANGLES)
		{
			MarkError();

			return false;
		}

		vertexCount = mesh->GetVertexCount();

		normalArray.assign(3*vertexCount, 0.0f);

		triangleCount = mesh->GetPrimitiveCount();

		triangle = mesh->GetElementArray();

		for (k=0 ; k!=triangleCount; k++)
		{
			if (!triangle)
			{
				MarkError();

				return false;
			}

			firstTriangleNormalCoord = mesh->GetNormal(triangle[0]);

			if (!firstTriangleNormalCoord)
			{
				MarkError();

				return false;
			}

			secondTriangleNormalCoord = mesh->GetNormal(triangle[2]);

			if (!secondTriangleNormalCoord)
			{
				MarkError();

				return false;
			}

			thirdTriangleNormalCoord = mesh->GetNormal(triangle[1]);

			if (!thirdTriangleNormalCoord)
			{
				MarkError();

				return false;
			}

			for (j=0; j<3; ++j)
				normalAverage[j] = 0.5f*(secondTriangleNormalCoord[j] + thirdTriangleNormalCoord[j]);

			MyMath::Normalize<float, 3>(normalAverage);

			for (j=0; j<3; ++j)
				normalArray[3*triangle[0] + j] += normalAverage[j];

			for (j=0; j<3; ++j)
				normalAverage[j] = 0.5f*(firstTriangleNormalCoord[j] + thirdTriangleNormalCoord[j]);

			MyMath::Normalize<float, 3>(normalAverage);

			for (j=0; j<3; ++j)
				normalArray[3*triangle[1] + j] += normalAverage[j];

			for (j=0; j<3; ++j)
				normalAverage[j] = 0.5f*(firstTriangleNormalCoord[j] + secondTriangleNormalCoord[j]);

			MyMath::Normalize<float, 3>(normalAverage);

			for (j=0; j<3; ++j)
				normalArray[3*triangle[2] + j] += normalAverage[j];

			triangle += 3;
		}

		vertexNormal = &normalArray[0];

		for (j=0 ; j!=vertexCount; j++)
		{
			if (!vertexNormal)
			{
				MarkError();

				return false;
			}

			MyMath::Normalize<float, 3>(vertexNormal);

			mesh->SetNormal(j, vertexNormal[0], vertexNormal[1], vertexNormal[2]);

			vertexNormal += 3;
		}

		return true;
	}
}

