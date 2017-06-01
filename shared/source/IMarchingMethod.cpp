#define BISECTION_STEPCOUNT_DEFAULT 8
#define TRANSFORMATION_STEPCOUNT_DEFAULT 8

#include <cmath>
#include <fstream>
#include <algorithm>

#include <FloatHack.h>

#include <ErrorManager.h>
#include <MyMath.h>

#include <IMarchingMethod.h>

using namespace std;

/**
*/
IMarchingMethod::IMarchingMethod(void)
{
	Create();
}

/**
*/
IMarchingMethod::~IMarchingMethod(void)
{
}

/**
*/
bool IMarchingMethod::Load(boost::shared_ptr<CGageAdaptor>& image)
{
	if (!image.get())
	{
		MarkError();

		return false;
	}

	m_image = image;

	return true;
}

/**
*/
bool IMarchingMethod::LoadSpanSpace(const std::string& spanSpaceFileName)
{
	ifstream spanSpaceFileStream;
	int spanSpaceCellPerPixelCount,
		i;
	long cellIndex;

	spanSpaceFileStream.open(spanSpaceFileName.c_str(), ios::binary);

	if (!spanSpaceFileStream.is_open())
		return false;

	spanSpaceFileStream.read((char *)&m_spanSpaceWidth, sizeof(int));
	spanSpaceFileStream.read((char *)&m_spanSpaceHeight, sizeof(int));
	
	spanSpaceFileStream.read((char *)&m_imageValueMin, sizeof(float));
	spanSpaceFileStream.read((char *)&m_imageValueMax, sizeof(float));
	
	m_spanSpace.assign(m_spanSpaceWidth*m_spanSpaceHeight, vector<long>());

	for (i=0; i<(m_spanSpaceWidth*m_spanSpaceHeight); ++i)
	{
		if (spanSpaceFileStream.eof())
			return false;
		
		spanSpaceFileStream.read((char *)&spanSpaceCellPerPixelCount, sizeof(int));

		while ((spanSpaceCellPerPixelCount--) != 0)
		{
			spanSpaceFileStream.read((char *)&cellIndex, sizeof(long));

			m_spanSpace[i].push_back(cellIndex);
		}
	}

	m_isUsingSpanSpace = true;

	return true;
}

/**
*/
bool IMarchingMethod::SetTransform(EDGE_TRANSFORM edgeTransform)
{
	m_edgeTransform = edgeTransform;

	return true;
}

IMarchingMethod::EDGE_TRANSFORM IMarchingMethod::GetTransform(void) const
{
	return m_edgeTransform;
}

/**
*/
bool IMarchingMethod::SetCellSize(float width, float height, float depth)
{
	int gridWidth,
		gridHeight,
		gridDepth;

	if (!m_image.get())
	{
		MarkError();

		return false;
	}

	if ((width < FLT_EPSILON) || (height < FLT_EPSILON) || (depth < FLT_EPSILON))
	{
		MarkError();

		return false;
	}

	gridWidth = (int)(m_image->GetWidth()/width);

	if (gridWidth == 0)
	{
		gridWidth = 1;

		width = (float)m_image->GetWidth();
	}

	gridHeight = (int)(m_image->GetHeight()/height);

	if (gridHeight == 0)
	{
		gridHeight = 1;

		height = (float)m_image->GetHeight();
	}

	gridDepth = (int)(m_image->GetDepth()/depth);

	if (gridDepth == 0)
	{
		gridDepth = 1;

		depth = (float)m_image->GetDepth();
	}

	m_cellWidth = width;
	m_cellHeight = height;
	m_cellDepth = depth;
	
	m_gridWidth = gridWidth;
	m_gridHeight = gridHeight;
	m_gridDepth = gridDepth;

	return true;
}

/**
*/
void IMarchingMethod::GetCellSize(float *width, float *height, float *depth) const
{
	*width = m_cellWidth;
	*height = m_cellHeight;
	*depth = m_cellDepth;
}

/**
*/
bool IMarchingMethod::SetGridSize(int width, int height, int depth)
{
	float cellWidth,
		cellHeight,
		cellDepth;

	if (!m_image.get())
	{
		MarkError();

		return false;
	}

	if ((width < 1) || (height < 1) || (depth < 1))
	{
		MarkError();

		return false;
	}

	cellWidth = (float)m_image->GetWidth()/width;
	cellHeight = (float)m_image->GetHeight()/height;
	cellDepth = (float)m_image->GetDepth()/depth;

	m_cellWidth = cellWidth;
	m_cellHeight = cellHeight;
	m_cellDepth = cellDepth;
	
	m_gridWidth = width;
	m_gridHeight = height;
	m_gridDepth = depth;

	return true;
}

/**
*/
void IMarchingMethod::GetGridSize(int *width, int *height, int *depth) const
{
	*width = m_gridWidth;
	*height = m_gridHeight;
	*depth = m_gridDepth;
}

/**
*/
void IMarchingMethod::TransformEdge(float *orgCoord, float *destCoord)// const
{
	if (m_image.get())
	{
		switch (m_edgeTransform) {
			case GRADIENT_TRANSFORM:
				ApplyGradientTransform(orgCoord, destCoord);
				break;
			case TANGENTIAL_TRANSFORM:
				ApplyTangentialTransform(orgCoord, destCoord);
				break;
			case PROJECTION_TRANSFORM:
				ApplyProjectionTransform(orgCoord, destCoord);
				break;
			case NONE:
				break;
		}
	}
	else
		MarkError();
}

