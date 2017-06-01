#define MY_LEAN_AND_MEAN_GAGEADAPTOR

#include <cmath>

#include <ErrorManager.h>
#include <GageAdaptor.h>

/**
*/
CGageAdaptor::CGageAdaptor(void)
{
	Create();
}

/**
*/
CGageAdaptor::CGageAdaptor(const std::string& path)
{
	Create();

	Open(path);
}

/**
*/
CGageAdaptor::~CGageAdaptor(void)
{
	if (IsOpen())
		Close();
}

/**
*/
bool CGageAdaptor::Open(const std::string& path)
{
	if (IsOpen())
		Close();

	if (!OpenNrrd(path))
	{
		MarkError();

		return false;
	}

	if (!CreateDefaultContext())
	{
		MarkError();

		return false;
	}

	m_isOpen = true;

	m_isCopy = false;

	return true;
}

/**
*/
bool CGageAdaptor::OpenFromMemory(void *data, VALUE_TYPE type, unsigned int width, unsigned int height, unsigned int depth)
{
	if (IsOpen())
		Close();

	if (!OpenNrrdFromMemory(data, type, width, height, depth))
	{
		MarkError();

		return false;
	}

	if (!CreateDefaultContext())
	{
		MarkError();

		return false;
	}

	m_isOpen = true;

	m_isCopy = false;

	return true;
}

/**
*/
void CGageAdaptor::Close(void)
{
	if (m_imageInfo && !m_isCopy) 
	{ 
		if (gagePerVolumeDetach(m_measurementContext, m_imageInfo))
			ReportError(biffGetDone(NRRD));

		free(gagePerVolumeNix(m_imageInfo));

		m_imageInfo = 0;
	}

	if (m_measurementContext)
	{
		free(gageContextNix(m_measurementContext));

		m_measurementContext = 0;
	}

	if (m_imageHandle)
	{
		nrrdNuke(m_imageHandle);
		
		m_imageHandle = 0;
	}

	m_isOpen = false;

	m_isCopy = false;
}

/**
*/
bool CGageAdaptor::IsOpen(void) const
{
	return m_isOpen;
}

/**
*/
bool CGageAdaptor::EnableQuery(int item)
{
	if (m_isCopy)
	{
		MarkError();

		return false;
	}

	if (!m_measurementContext || !m_imageInfo)
	{
		MarkError();

		return false;
	}

	if (gageQueryItemOn(m_measurementContext, m_imageInfo, item))
	{
		MarkError();

		return false;
	}

	switch (item) {
		case gageSclValue:
			if (!(m_valuePointer = gageAnswerPointer(m_measurementContext, m_imageInfo, gageSclValue)))
			{
				MarkError();
				
				return false;
			}
			break;
		case gageSclNormal:
			if (!(m_normalPointer = gageAnswerPointer(m_measurementContext, m_imageInfo, gageSclNormal)))
			{
				MarkError();

				return false;
			}
			break;
		case gageSclGradVec:
			if (!(m_gradientPointer = gageAnswerPointer(m_measurementContext, m_imageInfo, gageSclGradVec)))
			{
				MarkError();

				return false;
			}
			break;
		case gageSclGradMag:
			if (!(m_gradientMagnitudePointer = gageAnswerPointer(m_measurementContext, m_imageInfo, gageSclGradMag)))
			{
				MarkError();

				return false;
			}
			break;
		case gageSclHessian:
			if (!(m_hessianPointer = gageAnswerPointer(m_measurementContext, m_imageInfo, gageSclHessian)))
			{
				MarkError();

				return false;
			}
			break;
		case gageSclLaplacian:
			if (!(m_laplacianPointer = gageAnswerPointer(m_measurementContext, m_imageInfo, gageSclLaplacian)))
			{
				MarkError();

				return false;
			}
			break;
		case gageSclHessEval0:
			if (!(m_hessian1stEigenvaluePointer = gageAnswerPointer(m_measurementContext, m_imageInfo, gageSclHessEval0)))
			{
				MarkError();

				return false;
			}
			break;
		case gageSclHessEval1:
			if (!(m_hessian2ndEigenvaluePointer = gageAnswerPointer(m_measurementContext, m_imageInfo, gageSclHessEval1)))
			{
				MarkError();

				return false;
			}
			break;
		case gageSclHessEval2:
			if (!(m_hessian3rdEigenvaluePointer = gageAnswerPointer(m_measurementContext, m_imageInfo, gageSclHessEval2)))
			{
				MarkError();

				return false;
			}
			break;
		case gageSclK1:
			if (!(m_1stPrincipleCurvaturePointer = gageAnswerPointer(m_measurementContext, m_imageInfo, gageSclK1)))
			{
				MarkError();

				return false;
			}
			break;
		default:
			MarkError();
		
			return false;
	}

	if (!UpdateKernel())
	{
		MarkError();

		return false;
	}

	return true;
}

