// OpenGL Matrices are COLUMN major.
#define MATRIX_ELEMENT(M,i,j) (*(M + i + (j<<2)))

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

#include <GL/glew.h>

#include <GL/gl.h>

#include <cmath>

#include <ErrorManager.h>
#include <MyMath.h>

#include <MyGLH.h>

using namespace std;

#ifdef MY_ENABLE_POINT_SPRITES
CTexture *CMyGLH::m_pointSpriteTexturePointer = 0;
#endif // MY_ENABLE_POINT_SPRITES

namespace MyGLH 
{
	/**
	*/
	bool IsRenderingContextOk(void)
	{
		bool isOk;
		GLenum error;

		isOk = true;

		error = glGetError();

		if (error != GL_NO_ERROR)
		{
			do 
			{
				switch(error) 
				{
				case GL_INVALID_ENUM:
					CErrorManager::Instance().PushError("An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.");
					break;
				case GL_INVALID_VALUE:
					CErrorManager::Instance().PushError("A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.");
					break;
				case GL_INVALID_OPERATION:
					CErrorManager::Instance().PushError("The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.");
					break;
				case GL_STACK_OVERFLOW:
					CErrorManager::Instance().PushError("This command would cause a stack overflow. The offending command is ignored and has no other side effect than to set the error flag.");
					break;
				case GL_STACK_UNDERFLOW:
					CErrorManager::Instance().PushError("This command would cause a stack underflow. The offending command is ignored and has no other side effect than to set the error flag.");
					break;
				case GL_OUT_OF_MEMORY:
					CErrorManager::Instance().PushError("There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded.");
					break;
				case GL_TABLE_TOO_LARGE:
					CErrorManager::Instance().PushError("The specified table exceeds the implementation's maximum supported table size.  The offending command is ignored and has no other side effect than to set the error flag.");
					break;
				}
			} while ((error = glGetError()) != GL_NO_ERROR);

			isOk = false;
		}

		return isOk;
	}

	/**
	*/
	void DrawGrid(void)
	{
		float currentColor[4];
		unsigned int i;

		glLineStipple(1, 0xaaaa);

		glEnable(GL_LINE_STIPPLE);

		glGetFloatv(GL_CURRENT_COLOR, currentColor);

		glColor3f(0.5f, 0.5f, 0.5f);

		glBegin(GL_LINES);

		for (i=0; i<11; i++)
		{
			// yz
			glVertex3f(0.0f, i/10.0f, 0.0f);
			glVertex3f(0.0f, i/10.0f, 1.0f);

			glVertex3f(0.0f, 0.0f, i/10.0f);
			glVertex3f(0.0f, 1.0f, i/10.0f);

			// xz
			glVertex3f(i/10.0f, 0.0f, 0.0f);
			glVertex3f(i/10.0f, 0.0f, 1.0f);

			glVertex3f(0.0f, 0.0f, i/10.0f);
			glVertex3f(1.0f, 0.0f, i/10.0f);

			// xy
			glVertex3f(i/10.0f, 0.0f, 0.0f);
			glVertex3f(i/10.0f, 1.0f, 0.0f);

			glVertex3f(0.0f, i/10.0f, 0.0f);
			glVertex3f(1.0f, i/10.0f, 0.0f);
		}

		glEnd();

		glDisable(GL_LINE_STIPPLE);

		glLineWidth(2.0f);

		glBegin(GL_LINES);

		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3i(0, 0, 0);
		glVertex3i(1, 0, 0);

		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3i(0, 0, 0);
		glVertex3i(0, 1, 0);

		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3i(0, 0, 0);
		glVertex3i(0, 0, 1);

		glEnd();

		glLineWidth(1.0f);

		glColor4fv(currentColor);
	}

	/**
	*/
	float *GetTriangleNormal(const float *A, const float *B, const float *C, float *normal)
	{
		float AB[3],
			AC[3];
		int i;

		for (i=0; i<3; ++i)
		{
			AB[i] = B[i] - A[i];
			AC[i] = C[i] - A[i];
		}

		MyMath::Normalize<float, 3>(AB);
		MyMath::Normalize<float, 3>(AC);

		MyMath::CrossProduct<float>(AB, AC, normal);

		return normal;
	}