/**
*/
void IMarchingMethod::ApplyGradientTransform(float *orgCoord, float *destCoord)// const
{
	float maxDisplacementOnTrail,
		orgValue,
		destValue,
		epsilonValue,
		orgValueTranslated,
		destValueTranslated,
		probeVectorLength,
		orgNormal[3],
		orgValueProbe,
		orgNormalSign,
		newOrgCoord[3],
		destNormal[3],
		destValueProbe,
		destNormalSign,
		newDestCoord[3],
		newOrgValueTranslated,
		newDestValueTranslated;
	const float *normalAnswer;
	int i,
		j;

	// The gradient transform was designed to work only with planar 
	// isosurfaces, and thus it should be adjusted in most of the practical 
	// cases. We can, however, avoid collisions between active edges by 
	// constraining the space where their endpoints move. If the maximum 
	// displacement is set to half of the cell's size, ...
	maxDisplacementOnTrail = min(0.5f*m_cellWidth, min(0.5f*m_cellHeight, 0.5f*m_cellDepth));

	orgValue = m_image->GetValue(orgCoord[0], orgCoord[1], orgCoord[2]);
	destValue = m_image->GetValue(destCoord[0], destCoord[1], destCoord[2]);

	epsilonValue = 1.0f/powf(2.0f, BISECTION_STEPCOUNT_DEFAULT);

	orgValueTranslated = orgValue - m_isoLevel;
	destValueTranslated = destValue - m_isoLevel;

	// Is it better to have an adaptive probe length?
	probeVectorLength = min(m_cellWidth, min(m_cellHeight, m_cellDepth))*epsilonValue;

	if (fabs(orgValueTranslated) > epsilonValue)
	{
		if (fabs(destValueTranslated) > epsilonValue)
		{
			normalAnswer = m_image->GetNormal(orgCoord[0], orgCoord[1], orgCoord[2]);

			for (i=0; i<3; i++)
				orgNormal[i] = normalAnswer[i];

			newOrgValueTranslated = orgValueTranslated;

			// How can I justify this (or any) number of steps?
			for (j=0; j<m_transformStepsMax; j++)
			{
				orgValueProbe = m_image->GetValue(orgCoord[0] + orgNormal[0]*probeVectorLength, orgCoord[1] + orgNormal[1]*probeVectorLength, orgCoord[2] + orgNormal[2]*probeVectorLength) - m_isoLevel;

				orgNormalSign = 1.0f;

				// Assures that the vertex is always moving away from the 
				// isosurface.
				if (newOrgValueTranslated > 0.0f)
				{
					if (newOrgValueTranslated > orgValueProbe)
						orgNormalSign = -1.0f;
				}
				else
				{
					if (newOrgValueTranslated < orgValueProbe)
						orgNormalSign = -1.0f;
				}

				for (i=0; i<3; i++)
					newOrgCoord[i] = orgCoord[i] + (maxDisplacementOnTrail/m_transformStepsMax)*(orgNormal[i]*orgNormalSign);

				newOrgValueTranslated = m_image->GetValue(newOrgCoord[0], newOrgCoord[1], newOrgCoord[2]) - m_isoLevel;

				// Check if we still got an active edge.
				if (newOrgValueTranslated*orgValueTranslated > 0.0f)
					for (i=0; i<3; i++)
						orgCoord[i] = newOrgCoord[i];
				else 
					break;
			}

			normalAnswer = m_image->GetNormal(destCoord[0], destCoord[1], destCoord[2]);

			for (i=0; i<3; i++)
				destNormal[i] = normalAnswer[i];

			newDestValueTranslated = destValueTranslated;

			// How can I justify this (or any) number of steps?
			for (j=0; j<m_transformStepsMax; j++)
			{
				destValueProbe = m_image->GetValue(destCoord[0] + destNormal[0]*probeVectorLength, destCoord[1] + destNormal[1]*probeVectorLength, destCoord[2] + destNormal[2]*probeVectorLength) - m_isoLevel;

				destNormalSign = 1.0f;

				// Assures that the vertex is always moving away from the 
				// isosurface.
				if (newDestValueTranslated > 0.0f)
				{
					if (newDestValueTranslated > destValueProbe)
						destNormalSign = -1.0f;
				}
				else
				{
					if (newDestValueTranslated < destValueProbe)
						destNormalSign = -1.0f;
				}

				for (i=0; i<3; i++)
					newDestCoord[i] = destCoord[i] + (maxDisplacementOnTrail/m_transformStepsMax)*(destNormal[i]*destNormalSign);
				
				newDestValueTranslated = m_image->GetValue(newDestCoord[0], newDestCoord[1], newDestCoord[2]) - m_isoLevel;

				// Check if we still got an active edge.
				if (newDestValueTranslated*destValueTranslated > 0.0f)
					for (i=0; i<3; i++)
						destCoord[i] = newDestCoord[i];
				else
					break;
			}
		}
		// Destination vertex lying close to the isosurface.
		else
		{
			normalAnswer = m_image->GetNormal(orgCoord[0], orgCoord[1], orgCoord[2]);

			for (i=0; i<3; i++)
				orgNormal[i] = normalAnswer[i];

			newOrgValueTranslated = orgValueTranslated;

			// How can I justify this (or any) number of steps?
			for (j=0; j<m_transformStepsMax; j++)
			{
				orgValueProbe = m_image->GetValue(orgCoord[0] + orgNormal[0]*probeVectorLength, orgCoord[1] + orgNormal[1]*probeVectorLength, orgCoord[2] + orgNormal[2]*probeVectorLength) - m_isoLevel;

				orgNormalSign = 1.0f;

				// Assures that the vertex is always moving away from the 
				// isosurface.
				if (newOrgValueTranslated > 0.0f)
				{
					if (newOrgValueTranslated > orgValueProbe)
						orgNormalSign = -1.0f;
				}
				else
				{
					if (newOrgValueTranslated < orgValueProbe)
						orgNormalSign = -1.0f;
				}

				for (i=0; i<3; i++)
					newOrgCoord[i] = orgCoord[i] + (maxDisplacementOnTrail/m_transformStepsMax)*(orgNormal[i]*orgNormalSign);

				newOrgValueTranslated = m_image->GetValue(newOrgCoord[0], newOrgCoord[1], newOrgCoord[2]) - m_isoLevel;

				for (i=0; i<3; i++)
					newDestCoord[i] = destCoord[i] - (maxDisplacementOnTrail/m_transformStepsMax)*(orgNormal[i]*orgNormalSign);

				newDestValueTranslated = m_image->GetValue(newDestCoord[0], newDestCoord[1], newDestCoord[2]) - m_isoLevel;

				// Check if we still got an active edge.
				if (newOrgValueTranslated*newDestValueTranslated < 0.0f)
				{
					for (i=0; i<3; i++)
					{
						orgCoord[i] = newOrgCoord[i];
						destCoord[i] = newDestCoord[i];
					}
				}
				else 
					break;
			}
		}
	}
	// Origin vertex lying close to the isosurface.
	else if (fabs(destValueTranslated) > epsilonValue)
	{
		normalAnswer = m_image->GetNormal(destCoord[0], destCoord[1], destCoord[2]);

		for (i=0; i<3; i++)
			destNormal[i] = normalAnswer[i];

		newDestValueTranslated = destValueTranslated;

		// How can I justify this (or any) number of steps?
		for (j=0; j<m_transformStepsMax; j++)
		{
			destValueProbe = m_image->GetValue(destCoord[0] + destNormal[0]*probeVectorLength, destCoord[1] + destNormal[1]*probeVectorLength, destCoord[2] + destNormal[2]*probeVectorLength) - m_isoLevel;

			destNormalSign = 1.0f;

			if (newDestValueTranslated > 0.0f)
			{
				if (newDestValueTranslated > destValueProbe)
					destNormalSign = -1.0f;
			}
			else
			{
				if (newDestValueTranslated < destValueProbe)
					destNormalSign = -1.0f;
			}

			for (i=0; i<3; i++)
				newDestCoord[i] = destCoord[i] + (maxDisplacementOnTrail/m_transformStepsMax)*(destNormal[i]*destNormalSign);

			newDestValueTranslated = m_image->GetValue(newDestCoord[0], newDestCoord[1], newDestCoord[2]) - m_isoLevel;

			for (i=0; i<3; i++)
				newOrgCoord[i] = orgCoord[i] - (maxDisplacementOnTrail/m_transformStepsMax)*(destNormal[i]*destNormalSign);

			newOrgValueTranslated = m_image->GetValue(newOrgCoord[0], newOrgCoord[1], newOrgCoord[2]) - m_isoLevel;

			// Check if we still got an active edge.
			if (newOrgValueTranslated*newDestValueTranslated < 0.0f)
			{
				for (i=0; i<3; i++)
				{
					orgCoord[i] = newOrgCoord[i];
					destCoord[i] = newDestCoord[i];
				}
			}
			else 
				break;
		}
	}
}