/**
*/
bool CGageAdaptor::ResetKernel(void)
{
	if (m_isCopy)
	{
		MarkError();

		return false;
	}

	if (m_measurementContext)
	{
		gageKernelReset(m_measurementContext);
		
		if (!UpdateKernel())
		{
			MarkError();

			return false;
		}
	}

	return true;
}

/**
This method is used to indicate that the kernel is intended to be used with 
clamping.
*/
void CGageAdaptor::SetClamp(bool doClamp)
{
	m_doClamp = doClamp;
}

/**
*/
bool CGageAdaptor::SetValueKernel(const NrrdKernel *type, double *parameters)
{
	if (!m_measurementContext)
	{
		MarkError();

		return false;
	}

	if (m_isCopy)
	{
		MarkError();

		return false;
	}

	if (gageKernelSet(m_measurementContext, gageKernel00, type, parameters))
	{
		ReportError(biffGetDone(NRRD));

		return false;
	}

	return UpdateKernel();
}

/**
*/
bool CGageAdaptor::Set1stDerivativeKernel(const NrrdKernel *type, double *parameters)
{
	if (!m_measurementContext)
	{
		MarkError();

		return false;
	}

	if (m_isCopy)
	{
		MarkError();

		return false;
	}

	if (gageKernelSet(m_measurementContext, gageKernel11, type, parameters))
	{
		ReportError(biffGetDone(NRRD));

		return false;
	}

	return UpdateKernel();
}

/**
*/
bool CGageAdaptor::Set2ndDerivativeKernel(const NrrdKernel *type, double *parameters)
{
	if (!m_measurementContext)
	{
		MarkError();

		return false;
	}

	if (m_isCopy)
	{
		MarkError();

		return false;
	}

	if (gageKernelSet(m_measurementContext, gageKernel22, type, parameters))
	{
		ReportError(biffGetDone(NRRD));

		return false;
	}

	return UpdateKernel();
}

/**
*/
int CGageAdaptor::GetWidth(void) const
{
	/*if (!m_imageHandle)
	{
		MarkError();
	
		return 0;
	}

	return (int)m_imageHandle->axis[0].size;*/
	if (!m_measurementContext)
	{
		MarkError();
	
		return 0;
	}

	return (int)m_measurementContext->shape->size[0];
}

/**
*/
int CGageAdaptor::GetHeight(void) const
{
	/*if (!m_imageHandle)
	{
		MarkError();
	
		return 0;
	}

	return (int)m_imageHandle->axis[1].size;*/
	if (!m_measurementContext)
	{
		MarkError();
	
		return 0;
	}

	return (int)m_measurementContext->shape->size[1];
}

/**
*/
int CGageAdaptor::GetDepth(void) const
{
	/*if (!m_imageHandle)
	{
		MarkError();
	
		return 0;
	}

	return (int)m_imageHandle->axis[2].size;*/
	if (!m_measurementContext)
	{
		MarkError();
	
		return 0;
	}

	return (int)m_measurementContext->shape->size[2];
}