	/**
	*/
	void MultiplyTransformMatrices(const float *firstMatrix, const float *secondMatrix, float *productMatrix)
	{
		int i;

		for (i=0; i<4; i++) 
		{
			MATRIX_ELEMENT(productMatrix, i, 0) = MATRIX_ELEMENT(firstMatrix, i, 0) * MATRIX_ELEMENT(secondMatrix, 0, 0) + MATRIX_ELEMENT(firstMatrix, i, 1) * MATRIX_ELEMENT(secondMatrix, 1, 0) + MATRIX_ELEMENT(firstMatrix, i, 2) * MATRIX_ELEMENT(secondMatrix, 2, 0) + MATRIX_ELEMENT(firstMatrix, i, 3) * MATRIX_ELEMENT(secondMatrix, 3, 0);
			MATRIX_ELEMENT(productMatrix, i, 1) = MATRIX_ELEMENT(firstMatrix, i, 0) * MATRIX_ELEMENT(secondMatrix, 0, 1) + MATRIX_ELEMENT(firstMatrix, i, 1) * MATRIX_ELEMENT(secondMatrix, 1, 1) + MATRIX_ELEMENT(firstMatrix, i, 2) * MATRIX_ELEMENT(secondMatrix, 2, 1) + MATRIX_ELEMENT(firstMatrix, i, 3) * MATRIX_ELEMENT(secondMatrix, 3, 1);
			MATRIX_ELEMENT(productMatrix, i, 2) = MATRIX_ELEMENT(firstMatrix, i, 0) * MATRIX_ELEMENT(secondMatrix, 0, 2) + MATRIX_ELEMENT(firstMatrix, i, 1) * MATRIX_ELEMENT(secondMatrix, 1, 2) + MATRIX_ELEMENT(firstMatrix, i, 2) * MATRIX_ELEMENT(secondMatrix, 2, 2) + MATRIX_ELEMENT(firstMatrix, i, 3) * MATRIX_ELEMENT(secondMatrix, 3, 2);
			MATRIX_ELEMENT(productMatrix, i, 3) = MATRIX_ELEMENT(firstMatrix, i, 0) * MATRIX_ELEMENT(secondMatrix, 0, 3) + MATRIX_ELEMENT(firstMatrix, i, 1) * MATRIX_ELEMENT(secondMatrix, 1, 3) + MATRIX_ELEMENT(firstMatrix, i, 2) * MATRIX_ELEMENT(secondMatrix, 2, 3) + MATRIX_ELEMENT(firstMatrix, i, 3) * MATRIX_ELEMENT(secondMatrix, 3, 3);
		}
	}
};

/**
*/
void CMyGLH::DrawLine(float *originCoord, float *destinationCoord, int widthMax)
{
	float currentColor[4];
	int i;

	glGetFloatv(GL_CURRENT_COLOR, currentColor);

	glDepthMask(GL_FALSE);

	glEnable(GL_LINE_SMOOTH);

	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	for (i=widthMax; i>0; i--)
	{
		glLineWidth((float)i*i);

		glColor3f(max(currentColor[0] - ((float)i/widthMax), 0.0f), max(currentColor[1] - ((float)i/widthMax), 0.0f), max(currentColor[2] - ((float)i/widthMax), 0.0f));

		glBegin(GL_LINES);

		glVertex3fv(originCoord);
		glVertex3fv(destinationCoord);

		glEnd();
	}

	glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);

	glDisable(GL_LINE_SMOOTH);

	glDepthMask(GL_TRUE);

	glColor4fv(currentColor);
}