/**
*/
void IMarchingMethod::ApplyTangentialTransform(float *orgCoord, float *destCoord)// const
{
#if 0 // new
	float // Step length in the gradient descent.
		maxDisplacementOnTrail,
		stepsizeOnTrail,
		edgeAxis[3],
		orgValue,
		destValue,
		epsilonValue,
		orgValueDifferenceToIsolevel,
		destValueDifferenceToIsolevel,
		trailBinormal[3],
		trail[3],
		newOrgCoord[3],
		newOrgValueDifferenceToIsolevel,
		newDestCoord[3],
		newDestValueDifferenceToIsolevel;
	int i,
		j;
	const float *normalPointerFromImageAdaptor;
	// Thesis only!
	//int tangentialTransformFailureCountOld;

	// Thesis only!
	//tangentialTransformFailureCountOld = m_tangentialTransformFailureCount;

	maxDisplacementOnTrail = 0.5f;

	for (i=0; i<3; i++)
		edgeAxis[i] = destCoord[i] - orgCoord[i];

	destValue = m_image->GetValue(destCoord[0], destCoord[1], destCoord[2]);
	// The query for 'orgValue' comes after the one for 'destValue' to set up 
	// the Gage state for the next query of 'normalPointerFromImageAdaptor'.
	orgValue = m_image->GetValue(orgCoord[0], orgCoord[1], orgCoord[2]);

	epsilonValue = 1.0f/powf(2.0f, BISECTION_STEPCOUNT_DEFAULT);

	orgValueDifferenceToIsolevel = orgValue - m_isoLevel;
	destValueDifferenceToIsolevel = destValue - m_isoLevel;

	stepsizeOnTrail = maxDisplacementOnTrail/m_transformStepsMax;

	// Thesis only!
	//if ((fabs(orgValueDifferenceToIsolevel) > epsilonValue) && (fabs(destValueDifferenceToIsolevel) > epsilonValue))
	{
		newOrgCoord[0] = orgCoord[0];
		newOrgCoord[1] = orgCoord[1];
		newOrgCoord[2] = orgCoord[2];

		/*normalPointerFromImageAdaptor = m_image->GetNormal();
		trailBinormal[0] = normalPointerFromImageAdaptor[1]*edgeAxis[2] - normalPointerFromImageAdaptor[2]*edgeAxis[1];
		trailBinormal[1] = normalPointerFromImageAdaptor[2]*edgeAxis[0] - normalPointerFromImageAdaptor[0]*edgeAxis[2];
		trailBinormal[2] = normalPointerFromImageAdaptor[0]*edgeAxis[1] - normalPointerFromImageAdaptor[1]*edgeAxis[0];*/

		// How can I justify this (or any) number of steps?
		for (j=0; j<m_transformStepsMax; j++)
		{
			normalPointerFromImageAdaptor = m_image->GetNormal();

			trailBinormal[0] = normalPointerFromImageAdaptor[1]*edgeAxis[2] - normalPointerFromImageAdaptor[2]*edgeAxis[1];
			trailBinormal[1] = normalPointerFromImageAdaptor[2]*edgeAxis[0] - normalPointerFromImageAdaptor[0]*edgeAxis[2];
			trailBinormal[2] = normalPointerFromImageAdaptor[0]*edgeAxis[1] - normalPointerFromImageAdaptor[1]*edgeAxis[0];

			trail[0] = trailBinormal[1]*normalPointerFromImageAdaptor[2] - trailBinormal[2]*normalPointerFromImageAdaptor[1];
			trail[1] = trailBinormal[2]*normalPointerFromImageAdaptor[0] - trailBinormal[0]*normalPointerFromImageAdaptor[2];
			trail[2] = trailBinormal[0]*normalPointerFromImageAdaptor[1] - trailBinormal[1]*normalPointerFromImageAdaptor[0];

			newOrgCoord[0] += stepsizeOnTrail*trail[0];
			newOrgCoord[1] += stepsizeOnTrail*trail[1];
			newOrgCoord[2] += stepsizeOnTrail*trail[2];

			newOrgValueDifferenceToIsolevel = m_image->GetValue(newOrgCoord[0], newOrgCoord[1], newOrgCoord[2]) - m_isoLevel;

			// Check if we still got an active edge.
			if (newOrgValueDifferenceToIsolevel*orgValueDifferenceToIsolevel <= 0.0f)
			{
				// Thesis only!
				//m_tangentialTransformFailureCount++;

				// Rollback.
				newOrgCoord[0] -= stepsizeOnTrail*trail[0];
				newOrgCoord[1] -= stepsizeOnTrail*trail[1];
				newOrgCoord[2] -= stepsizeOnTrail*trail[2];

				break;
			}
		}

		orgCoord[0] = newOrgCoord[0];
		orgCoord[1] = newOrgCoord[1];
		orgCoord[2] = newOrgCoord[2];

		for (i=0; i<3; i++)
			edgeAxis[i] = -edgeAxis[i];

		// Its here just to set up the Gage cache.
		normalPointerFromImageAdaptor = m_image->GetNormal(destCoord[0], destCoord[1], destCoord[2]);

		newDestCoord[0] = destCoord[0];
		newDestCoord[1] = destCoord[1];
		newDestCoord[2] = destCoord[2];

		/*trailBinormal[0] = normalPointerFromImageAdaptor[1]*edgeAxis[2] - normalPointerFromImageAdaptor[2]*edgeAxis[1];
		trailBinormal[1] = normalPointerFromImageAdaptor[2]*edgeAxis[0] - normalPointerFromImageAdaptor[0]*edgeAxis[2];
		trailBinormal[2] = normalPointerFromImageAdaptor[0]*edgeAxis[1] - normalPointerFromImageAdaptor[1]*edgeAxis[0];*/

		// How can I justify this (or any) number of steps?
		for (j=0; j<m_transformStepsMax; j++)
		{
			normalPointerFromImageAdaptor = m_image->GetNormal();

			trailBinormal[0] = normalPointerFromImageAdaptor[1]*edgeAxis[2] - normalPointerFromImageAdaptor[2]*edgeAxis[1];
			trailBinormal[1] = normalPointerFromImageAdaptor[2]*edgeAxis[0] - normalPointerFromImageAdaptor[0]*edgeAxis[2];
			trailBinormal[2] = normalPointerFromImageAdaptor[0]*edgeAxis[1] - normalPointerFromImageAdaptor[1]*edgeAxis[0];

			trail[0] = trailBinormal[1]*normalPointerFromImageAdaptor[2] - trailBinormal[2]*normalPointerFromImageAdaptor[1];
			trail[1] = trailBinormal[2]*normalPointerFromImageAdaptor[0] - trailBinormal[0]*normalPointerFromImageAdaptor[2];
			trail[2] = trailBinormal[0]*normalPointerFromImageAdaptor[1] - trailBinormal[1]*normalPointerFromImageAdaptor[0];

			newDestCoord[0] += stepsizeOnTrail*trail[0];
			newDestCoord[1] += stepsizeOnTrail*trail[1];
			newDestCoord[2] += stepsizeOnTrail*trail[2];

			newDestValueDifferenceToIsolevel = m_image->GetValue(newDestCoord[0], newDestCoord[1], newDestCoord[2]) - m_isoLevel;

			// Check if we still got an active edge.
			if (newDestValueDifferenceToIsolevel*destValueDifferenceToIsolevel <= 0.0f)
			{
				// Thesis only!
				//m_tangentialTransformFailureCount++;

				// Rollback.
				newDestCoord[0] -= stepsizeOnTrail*trail[0];
				newDestCoord[1] -= stepsizeOnTrail*trail[1];
				newDestCoord[2] -= stepsizeOnTrail*trail[2];

				break;
			}
		}

		destCoord[0] = newDestCoord[0];
		destCoord[1] = newDestCoord[1];
		destCoord[2] = newDestCoord[2];
	}

	// Thesis only!
	//if (tangentialTransformFailureCountOld == m_tangentialTransformFailureCount)
		//m_triangleMesh->AddColor(1.0f, 1.0f, 1.0f);
	//else
		//m_triangleMesh->AddColor(1.0f, 0.0f, 0.0f);

#else // old
	float // Step length in the gradient descent.
		maxDisplacementOnTrail,
		edgeAxis[3],
		orgValue,
		destValue,
		epsilonValue,
		orgValueTranslated,
		destValueTranslated,
		orgNormal[3],
		trailBinormal[3],
		trail[3],
		newOrgCoord[3],
		newOrgValueTranslated,
		destNormal[3],
		newDestCoord[3],
		newDestValueTranslated;
	int i,
		j;
	const float *normalAnswer;

	maxDisplacementOnTrail = 0.5f;

	for (i=0; i<3; i++)
		edgeAxis[i] = destCoord[i] - orgCoord[i];

	orgValue = m_image->GetValue(orgCoord[0], orgCoord[1], orgCoord[2]);
	destValue = m_image->GetValue(destCoord[0], destCoord[1], destCoord[2]);

	epsilonValue = 1.0f/powf(2.0f, BISECTION_STEPCOUNT_DEFAULT);

	orgValueTranslated = orgValue - m_isoLevel;
	destValueTranslated = destValue - m_isoLevel;

	if ((fabs(orgValueTranslated) > epsilonValue) && (fabs(destValueTranslated) > epsilonValue))
	{
		// How can I justify this (or any) number of steps?
		for (j=0; j<m_transformStepsMax; j++)
		{
			normalAnswer = m_image->GetNormal(orgCoord[0], orgCoord[1], orgCoord[2]);

			for (i=0; i<3; i++)
				orgNormal[i] = normalAnswer[i];

			trailBinormal[0] = orgNormal[1]*edgeAxis[2] - orgNormal[2]*edgeAxis[1];
			trailBinormal[1] = orgNormal[2]*edgeAxis[0] - orgNormal[0]*edgeAxis[2];
			trailBinormal[2] = orgNormal[0]*edgeAxis[1] - orgNormal[1]*edgeAxis[0];

			trail[0] = trailBinormal[1]*orgNormal[2] - trailBinormal[2]*orgNormal[1];
			trail[1] = trailBinormal[2]*orgNormal[0] - trailBinormal[0]*orgNormal[2];
			trail[2] = trailBinormal[0]*orgNormal[1] - trailBinormal[1]*orgNormal[0];

			for (i=0; i<3; i++)
				newOrgCoord[i] = orgCoord[i] + (maxDisplacementOnTrail/m_transformStepsMax)*trail[i];

			newOrgValueTranslated = m_image->GetValue(newOrgCoord[0], newOrgCoord[1], newOrgCoord[2]) - m_isoLevel;

			// Check if we still got an active edge.
			if (newOrgValueTranslated*orgValueTranslated > 0.0f)
				for (i=0; i<3; i++)
					orgCoord[i] = newOrgCoord[i];
			else
				break;
		}

		for (i=0; i<3; i++)
			edgeAxis[i] = -edgeAxis[i];

		// How can I justify this (or any) number of steps?
		for (j=0; j<m_transformStepsMax; j++)
		{
			normalAnswer = m_image->GetNormal(destCoord[0], destCoord[1], destCoord[2]);

			for (i=0; i<3; i++)
				destNormal[i] = normalAnswer[i];

			trailBinormal[0] = destNormal[1]*edgeAxis[2] - destNormal[2]*edgeAxis[1];
			trailBinormal[1] = destNormal[2]*edgeAxis[0] - destNormal[0]*edgeAxis[2];
			trailBinormal[2] = destNormal[0]*edgeAxis[1] - destNormal[1]*edgeAxis[0];

			trail[0] = trailBinormal[1]*destNormal[2] - trailBinormal[2]*destNormal[1];
			trail[1] = trailBinormal[2]*destNormal[0] - trailBinormal[0]*destNormal[2];
			trail[2] = trailBinormal[0]*destNormal[1] - trailBinormal[1]*destNormal[0];

			for (i=0; i<3; i++)
				newDestCoord[i] = destCoord[i] + (maxDisplacementOnTrail/m_transformStepsMax)*trail[i];

			newDestValueTranslated = m_image->GetValue(newDestCoord[0], newDestCoord[1], newDestCoord[2]) - m_isoLevel;

			// Check if we still got an active edge.
			if (newDestValueTranslated*destValueTranslated > 0.0f)
				for (i=0; i<3; i++)
					destCoord[i] = newDestCoord[i];
			else
				break;
		}
	}
#endif 
}