/**
*/
CGageAdaptor::VALUE_TYPE CGageAdaptor::GetType(void) const
{
	switch (m_imageHandle->type) {
		case nrrdTypeChar:
			return BYTE;
			break;
		case nrrdTypeUChar:
			return UNSIGNED_BYTE;
			break;
		case nrrdTypeShort:
			return SHORT;
		case nrrdTypeUShort:
			return UNSIGNED_SHORT;
			break;
		case nrrdTypeInt:
			return INT;
			break;
		case nrrdTypeUInt:
			return UNSIGNED_INT;
			break;
		case nrrdTypeFloat:
			return FLOAT;
			break;
		case nrrdTypeDouble:
			return DOUBLE;
			break;
	}

	return UNKNOWN_TYPE;
}

/**
*/
const float *CGageAdaptor::GetValueArray(void) const
{
#ifndef MY_LEAN_AND_MEAN_GAGEADAPTOR
	if (!m_imageInfo)
	{
		MarkError();
	
		return 0;
	}
#endif // #ifndef MY_LEAN_AND_MEAN_GAGEADAPTOR

	return (float*)m_imageInfo->nin->data;
}

/**
*/
float CGageAdaptor::GetValue(float x, float y, float z) const
{
#ifndef MY_LEAN_AND_MEAN_GAGEADAPTOR
	if (!m_measurementContext || !m_valuePointer)
	{
		MarkError();
	
		return 0;
	}

	if (m_doClamp)
		Clamp(&x, &y, &z);
#endif // #ifndef MY_LEAN_AND_MEAN_GAGEADAPTOR

	if (gageProbe(m_measurementContext, x, y, z))
		ReportError(biffGetDone(NRRD));

	return *m_valuePointer;
}

/**
*/
const float *CGageAdaptor::GetNormal(float x, float y, float z) const
{
#ifndef MY_LEAN_AND_MEAN_GAGEADAPTOR
	if (!m_measurementContext || !m_normalPointer)
	{
		MarkError();
	
		return 0;
	}

	if (m_doClamp)
		Clamp(&x, &y, &z);
#endif // #ifndef MY_LEAN_AND_MEAN_GAGEADAPTOR

	if (gageProbe(m_measurementContext, x, y, z))
		ReportError(biffGetDone(NRRD));

	return m_normalPointer;
}

/**
*/
const float *CGageAdaptor::GetNormal(void) const
{
	return m_normalPointer;
}

/**
*/
const float *CGageAdaptor::GetGradient(float x, float y, float z) const
{
	if (!m_measurementContext || !m_gradientPointer)
	{
		MarkError();
	
		return 0;
	}

	if (m_doClamp)
		Clamp(&x, &y, &z);

	if (gageProbe(m_measurementContext, x, y, z))
		ReportError(biffGetDone(NRRD));

	return m_gradientPointer;
}

/**
*/
float CGageAdaptor::GetGradientMagnitude(float x, float y, float z) const
{
	if (!m_measurementContext || !m_gradientMagnitudePointer)
	{
		MarkError();
	
		return 0;
	}

	if (m_doClamp)
		Clamp(&x, &y, &z);

	if (gageProbe(m_measurementContext, x, y, z))
		ReportError(biffGetDone(NRRD));

	return *m_gradientMagnitudePointer;
}

/**
*/
const float *CGageAdaptor::GetHessian(float x, float y, float z) const
{
	if (!m_measurementContext || !m_hessianPointer)
	{
		MarkError();
	
		return 0;
	}

	if (m_doClamp)
		Clamp(&x, &y, &z);

	if (gageProbe(m_measurementContext, x, y, z))
		ReportError(biffGetDone(NRRD));

	return m_hessianPointer;
}