/**
*/
bool CMyGLH::DrawCylinder(float *originCoord, float *destinationCoord, float bottomRadius, float topRadius, int slices)
{
	int i;
	float currentColor[4],
		norma,
		axis[3],
		u[3],
		v[3],
		angle,
		pu,
		pv,
		spub,
		spvb,
		sput,
		spvt;

	glGetFloatv(GL_CURRENT_COLOR, currentColor);

	norma = 0.0f;

	for (i=3; i--; )
	{
		axis[i] = destinationCoord[i] - originCoord[i];

		norma += axis[i]*axis[i];
	}

	if (norma < MyMath::GetMinimumPositiveValue<float>())
	{
		MarkError();

		return false;
	}

	norma = sqrtf(norma);

	for (i=3; i--; )
		axis[i] /= norma;

	if ((axis[0] + axis[2]) > MyMath::GetMinimumPositiveValue<float>())
	{
		u[0] = -axis[2];
		u[1] = 0.0f;
		u[2] = axis[0];
	}
	else
	{
		u[0] = axis[1];
		u[1] = -axis[0];
		u[2] = 0.0f;
	}

	norma = 0.0f;

	for (i=3; i--; )
		norma += u[i]*u[i];

	if (norma < MyMath::GetMinimumPositiveValue<float>())
	{
		MarkError();

		return false;
	}

	norma = sqrtf(norma);

	for (i=3; i--; )
		u[i] /= norma;

	v[0] = u[1]*axis[2] - u[2]*axis[1];
	v[1] = u[2]*axis[0] - u[0]*axis[2];
	v[2] = u[0]*axis[1] - u[1]*axis[0];

	norma = 0.0f;

	for (i=3; i--; )
		norma += v[i]*v[i];

	if (norma < MyMath::GetMinimumPositiveValue<float>())
	{
		MarkError();

		return false;
	}

	norma = sqrtf(norma);

	for (i=3; i--; )
		v[i] /= norma;

	glBegin(GL_TRIANGLE_STRIP);

	for (i=slices; i--; )
	{
		angle = ((float)i/(slices - 1))*2.0f*MyMath::Pi<float>();

		pu = cos(angle);
		pv = sin(angle);

		spub = bottomRadius*pu;
		spvb = bottomRadius*pv;
		sput = topRadius*pu;
		spvt = topRadius*pv;

		glNormal3f(pu*u[0] + pv*v[0], pu*u[1] + pv*v[1], pu*u[2] + pv*v[2]);

		glVertex3f(originCoord[0] + spub*u[0] + spvb*v[0], originCoord[1] + spub*u[1] + spvb*v[1], originCoord[2] + spub*u[2] + spvb*v[2]);
		glVertex3f(destinationCoord[0] + sput*u[0] + spvt*v[0], destinationCoord[1] + sput*u[1] + spvt*v[1], destinationCoord[2] + sput*u[2] + spvt*v[2]);
	}

	glEnd();

	glColor4fv(currentColor);

	return true;
}

/*
*/
void CMyGLH::TransformVertex3(const float *transformMatrix, const float *inputVertex, float *outputVertex)
{
   outputVertex[0] = MATRIX_ELEMENT(transformMatrix, 0, 0)*inputVertex[0] + MATRIX_ELEMENT(transformMatrix, 0, 1)*inputVertex[1] + MATRIX_ELEMENT(transformMatrix, 0, 2)*inputVertex[2] + MATRIX_ELEMENT(transformMatrix, 0, 3);
   outputVertex[1] = MATRIX_ELEMENT(transformMatrix, 1, 0)*inputVertex[0] + MATRIX_ELEMENT(transformMatrix, 1, 1)*inputVertex[1] + MATRIX_ELEMENT(transformMatrix, 1, 2)*inputVertex[2] + MATRIX_ELEMENT(transformMatrix, 1, 3);
   outputVertex[2] = MATRIX_ELEMENT(transformMatrix, 2, 0)*inputVertex[0] + MATRIX_ELEMENT(transformMatrix, 2, 1)*inputVertex[1] + MATRIX_ELEMENT(transformMatrix, 2, 2)*inputVertex[2] + MATRIX_ELEMENT(transformMatrix, 2, 3);
   outputVertex[3] = MATRIX_ELEMENT(transformMatrix, 3, 0)*inputVertex[0] + MATRIX_ELEMENT(transformMatrix, 3, 1)*inputVertex[1] + MATRIX_ELEMENT(transformMatrix, 3, 2)*inputVertex[2] + MATRIX_ELEMENT(transformMatrix, 3, 3);
}