/**
*/
void IMarchingMethod::ApplyProjectionTransform(float *orgCoord, float *destCoord) const
{
	// Minimum difference allowed in the function value comparisons.
	float epsilonValue,
		// Coordinates of the edge middle point.
		midCoord[3],
		// Value of the function at the edge middle point.
		midValue,
		// Value of the function at the edge origin.
		orgValue,
		// Value of the function at the edge destination.
		destValue,
		// Normalized gradient of the function at the edge destination.
		destNormal[3],
		minCellEdgeSize,
		// Maximum displacement of the grid vertices along the transformation 
		// path. Here, it is used only in the gradient transform.
		maxDisplacementOnTrail,
		stepOnTrail,
		// Minimum vector size.
		epsilonVectorSize,
		destValueProbe,
		destNormalSign,
		newDestCoord[3],
		newDestValue,
		// Tangential transform parameters.
		trailBinormal[3],
		trail[3],
		edge[3];
	bool flipEdge;
	const float *pointerToNormalQueryResult;
	int i,
		j;

	// The minimum difference allowed in the value comparisons is linked to 
	// the error allowed in the following bisection procedure.
	epsilonValue = 1.0f/powf(2.0f, BISECTION_STEPCOUNT_DEFAULT);

	// Calculates the middle point of the edge.
	for (i=0; i<3; ++i)
		midCoord[i] = 0.5f*(orgCoord[i] + destCoord[i]);

	midValue = m_image->GetValue(midCoord[0], midCoord[1], midCoord[2]) - m_isoLevel;

	// If the middle point is placed on the isosurface, the result of the 
	// projection is the middlepoint itself, and edge is collapsed to guarantee 
	// that the cut vertex will be generated in the middle point of the edge.
	if (fabs(midValue) <= epsilonValue)
	{
		copy(midCoord, midCoord + 3, orgCoord);
		copy(midCoord, midCoord + 3, destCoord);
	}
	else
	{
		flipEdge = false;

		orgValue = m_image->GetValue(orgCoord[0], orgCoord[1], orgCoord[2]) - m_isoLevel;

		// Tests if the edge should be flipped. The edge will be flipped if the 
		// cut point lies between its origin and middle point.
		if (orgValue < 0.0f)
		{
			if (midValue >= 0.0f)
				flipEdge = true;
		}
		else
		{
			if (midValue < 0.0f)
				flipEdge = true;
		}

		if (flipEdge)
			copy(orgCoord, orgCoord + 3, destCoord);

		destValue = m_image->GetValue(destCoord[0], destCoord[1], destCoord[2]) - m_isoLevel;

		// Only works if the Gage cache retains a query to a 'destCoord' point.
		pointerToNormalQueryResult = m_image->GetNormal();

		copy(pointerToNormalQueryResult, pointerToNormalQueryResult + 3, destNormal);

		copy(midCoord, midCoord + 3, orgCoord);

		newDestValue = destValue;

		minCellEdgeSize = min(m_cellWidth, min(m_cellHeight, m_cellDepth));

		epsilonVectorSize = epsilonValue*minCellEdgeSize;

		maxDisplacementOnTrail = 0.5f*minCellEdgeSize;

		stepOnTrail = maxDisplacementOnTrail/m_transformStepsMax;

		// Half of the gradient transform applied to the destination grid 
		// vertex.
		for (j=0; j<m_transformStepsMax/2; ++j)
		{
			destValueProbe = m_image->GetValue(destCoord[0] + destNormal[0]*epsilonVectorSize, destCoord[1] + destNormal[1]*epsilonVectorSize, destCoord[2] + destNormal[2]*epsilonVectorSize) - m_isoLevel;

			destNormalSign = 1.0f;

			// Assures that the vertex is always moving away from isosurface.
			if (newDestValue > 0.0f)
			{
				if (newDestValue > destValueProbe)
					destNormalSign = -1.0f;
			}
			else
				if (newDestValue < destValueProbe)
					destNormalSign = -1.0f;

			for (i=0; i<3; ++i)
				newDestCoord[i] = destCoord[i] + stepOnTrail*(destNormal[i]*destNormalSign);
			
			newDestValue = m_image->GetValue(newDestCoord[0], newDestCoord[1], newDestCoord[2]) - m_isoLevel;

			// Check if we still got an active edge.
			if (newDestValue*destValue > 0.0f)
				copy(newDestCoord, newDestCoord + 3, destCoord);
			else
				break;
		}

		stepOnTrail = 0.5f/m_transformStepsMax;

		for (i=0; i<3; ++i)
			edge[i] = orgCoord[i] - destCoord[i];

		// Just a hack to set up the Gage cache.
		pointerToNormalQueryResult = m_image->GetNormal(destCoord[0], destCoord[1], destCoord[2]);

		// Half of the tangential transform applied to the destination grid 
		// vertex.
		for (j=0; j<m_transformStepsMax/2; ++j)
		{
			// Only works if the Gage cache retains a query to a 'destCoord' point.
			pointerToNormalQueryResult = m_image->GetNormal();

			trailBinormal[0] = pointerToNormalQueryResult[1]*edge[2] - pointerToNormalQueryResult[2]*edge[1];
			trailBinormal[1] = pointerToNormalQueryResult[2]*edge[0] - pointerToNormalQueryResult[0]*edge[2];
			trailBinormal[2] = pointerToNormalQueryResult[0]*edge[1] - pointerToNormalQueryResult[1]*edge[0];

			trail[0] = trailBinormal[1]*pointerToNormalQueryResult[2] - trailBinormal[2]*pointerToNormalQueryResult[1];
			trail[1] = trailBinormal[2]*pointerToNormalQueryResult[0] - trailBinormal[0]*pointerToNormalQueryResult[2];
			trail[2] = trailBinormal[0]*pointerToNormalQueryResult[1] - trailBinormal[1]*pointerToNormalQueryResult[0];

			for (i=0; i<3; ++i)
				newDestCoord[i] = destCoord[i] + stepOnTrail*trail[i];

			newDestValue = m_image->GetValue(newDestCoord[0], newDestCoord[1], newDestCoord[2]) - m_isoLevel;

			// Check if we still got an active edge.
			if (newDestValue*destValue > 0.0f)
				copy(newDestCoord, newDestCoord + 3, destCoord);
			else
				break;
		}
	}
}

