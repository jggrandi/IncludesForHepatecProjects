#ifndef GAGEADAPTOR_INCLUDED
#define GAGEADAPTOR_INCLUDED

#include <string>

#include <teem/nrrd.h>
#include <teem/gage.h>

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

//#include "IMedicalImage.h"

class CGageAdaptor//::IMedicalImage
	: boost::noncopyable
{
public:
	enum QUERY_ITEM {
		// Data value.
		VALUE = gageSclValue,
		// Gradient vector, normalized.
		NORMAL = gageSclNormal,
		// Gradient vector, un-normalized.
		GRADIENT = gageSclGradVec,
		// Gradient magnitude.
		GRADIENT_MAGNITUDE = gageSclGradMag,
		// Hessian (column-order).
		HESSIAN = gageSclHessian,
		// Laplacian: Dxx + Dyy + Dzz.
		LAPLACIAN = gageSclLaplacian,
		// Hessian's 1st eigenvalue.
		HESSIAN_1ST_EIGENVALUE = gageSclHessEval0,
		// Hessian's 2nd eigenvalue.
		HESSIAN_2ND_EIGENVALUE = gageSclHessEval1,
		// Hessian's 3rd eigenvalue.
		HESSIAN_3RD_EIGENVALUE = gageSclHessEval2,
		// 1st principle curvature.
		PRINCIPAL_CURVATURE = gageSclK1
	};
	enum VALUE_TYPE {
		// Signifies "type is unset/unknown".
		UNKNOWN_TYPE = nrrdTypeUnknown,
		// Signed 1-byte integer.
		BYTE = nrrdTypeChar,
		// Unsigned 1-byte integer.
		UNSIGNED_BYTE = nrrdTypeUChar,
		// Signed 2-byte integer.
		SHORT = nrrdTypeShort,
		// Unsigned 2-byte integer.
		UNSIGNED_SHORT = nrrdTypeUShort,
		// Signed 4-byte integer.
		INT = nrrdTypeInt,
		// Unsigned 4-byte integer.
		UNSIGNED_INT = nrrdTypeUInt,
		// 4-byte floating point.
		FLOAT = nrrdTypeFloat,
		// 8-byte floating point.
		DOUBLE = nrrdTypeDouble
	};
	CGageAdaptor(void);
	CGageAdaptor(const std::string& path);
	virtual ~CGageAdaptor(void);
	virtual bool Open(const std::string& path);
	virtual bool OpenFromMemory(void *data, VALUE_TYPE type, unsigned int width, unsigned int height, unsigned int depth);
	virtual void Close(void);
	virtual bool IsOpen(void) const;
	virtual bool EnableQuery(int item);
	virtual bool ResetKernel(void);
	virtual void SetClamp(bool doClamp);
	virtual bool SetValueKernel(const NrrdKernel *type, double *parameters);
	virtual bool Set1stDerivativeKernel(const NrrdKernel *type, double *parameters);
	virtual bool Set2ndDerivativeKernel(const NrrdKernel *type, double *parameters);
	virtual int GetWidth(void) const;
	virtual int GetHeight(void) const;
	virtual int GetDepth(void) const;
	virtual VALUE_TYPE GetType(void) const;
	virtual const float *GetValueArray(void) const;
	virtual float GetValue(float x, float y, float z) const;
	virtual const float *GetNormal(float x, float y, float z) const;
	const float *GetNormal(void) const;
	virtual const float *GetGradient(float x, float y, float z) const;
	virtual float GetGradientMagnitude(float x, float y, float z) const;
	virtual const float *GetHessian(float x, float y, float z) const;
	virtual float GetLaplacian(float x, float y, float z) const;
	virtual float GetHessian1stEigenvalue(float x, float y, float z) const;
	virtual float GetHessian2ndEigenvalue(float x, float y, float z) const;
	virtual float GetHessian3rdEigenvalue(float x, float y, float z) const;
	virtual float Get1stPrincipalCurvature(float x, float y, float z) const;
private:
	bool OpenNrrd(const std::string& path);
	bool OpenNrrdFromMemory(void *data, VALUE_TYPE type, unsigned int width, unsigned int height, unsigned int depth);
	bool CreateDefaultContext(void);
	bool UpdateKernel(void);
	inline void Clamp(float *x, float *y, float *z) const;
protected:
	void Create(void);
protected:
	Nrrd *m_imageHandle;
	gageContext *m_measurementContext;
	gagePerVolume *m_imageInfo;
	const float *m_valuePointer;
	const float *m_normalPointer;
	const float *m_gradientPointer;
	const float *m_gradientMagnitudePointer;
	const float *m_hessianPointer;
	const float *m_laplacianPointer;
	const float *m_hessian1stEigenvaluePointer;
	const float *m_hessian2ndEigenvaluePointer;
	const float *m_hessian3rdEigenvaluePointer;
	const float *m_1stPrincipleCurvaturePointer;
	bool m_isOpen;
	bool m_isCopy;
	bool m_doClamp;
};

#endif // GAGEADAPTOR_INCLUDED