/*
*/
void CMyGLH::TransformVertex4(const float *transformMatrix, const float *inputVertex, float *outputVertex)
{
   outputVertex[0] = MATRIX_ELEMENT(transformMatrix, 0, 0)*inputVertex[0] + MATRIX_ELEMENT(transformMatrix, 0, 1)*inputVertex[1] + MATRIX_ELEMENT(transformMatrix, 0, 2)*inputVertex[2] + MATRIX_ELEMENT(transformMatrix, 0, 3)*inputVertex[3];
   outputVertex[1] = MATRIX_ELEMENT(transformMatrix, 1, 0)*inputVertex[0] + MATRIX_ELEMENT(transformMatrix, 1, 1)*inputVertex[1] + MATRIX_ELEMENT(transformMatrix, 1, 2)*inputVertex[2] + MATRIX_ELEMENT(transformMatrix, 1, 3)*inputVertex[3];
   outputVertex[2] = MATRIX_ELEMENT(transformMatrix, 2, 0)*inputVertex[0] + MATRIX_ELEMENT(transformMatrix, 2, 1)*inputVertex[1] + MATRIX_ELEMENT(transformMatrix, 2, 2)*inputVertex[2] + MATRIX_ELEMENT(transformMatrix, 2, 3)*inputVertex[3];
   outputVertex[3] = MATRIX_ELEMENT(transformMatrix, 3, 0)*inputVertex[0] + MATRIX_ELEMENT(transformMatrix, 3, 1)*inputVertex[1] + MATRIX_ELEMENT(transformMatrix, 3, 2)*inputVertex[2] + MATRIX_ELEMENT(transformMatrix, 3, 3)*inputVertex[3];
}

/**
*/
void CMyGLH::MultiplyTransformMatrix4x4(const float *firstMatrix, const float *secondMatrix, float *productMatrix)
{
	int i;

	for (i=0; i<4; i++) 
	{
		MATRIX_ELEMENT(productMatrix, i, 0) = MATRIX_ELEMENT(firstMatrix, i, 0) * MATRIX_ELEMENT(secondMatrix, 0, 0) + MATRIX_ELEMENT(firstMatrix, i, 1) * MATRIX_ELEMENT(secondMatrix, 1, 0) + MATRIX_ELEMENT(firstMatrix, i, 2) * MATRIX_ELEMENT(secondMatrix, 2, 0) + MATRIX_ELEMENT(firstMatrix, i, 3) * MATRIX_ELEMENT(secondMatrix, 3, 0);
		MATRIX_ELEMENT(productMatrix, i, 1) = MATRIX_ELEMENT(firstMatrix, i, 0) * MATRIX_ELEMENT(secondMatrix, 0, 1) + MATRIX_ELEMENT(firstMatrix, i, 1) * MATRIX_ELEMENT(secondMatrix, 1, 1) + MATRIX_ELEMENT(firstMatrix, i, 2) * MATRIX_ELEMENT(secondMatrix, 2, 1) + MATRIX_ELEMENT(firstMatrix, i, 3) * MATRIX_ELEMENT(secondMatrix, 3, 1);
		MATRIX_ELEMENT(productMatrix, i, 2) = MATRIX_ELEMENT(firstMatrix, i, 0) * MATRIX_ELEMENT(secondMatrix, 0, 2) + MATRIX_ELEMENT(firstMatrix, i, 1) * MATRIX_ELEMENT(secondMatrix, 1, 2) + MATRIX_ELEMENT(firstMatrix, i, 2) * MATRIX_ELEMENT(secondMatrix, 2, 2) + MATRIX_ELEMENT(firstMatrix, i, 3) * MATRIX_ELEMENT(secondMatrix, 3, 2);
		MATRIX_ELEMENT(productMatrix, i, 3) = MATRIX_ELEMENT(firstMatrix, i, 0) * MATRIX_ELEMENT(secondMatrix, 0, 3) + MATRIX_ELEMENT(firstMatrix, i, 1) * MATRIX_ELEMENT(secondMatrix, 1, 3) + MATRIX_ELEMENT(firstMatrix, i, 2) * MATRIX_ELEMENT(secondMatrix, 2, 3) + MATRIX_ELEMENT(firstMatrix, i, 3) * MATRIX_ELEMENT(secondMatrix, 3, 3);
	}
}