/**
*/
int IMarchingMethod::FindIntersectionByLinearInterpolation(float *orgCoord, float *destCoord)
{
	float orgValue,
		destValue,
		mu,
		vertexCoord[3],
		vertexValue;
	int vertexIndex,
		i;
	const float *vertexNormal;

	orgValue = m_image->GetValue(orgCoord[0], orgCoord[1], orgCoord[2]) - m_isoLevel;
	destValue = m_image->GetValue(destCoord[0], destCoord[1], destCoord[2]) - m_isoLevel;

	mu = fabs(orgValue)/(fabs(destValue) + fabs(orgValue));

#ifdef MY_USING_DISPLACEMENT_TRANSFORM
	mu = MyMath::Gain<float>(0.3f, mu);
#endif // #ifdef MY_USING_DISPLACEMENT_TRANSFORM

	if (mu < 0.0f)
		mu = 0.0f;
	else if (mu > 1.0f)
		mu = 1.0f;

	for (i=0; i<3; i++)
		vertexCoord[i] = orgCoord[i] + mu*(destCoord[i] - orgCoord[i]);

	vertexValue = m_image->GetValue(vertexCoord[0], vertexCoord[1], vertexCoord[2]) - m_isoLevel;

	m_algebraicError = max(m_algebraicError, fabs(vertexValue));

	vertexIndex = (int)m_triangleMesh->m_vertexArray.size()/3;

	m_triangleMesh->m_vertexArray.push_back(vertexCoord[0]);
	m_triangleMesh->m_vertexArray.push_back(vertexCoord[1]);
	m_triangleMesh->m_vertexArray.push_back(vertexCoord[2]);

	vertexNormal = m_image->GetNormal();

	m_triangleMesh->m_normalArray.push_back(-vertexNormal[0]);
	m_triangleMesh->m_normalArray.push_back(-vertexNormal[1]);
	m_triangleMesh->m_normalArray.push_back(-vertexNormal[2]);

	/*// BEGIN OF TEST (store the active edges).
	if (orgValue < destValue)
	{
		m_activeEdges->m_vertexArray.push_back((float)orgCoord[0]);
		m_activeEdges->m_vertexArray.push_back((float)orgCoord[1]);
		m_activeEdges->m_vertexArray.push_back((float)orgCoord[2]);
	
		m_activeEdges->m_vertexArray.push_back((float)destCoord[0]);
		m_activeEdges->m_vertexArray.push_back((float)destCoord[1]);
		m_activeEdges->m_vertexArray.push_back((float)destCoord[2]);
	}
	else
	{
		m_activeEdges->m_vertexArray.push_back((float)destCoord[0]);
		m_activeEdges->m_vertexArray.push_back((float)destCoord[1]);
		m_activeEdges->m_vertexArray.push_back((float)destCoord[2]);

		m_activeEdges->m_vertexArray.push_back((float)orgCoord[0]);
		m_activeEdges->m_vertexArray.push_back((float)orgCoord[1]);
		m_activeEdges->m_vertexArray.push_back((float)orgCoord[2]);
	}

	m_activeEdges->m_elementArray.push_back((unsigned int)m_activeEdges->m_elementArray.size());
	m_activeEdges->m_elementArray.push_back((unsigned int)m_activeEdges->m_elementArray.size());
	// END OF TEST (store the active edges).*/

	return vertexIndex;
}