/**
*/
float CGageAdaptor::GetLaplacian(float x, float y, float z) const
{
	if (!m_measurementContext || !m_laplacianPointer)
	{
		MarkError();
	
		return 0;
	}

	if (m_doClamp)
		Clamp(&x, &y, &z);

	if (gageProbe(m_measurementContext, x, y, z))
		ReportError(biffGetDone(NRRD));

	return *m_laplacianPointer;
}

/**
*/
float CGageAdaptor::GetHessian1stEigenvalue(float x, float y, float z) const
{
	if (!m_measurementContext || !m_hessian1stEigenvaluePointer)
	{
		MarkError();
	
		return 0;
	}

	if (m_doClamp)
		Clamp(&x, &y, &z);

	if (gageProbe(m_measurementContext, x, y, z))
		ReportError(biffGetDone(NRRD));

	return *m_hessian1stEigenvaluePointer;
}

/**
*/
float CGageAdaptor::GetHessian2ndEigenvalue(float x, float y, float z) const
{
	if (!m_measurementContext || !m_hessian2ndEigenvaluePointer)
	{
		MarkError();
	
		return 0;
	}

	if (m_doClamp)
		Clamp(&x, &y, &z);

	if (gageProbe(m_measurementContext, x, y, z))
		ReportError(biffGetDone(NRRD));

	return *m_hessian2ndEigenvaluePointer;
}

/**
*/
float CGageAdaptor::GetHessian3rdEigenvalue(float x, float y, float z) const
{
	if (!m_measurementContext || !m_hessian3rdEigenvaluePointer)
	{
		MarkError();
	
		return 0;
	}

	if (m_doClamp)
		Clamp(&x, &y, &z);

	if (gageProbe(m_measurementContext, x, y, z))
		ReportError(biffGetDone(NRRD));

	return *m_hessian3rdEigenvaluePointer;
}

/**
*/
float CGageAdaptor::Get1stPrincipalCurvature(float x, float y, float z) const
{
	if (!m_measurementContext || !m_1stPrincipleCurvaturePointer)
	{
		MarkError();
	
		return 0;
	}

	if (m_doClamp)
		Clamp(&x, &y, &z);

	if (gageProbe(m_measurementContext, x, y, z))
		ReportError(biffGetDone(NRRD));

	return *m_1stPrincipleCurvaturePointer;
}

/**
*/
bool CGageAdaptor::OpenNrrd(const std::string& path)
{
	if (IsOpen())
	{
		MarkError();

		return false;
	}

	m_imageHandle = nrrdNew();

	nrrdStateDisableContent = AIR_TRUE;

	if (nrrdLoad(m_imageHandle, path.c_str(), NULL)) 
	{
		ReportError(biffGetDone(NRRD));

		Close();
		
		return false;
	}

	return true;
}

/**
*/
bool CGageAdaptor::OpenNrrdFromMemory(void *data, VALUE_TYPE type, unsigned int width, unsigned int height, unsigned int depth)
{
	if (IsOpen())
	{
		MarkError();

		return false;
	}

	m_imageHandle = nrrdNew();

	nrrdStateDisableContent = AIR_TRUE;

	if (nrrdAlloc_nva(m_imageHandle, type, 3, (const int *)&width))
	{
		ReportError(biffGetDone(NRRD));

		Close();
		
		return false;
	}

	switch (type) {
		case nrrdTypeFloat:
			memcpy(m_imageHandle->data, data, width*height*depth*sizeof(float));
			break;
		case nrrdTypeDouble:
			memcpy(m_imageHandle->data, data, width*height*depth*sizeof(double));
			break;
		default:
			MarkError();

			return false;
	};

	// Why do I have to do it? Gage cannot do it automatically?
	m_imageHandle->axis[0].spacing = 1.0;
	m_imageHandle->axis[1].spacing = 1.0;
	m_imageHandle->axis[2].spacing = 1.0;

	return true;
}