/**
*/
bool CMyGLH::InvertTransformMatrix(const float *transformMatrix, float *inverseTransformMatrix)
{
	float wtmp[4][8],
		m0, 
		m1, 
		m2, 
		m3, 
		s,
		*r0, 
		*r1, 
		*r2, 
		*r3,
		*_tmp;

	r0 = wtmp[0];
	r1 = wtmp[1];
	r2 = wtmp[2];
	r3 = wtmp[3];

	r0[0] = MATRIX_ELEMENT(transformMatrix, 0, 0);
	r0[1] = MATRIX_ELEMENT(transformMatrix, 0, 1);
	r0[2] = MATRIX_ELEMENT(transformMatrix, 0, 2);
	r0[3] = MATRIX_ELEMENT(transformMatrix, 0, 3);
	r0[4] = 1.0f;
	r0[5] = 0.0f;
	r0[6] = 0.0f;
	r0[7] = 0.0f;
	
	r1[0] = MATRIX_ELEMENT(transformMatrix, 1, 0);
	r1[1] = MATRIX_ELEMENT(transformMatrix, 1, 1);
	r1[2] = MATRIX_ELEMENT(transformMatrix, 1, 2);
	r1[3] = MATRIX_ELEMENT(transformMatrix, 1, 3);
	r1[5] = 1.0f;
	r1[4] = 0.0f;
	r1[6] = 0.0f;
	r1[7] = 0.0f;

	r2[0] = MATRIX_ELEMENT(transformMatrix, 2, 0);
	r2[1] = MATRIX_ELEMENT(transformMatrix, 2, 1);
	r2[2] = MATRIX_ELEMENT(transformMatrix, 2, 2);
	r2[3] = MATRIX_ELEMENT(transformMatrix, 2, 3);
	r2[6] = 1.0f;
	r2[4] = 0.0f;
	r2[5] = 0.0f;
	r2[7] = 0.0f;
	
	r3[0] = MATRIX_ELEMENT(transformMatrix, 3, 0);
	r3[1] = MATRIX_ELEMENT(transformMatrix, 3, 1);
	r3[2] = MATRIX_ELEMENT(transformMatrix, 3, 2);
	r3[3] = MATRIX_ELEMENT(transformMatrix, 3, 3);
	r3[7] = 1.0f;
	r3[4] = 0.0f;
	r3[5] = 0.0f;
	r3[6] = 0.0f;

	// Choose a pivot.
	if (fabs(r3[0]) > fabs(r2[0]))
	{
		_tmp = r3; 
		r3 = r2; 
		r2 = _tmp;
	}

	if (fabs(r2[0]) > fabs(r1[0]))
	{
		_tmp = r2; 
		r2 = r1; 
		r1 = _tmp;
	}

	if (fabs(r1[0]) > fabs(r0[0]))
	{
		_tmp = r1; 
		r1 = r0; 
		r0 = _tmp;
	}

	if (0.0f == r0[0])
	{
		MarkError();

		return false;
	}

	// Eliminate first variable.
	m1 = r1[0]/r0[0];
	m2 = r2[0]/r0[0];
	m3 = r3[0]/r0[0];
	
	s = r0[1];

	r1[1] -= m1*s;
	r2[1] -= m2*s;
	r3[1] -= m3*s;
	
	s = r0[2];

	r1[2] -= m1*s;
	r2[2] -= m2*s;
	r3[2] -= m3*s;
	
	s = r0[3];

	r1[3] -= m1*s;
	r2[3] -= m2*s;
	r3[3] -= m3*s;
	
	s = r0[4];

	if (s != 0.0) 
	{
		r1[4] -= m1*s;
		r2[4] -= m2*s;
		r3[4] -= m3*s;
	}
	
	s = r0[5];

	if (s != 0.0) 
	{
		r1[5] -= m1*s;
		r2[5] -= m2*s;
		r3[5] -= m3*s;
	}
	
	s = r0[6];
	
	if (s != 0.0) 
	{
		r1[6] -= m1*s;
		r2[6] -= m2*s;
		r3[6] -= m3*s;
	}
	
	s = r0[7];
	
	if (s != 0.0) 
	{
		r1[7] -= m1*s;
		r2[7] -= m2*s;
		r3[7] -= m3*s;
	}

	// Choose pivot.
	if (fabs(r3[1]) > fabs(r2[1]))
	{
		_tmp = r3; 
		r3 = r2; 
		r2 = _tmp;
	}

	if (fabs(r2[1]) > fabs(r1[1]))
	{
		_tmp = r2; 
		r2 = r1; 
		r1 = _tmp;
	}

	if (0.0 == r1[1])
	{
		MarkError();

		return false;
	}

	// Eliminate second variable.
	m2 = r2[1]/r1[1];
	m3 = r3[1]/r1[1];

	r2[2] -= m2*r1[2];
	r3[2] -= m3*r1[2];
	r2[3] -= m2*r1[3];
	r3[3] -= m3*r1[3];

	s = r1[4];

	if (0.0 != s) 
	{
		r2[4] -= m2*s;
		r3[4] -= m3*s;
	}
	
	s = r1[5];
	
	if (0.0 != s) 
	{
		r2[5] -= m2*s;
		r3[5] -= m3*s;
	}

	s = r1[6];

	if (0.0 != s) 
	{
		r2[6] -= m2*s;
		r3[6] -= m3*s;
	}

	s = r1[7];

	if (0.0 != s) 
	{
		r2[7] -= m2*s;
		r3[7] -= m3*s;
	}

	// Choose pivot.
	if (fabs(r3[2]) > fabs(r2[2]))
	{
		_tmp = r3; 
		r3 = r2; 
		r2 = _tmp;
	}

	if (0.0 == r2[2])
	{
		MarkError();

		return false;
	}

	// Eliminate third variable.
	m3 = r3[2]/r2[2];

	r3[3] -= m3*r2[3];
	r3[4] -= m3*r2[4];
	r3[5] -= m3*r2[5];
	r3[6] -= m3*r2[6];
	r3[7] -= m3*r2[7];

	// Last check.
	if (0.0 == r3[3])
	{
		MarkError();

		return false;
	}

	// Now back substitute row 3.
	s = 1.0f/r3[3];

	r3[4] *= s;
	r3[5] *= s;
	r3[6] *= s;
	r3[7] *= s;

	// Now back substitute row 2.
	m2 = r2[3];

	s = 1.0f/r2[2];

	r2[4] = s*(r2[4] - r3[4]*m2);
	r2[5] = s*(r2[5] - r3[5]*m2);
	r2[6] = s*(r2[6] - r3[6]*m2);
	r2[7] = s*(r2[7] - r3[7]*m2);

	m1 = r1[3];

	r1[4] -= r3[4]*m1;
	r1[5] -= r3[5]*m1;
	r1[6] -= r3[6]*m1;
	r1[7] -= r3[7]*m1;

	m0 = r0[3];

	r0[4] -= r3[4]*m0;
	r0[5] -= r3[5]*m0;
	r0[6] -= r3[6]*m0;
	r0[7] -= r3[7]*m0;

	// Now back substitute row 1.
	m1 = r1[2];
	
	s = 1.0f/r1[1];

	r1[4] = s*(r1[4] - r2[4]*m1);
	r1[5] = s*(r1[5] - r2[5]*m1);
	r1[6] = s*(r1[6] - r2[6]*m1);
	r1[7] = s*(r1[7] - r2[7]*m1);

	m0 = r0[2];

	r0[4] -= r2[4]*m0;
	r0[5] -= r2[5]*m0;
	r0[6] -= r2[6]*m0;
	r0[7] -= r2[7]*m0;

	// Now back substitute row 0.
	m0 = r0[1];			

	s = 1.0f/r0[0];

	r0[4] = s*(r0[4] - r1[4]*m0);
	r0[5] = s*(r0[5] - r1[5]*m0);
	r0[6] = s*(r0[6] - r1[6]*m0);
	r0[7] = s*(r0[7] - r1[7]*m0);

	MATRIX_ELEMENT(inverseTransformMatrix, 0, 0) = r0[4];
	MATRIX_ELEMENT(inverseTransformMatrix, 0, 1) = r0[5];
	MATRIX_ELEMENT(inverseTransformMatrix, 0, 2) = r0[6];
	MATRIX_ELEMENT(inverseTransformMatrix, 0, 3) = r0[7];
	MATRIX_ELEMENT(inverseTransformMatrix, 1, 0) = r1[4];
	MATRIX_ELEMENT(inverseTransformMatrix, 1, 1) = r1[5];
	MATRIX_ELEMENT(inverseTransformMatrix, 1, 2) = r1[6];
	MATRIX_ELEMENT(inverseTransformMatrix, 1, 3) = r1[7];
	MATRIX_ELEMENT(inverseTransformMatrix, 2, 0) = r2[4];
	MATRIX_ELEMENT(inverseTransformMatrix, 2, 1) = r2[5];
	MATRIX_ELEMENT(inverseTransformMatrix, 2, 2) = r2[6];
	MATRIX_ELEMENT(inverseTransformMatrix, 2, 3) = r2[7];
	MATRIX_ELEMENT(inverseTransformMatrix, 3, 0) = r3[4];
	MATRIX_ELEMENT(inverseTransformMatrix, 3, 1) = r3[5];
	MATRIX_ELEMENT(inverseTransformMatrix, 3, 2) = r3[6];
	MATRIX_ELEMENT(inverseTransformMatrix, 3, 3) = r3[7];

	return true;
}