/**
*/
int IMarchingMethod::FindIntersectionByBisection(float *orgCoord, float *destCoord)
{
	float orgValue,
		destValue,
		mu,
		muStepsize,
		epsilonValue,
		vertexValue,
		vertexCoord[3];
	int vertexIndex,
		i,
		j;
	const float *vertexNormal;

	orgValue = m_image->GetValue(orgCoord[0], orgCoord[1], orgCoord[2]) - m_isoLevel;
	destValue = m_image->GetValue(destCoord[0], destCoord[1], destCoord[2]) - m_isoLevel;

	epsilonValue = 1.0f/powf(2.0f, BISECTION_STEPCOUNT_DEFAULT);

	mu = 0.5f;

	muStepsize = 0.25f;
	
	for (i=0; i<BISECTION_STEPCOUNT_DEFAULT; i++)
	{
		for (j=0; j<3; j++)
			vertexCoord[j] = orgCoord[j] + mu*(destCoord[j] - orgCoord[j]);

		vertexValue = m_image->GetValue(vertexCoord[0], vertexCoord[1], vertexCoord[2]) - m_isoLevel;

		if (fabs(vertexValue) < epsilonValue)
			break;

		if (orgValue < 0.0f)
		{
			if (vertexValue < 0.0f)
				mu += muStepsize;
			else
				mu -= muStepsize;
		}
		else
		{
			if (vertexValue < 0.0f)
				mu -= muStepsize;
			else
				mu += muStepsize;
		}

		muStepsize *= 0.5f;
	}

#ifdef MY_USING_DISPLACEMENT_TRANSFORM
	/*mu = MyMath::Gain<float>(0.3f, mu);

	if (mu < 0.0f)
		mu = 0.0f;
	else if (mu > 1.0f)
		mu = 1.0f;*/

	// Thesis only!
	/*mu = MyMath::Bias<float>(0.275f, mu);

	if (mu < 0.0f)
		mu = 0.0f;
	else if (mu > 1.0f)
		mu = 1.0f;*/
	mu = mu*(1.0f - 0.6f*mu);
#endif // #ifdef MY_USING_DISPLACEMENT_TRANSFORM

	m_algebraicError = max(m_algebraicError, fabs(vertexValue));

	for (i=0; i<3; i++)
		vertexCoord[i] = orgCoord[i] + mu*(destCoord[i] - orgCoord[i]);

	vertexIndex = (int)m_triangleMesh->m_vertexArray.size()/3;

	m_triangleMesh->m_vertexArray.insert(m_triangleMesh->m_vertexArray.end(), vertexCoord, vertexCoord + 3);

	// Assume the approximated normal of last 'mu' bisection step.
	vertexNormal = m_image->GetNormal();

	m_triangleMesh->m_normalArray.push_back(-vertexNormal[0]);
	m_triangleMesh->m_normalArray.push_back(-vertexNormal[1]);
	m_triangleMesh->m_normalArray.push_back(-vertexNormal[2]);

	/*// BEGIN OF TEST (store the active edges).
	if (orgValue < destValue)
	{
		m_activeEdges->m_vertexArray.push_back((float)orgCoord[0]);
		m_activeEdges->m_vertexArray.push_back((float)orgCoord[1]);
		m_activeEdges->m_vertexArray.push_back((float)orgCoord[2]);
	
		m_activeEdges->m_vertexArray.push_back((float)destCoord[0]);
		m_activeEdges->m_vertexArray.push_back((float)destCoord[1]);
		m_activeEdges->m_vertexArray.push_back((float)destCoord[2]);
	}
	else
	{
		m_activeEdges->m_vertexArray.push_back((float)destCoord[0]);
		m_activeEdges->m_vertexArray.push_back((float)destCoord[1]);
		m_activeEdges->m_vertexArray.push_back((float)destCoord[2]);

		m_activeEdges->m_vertexArray.push_back((float)orgCoord[0]);
		m_activeEdges->m_vertexArray.push_back((float)orgCoord[1]);
		m_activeEdges->m_vertexArray.push_back((float)orgCoord[2]);
	}

	m_activeEdges->m_elementArray.push_back((unsigned int)m_activeEdges->m_elementArray.size());
	m_activeEdges->m_elementArray.push_back((unsigned int)m_activeEdges->m_elementArray.size());
	// END OF TEST (store the active edges).*/

	return vertexIndex;
}