/**
*/
bool CGageAdaptor::CreateDefaultContext(void)
{
	double parameters[3];
	bool status;
	
	// Scale.
	parameters[0] = 1.0f;
	// Don't care.
	parameters[1] = 0.0f;
	// Don't care.
	parameters[2] = 0.0f;

	if (!m_imageHandle)
	{
		MarkError();

		return false;
	}

	if (!(m_measurementContext = gageContextNew()))
	{
		ReportError(biffGetDone(GAGE));

		return false;
	}

	if (!(m_imageInfo = gagePerVolumeNew(m_measurementContext, m_imageHandle, gageKindScl)))
	{
		ReportError(biffGetDone(GAGE));

		return false;
	}

	status = false;

	if (!gagePerVolumeAttach(m_measurementContext, m_imageInfo))
		status = true;

	// The tent function: f(-1)=0, f(0)=1, f(1)=0, with linear ramps in 
	// between, and zero elsewhere. Used for linear (and bilinear and 
	// trilinear) interpolation.
	if (status)
		status = SetValueKernel(nrrdKernelTent, parameters);

	// Piecewise-linear ramps that implement forward-difference 
	// differentiation.
	//if (status)
	//	status = Set1stDerivativeKernel(nrrdKernelForwDiff, parameters);

	if (status)
		status = EnableQuery(gageSclValue);

	//if (status)
	//	status = EnableQuery(gageSclNormal);
/*
	if (status)
		status = UpdateKernel();

	if (!status)
	{
		ReportError(biffGetDone(NRRD));

		Close();

		return false;
	}

	if (!(m_valuePointer = gageAnswerPointer(m_measurementContext, m_imageInfo, gageSclValue)))
	{
		MarkError();

		return false;
	}
*/
	//if (!(m_normalPointer = gageAnswerPointer(m_measurementContext, m_imageInfo, gageSclNormal)))
	//{
	//	MarkError();
	//
	//	return false;
	//}

	return true;
}

/**
*/
bool CGageAdaptor::UpdateKernel(void)
{
	if (!m_measurementContext)
	{
		MarkError();

		return false;
	}

	if (m_isCopy)
	{
		MarkError();

		return false;
	}

	if (gageUpdate(m_measurementContext))
	{
		ReportError(biffGetDone(NRRD));

		return false;
	}

	return true;
}

/**
*/
void CGageAdaptor::Clamp(float *x, float *y, float *z) const
{
	if (*x < FLT_EPSILON)
		*x = FLT_EPSILON;
	else if (*x > (m_measurementContext->shape->size[0] - 1.0f - FLT_EPSILON))
		*x = m_measurementContext->shape->size[0] - 1.0f - FLT_EPSILON;

	if (*y < FLT_EPSILON)
		*y = FLT_EPSILON;
	else if (*y > (m_measurementContext->shape->size[1] - 1.0f - FLT_EPSILON))
		*y = m_measurementContext->shape->size[1] - 1.0f - FLT_EPSILON;

	if (*z < FLT_EPSILON)
		*z = FLT_EPSILON;
	else if (*z > (m_measurementContext->shape->size[2] - 1.0f - FLT_EPSILON))
		*z = m_measurementContext->shape->size[2] - 1.0f - FLT_EPSILON;
}

/**
*/
void CGageAdaptor::Create(void)
{
	m_imageHandle = 0;

	m_measurementContext = 0;
	m_imageInfo = 0;
	
	m_valuePointer = 0;
	m_normalPointer = 0;

	m_gradientPointer = 0;
	m_gradientMagnitudePointer = 0;

	m_hessianPointer = 0;

	m_laplacianPointer = 0;

	m_hessian1stEigenvaluePointer = 0;
	m_hessian2ndEigenvaluePointer = 0;
	m_hessian3rdEigenvaluePointer = 0;

	m_1stPrincipleCurvaturePointer = 0;

	m_isOpen = false;

	m_isCopy = false;

	m_doClamp = false;
}