/**
*/
void CMyGLH::TransposeTransformMatrix(float *transformMatrix)
{
	int i,
		j;
	float tmp;

	for (i=0; i<4; i++)
		for (j=0; j<i; j++)
		{
			tmp = transformMatrix[i*4 + j];
			transformMatrix[i*4 + j] = transformMatrix[j*4 + i];
			transformMatrix[j*4 + i] = tmp;
		}
}

/**
*/
bool CMyGLH::ProjectVertex3(const float viewMatrix[16], const float projectionMatrix[16], const int viewportCoord[4], const float *inputVertex, float *outputVertex)
{
	float homogeneousInputVertex[4], 
		homogeneousOutputVertex[4];

	homogeneousInputVertex[0] = inputVertex[0];
	homogeneousInputVertex[1] = inputVertex[1];
	homogeneousInputVertex[2] = inputVertex[2];
	homogeneousInputVertex[3] = 1.0f;
	
	TransformVertex4(viewMatrix, homogeneousInputVertex, homogeneousOutputVertex);
	TransformVertex4(projectionMatrix, homogeneousOutputVertex, homogeneousInputVertex);

	if (homogeneousInputVertex[3] == 0.0f)
	{
		MarkError();

		return false;
	}

	homogeneousInputVertex[0] /= homogeneousInputVertex[3];
	homogeneousInputVertex[1] /= homogeneousInputVertex[3];
	homogeneousInputVertex[2] /= homogeneousInputVertex[3];

	outputVertex[0] = viewportCoord[0] + (1.0f + homogeneousInputVertex[0])*viewportCoord[2]/2.0f;
	outputVertex[1] = viewportCoord[1] + (1.0f + homogeneousInputVertex[1])*viewportCoord[3]/2.0f;
	
	outputVertex[2] = (1.0f + homogeneousInputVertex[2])/2.0f;
	
	return true;
}