/**
*/
int IMarchingMethod::FindIntersection(float *orgCoord, float *destCoord)
{
	//return FindIntersectionByLinearInterpolation(orgCoord, destCoord);
	return FindIntersectionByBisection(orgCoord, destCoord);
}

/**
*/
float IMarchingMethod::GetIsoLevel(void) const
{
	return m_isoLevel;
}

/**
*/
boost::shared_ptr<CVertexPool> IMarchingMethod::GetMesh(void) const
{
	return m_triangleMesh;
}

/**
*/
boost::shared_ptr<CVertexPool> IMarchingMethod::GetActiveEdges(void) const
{
	return m_activeEdges;
}

/**
*/
float IMarchingMethod::GetAlgebraicError(void) const
{
	return m_algebraicError;
}

/**
*/
bool IMarchingMethod::SetTransformationStepCount(int stepCount)
{
	if (stepCount <= 0)
	{
		MarkError();

		return false;
	}

	m_transformStepsMax = stepCount;

	return true;
}

/**
*/
bool IMarchingMethod::IsValid(void) const
{
	return !m_triangleMesh->m_vertexArray.empty();
}

/**
*/
bool IMarchingMethod::GenerateMeshFromSpanSpace(float isoLevel)
{
	return false;
}

/**
*/
void IMarchingMethod::DeleteMesh(void)
{
	m_triangleMesh->Clear();
	m_activeEdges->Clear();

	m_algebraicError = 0.0f;

	// Thesis only!
	//m_tangentialTransformFailureCount = 0;
	//m_gradientTransformFailureCount = 0;
}

/**
*/
void IMarchingMethod::Create(void)
{
	m_image.reset();

	m_cellWidth = 1.0f;
	m_cellHeight = 1.0f;
	m_cellDepth = 1.0f;

	m_gridWidth = 0;
	m_gridHeight = 0;
	m_gridDepth = 0;

	m_triangleMesh.reset(new CVertexPool);

	if (!m_triangleMesh.get())
		MarkError();

	m_activeEdges.reset(new CVertexPool);

	if (!m_activeEdges.get())
		MarkError();

	m_bisectionStepsMax = BISECTION_STEPCOUNT_DEFAULT;

	m_edgeTransform = NONE;

	m_transformStepsMax = TRANSFORMATION_STEPCOUNT_DEFAULT;

	m_isoLevel = 0.0f;

	m_algebraicError = 0.0f;

	m_isUsingSpanSpace = false;

	m_spanSpaceWidth = 0;
	m_spanSpaceHeight = 0;
	
	m_imageValueMin = 0;
	m_imageValueMax = 0;
	
	m_spanSpace.clear();
}