bool CMyGLH::UnprojectVertex3(const float viewMatrix[16], const float projectionMatrix[16], const int viewportCoord[4], const float *inputVertex, float *outputVertex)
{
	float inverseTransformMatrix[16], 
		transformMatrix[16],
		homogeneousInputVertex[4], 
		homogeneousOutputVertex[4];

	homogeneousInputVertex[0] = (inputVertex[0] - viewportCoord[0])*2.0f/viewportCoord[2] - 1.0f;
	homogeneousInputVertex[1] = (inputVertex[1] - viewportCoord[1])*2.0f/viewportCoord[3] - 1.0f;
	homogeneousInputVertex[2] = 2.0f*inputVertex[2] - 1.0f;
	homogeneousInputVertex[3] = 1.0f;

	MultiplyTransformMatrix4x4(projectionMatrix, viewMatrix, transformMatrix);

	InvertTransformMatrix(transformMatrix, inverseTransformMatrix);

	TransformVertex4(inverseTransformMatrix, homogeneousInputVertex, homogeneousOutputVertex);

	if (homogeneousOutputVertex[3] == 0.0f)
	{
		MarkError();

		return false;
	}

	outputVertex[0] = homogeneousOutputVertex[0]/homogeneousOutputVertex[3];
	outputVertex[1] = homogeneousOutputVertex[1]/homogeneousOutputVertex[3];
	outputVertex[2] = homogeneousOutputVertex[2]/homogeneousOutputVertex[3];

	return true;
}

#ifdef MY_ENABLE_POINT_SPRITES
/**
*/
bool CMyGLH::EnablePointSprites(void)
{
	static CTexture pointSpriteTexture;
	vector<unsigned char> spriteTextureBuffer;
	int i,
		j;
	float normalizedDistance,
		luminance,
		alpha;

	if (!m_pointSpriteTexturePointer)
	{
		for (i=0; i<16; i++)
			for (j=0; j<16; j++)
			{
				normalizedDistance = sqrtf(MyMath::SquaredDifference((float)i, (float)j, 7.5f, 7.5f));

				normalizedDistance /= 8.0f;

				if (normalizedDistance < 0.0f)
					normalizedDistance = 0.0f;
				else if (normalizedDistance > 1.0f)
					normalizedDistance = 1.0f;

				luminance = 1.0f - MyMath::Bias<float>(0.4f, normalizedDistance);

				spriteTextureBuffer.push_back((unsigned char)((int)(luminance*255.0f)));

				alpha = 1.0f - MyMath::Bias<float>(0.6f, normalizedDistance);

				spriteTextureBuffer.push_back((unsigned char)((int)(alpha*255.0f)));
			}

		if (!pointSpriteTexture.Initialize(GL_TEXTURE_2D, GL_LUMINANCE_ALPHA))
		{
			MarkError();

			return false;
		}

		if (!pointSpriteTexture.SetImage(16, 16, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, &spriteTextureBuffer[0]))
		{
			MarkError();

			return false;
		}

		if (!pointSpriteTexture.IsValid())
		{
			MarkError();

			return false;
		}

		pointSpriteTexture.SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		pointSpriteTexture.SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		pointSpriteTexture.SetParameter(GL_TEXTURE_WRAP_S, GL_CLAMP);
		pointSpriteTexture.SetParameter(GL_TEXTURE_WRAP_T, GL_CLAMP);

		m_pointSpriteTexturePointer = &pointSpriteTexture;
	}

	glEnable(GL_POINT_SPRITE_ARB);

	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

	glTexEnvi(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);

	m_pointSpriteTexturePointer->Bind(0);

	glAlphaFunc(GL_GREATER, 0.25f);

	glEnable(GL_ALPHA_TEST);

	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glEnable(GL_BLEND);

	return true;
}

/**
*/
void CMyGLH::DisablePointSprites(void)
{
	//glDisable(GL_BLEND);

	glDisable(GL_ALPHA_TEST);

	glHint(GL_POINT_SMOOTH_HINT, GL_DONT_CARE);

	glDisable(GL_POINT_SPRITE_ARB);

	CTexture::DisableTextureUnit(0);
}
#endif // MY_ENABLE_POINT_SPRITES

