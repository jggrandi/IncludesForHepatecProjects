#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

#include <GL/gl.h>
#include <GL/glu.h>

#include <cmath>
#include <fstream>

#include <ErrorManager.h>

#include <PinholeCamera.h>

using namespace std;

/**
*/
CPinholeCamera::CPinholeCamera(void)
{
	Create();
}

/**
*/
CPinholeCamera::CPinholeCamera(const CPinholeCamera &pinholeCamera)
{
	Copy(pinholeCamera);
}

/**
*/
CPinholeCamera::~CPinholeCamera(void)
{
}

/**
*/
CPinholeCamera& CPinholeCamera::operator=(const CPinholeCamera &pinholeCamera)
{
	Copy(pinholeCamera);

	return *this;
}

/**
*/
bool CPinholeCamera::Create(float leftClippingPlaneCoord, float rightClippingPlaneCoord, float bottomClippingPlaneCoord, float topClippingPlaneCoord, float nearPlaneDepth, float farPlaneDepth, int windowWidth, int windowHeight)
{
	m_projectionType = ORTHOGONAL_PROJECTION;

	// Change only the necessary.
	//m_cameraType = TRACKBALL_CAMERA;

	m_leftClippingPlaneCoord = leftClippingPlaneCoord;
	m_rightClippingPlaneCoord = rightClippingPlaneCoord;
	m_bottomClippingPlaneCoord = bottomClippingPlaneCoord;
	m_topClippingPlaneCoord = topClippingPlaneCoord;

	m_fieldOfView = 0.0f;
	
	m_nearPlaneDepth = nearPlaneDepth;
	m_farPlaneDepth = farPlaneDepth;

	m_viewport[0] = 0;
	m_viewport[1] = 0;
	m_viewport[2] = windowWidth;
	m_viewport[3] = windowHeight;

	m_pitchRotation = 0.0f;
	m_yawRotation = 0.0f;

	m_eyeVectorDisplacement = 0.0f;
	m_sideVectorDisplacement = 0.0f;
	m_upVectorDisplacement = 0.0f;
	
	// Change only the necessary.
	//ResetViewMatrix();

	CalculateProjectionMatrix();

	// It doesn't make any sense here.
	ResetImagePlaneMatrix();

	return true;
}

/**
*/
bool CPinholeCamera::Create(float fieldOfView, float nearPlaneDepth, float farPlaneDepth, int windowWidth, int windowHeight)
{
	m_projectionType = PERSPECTIVE_PROJECTION;

	// Change only the necessary.
	//m_cameraType = TRACKBALL_CAMERA;

	m_leftClippingPlaneCoord = -1.0f;
	m_rightClippingPlaneCoord = 1.0f;
	m_bottomClippingPlaneCoord = -1.0f;
	m_topClippingPlaneCoord = 1.0f;

	m_fieldOfView = fieldOfView;
	
	m_nearPlaneDepth = nearPlaneDepth;
	m_farPlaneDepth = farPlaneDepth;

	m_viewport[0] = 0;
	m_viewport[1] = 0;
	m_viewport[2] = windowWidth;
	m_viewport[3] = windowHeight;

	m_pitchRotation = 0.0f;
	m_yawRotation = 0.0f;

	m_eyeVectorDisplacement = 0.0f;
	m_sideVectorDisplacement = 0.0f;
	m_upVectorDisplacement = 0.0f;
	
	// Change only the necessary.
	//ResetViewMatrix();

	CalculateProjectionMatrix();

	CalculateImagePlaneMatrix();

	return true;
}

/**
*/
float CPinholeCamera::GetLeftClippingPlaneCoord(void) const
{
	return m_leftClippingPlaneCoord;
}

/**
*/
bool CPinholeCamera::SetLeftClippingPlaneCoord(float coord)
{
	if (!UpdateProjection(coord, m_rightClippingPlaneCoord, m_bottomClippingPlaneCoord, m_topClippingPlaneCoord, m_nearPlaneDepth, m_farPlaneDepth))
	{
		MarkError();

		return false;
	}

	return true;
}

/**
*/
float CPinholeCamera::GetRightClippingPlaneCoord(void) const
{
	return m_rightClippingPlaneCoord;
}

/**
*/
bool CPinholeCamera::SetRightClippingPlaneCoord(float coord)
{
	if (!UpdateProjection(m_leftClippingPlaneCoord, coord, m_bottomClippingPlaneCoord, m_topClippingPlaneCoord, m_nearPlaneDepth, m_farPlaneDepth))
	{
		MarkError();

		return false;
	}

	return true;
}

/**
*/
float CPinholeCamera::GetBottomClippingPlaneCoord(void) const
{
	return m_bottomClippingPlaneCoord;
}

/**
*/
bool CPinholeCamera::SetBottomClippingPlaneCoord(float coord)
{
	if (!UpdateProjection(m_leftClippingPlaneCoord, m_rightClippingPlaneCoord, coord, m_topClippingPlaneCoord, m_nearPlaneDepth, m_farPlaneDepth))
	{
		MarkError();

		return false;
	}

	return true;
}

/**
*/
float CPinholeCamera::GetTopClippingPlaneCoord(void) const
{
	return m_topClippingPlaneCoord;
}

/**
*/
bool CPinholeCamera::SetTopClippingPlaneCoord(float coord)
{
	if (!UpdateProjection(m_leftClippingPlaneCoord, m_rightClippingPlaneCoord, m_bottomClippingPlaneCoord, coord, m_nearPlaneDepth, m_farPlaneDepth))
	{
		MarkError();

		return false;
	}

	return true;
}

/**
*/
float CPinholeCamera::GetFieldOfView(void) const
{
	return m_fieldOfView;
}

/**
*/
bool CPinholeCamera::SetFieldOfView(float fieldOfView)
{
	if (fieldOfView < 1.0f)
	{
		MarkError();

		return false;
	}

	if (!UpdateProjection(fieldOfView, m_nearPlaneDepth, m_farPlaneDepth))
	{
		MarkError();

		return false;
	}

	return true;
}

/**
*/
float CPinholeCamera::GetNearPlaneDepth(void) const
{
	return m_nearPlaneDepth;
}

/**
*/
bool CPinholeCamera::SetNearPlaneDepth(float depth)
{
	if (!UpdateProjection(m_fieldOfView, depth, m_farPlaneDepth))
	{
		MarkError();

		return false;
	}

	return true;
}

/**
*/
float CPinholeCamera::GetFarPlaneDepth(void) const
{
	return m_farPlaneDepth;
}

/**
*/
bool CPinholeCamera::SetFarPlaneDepth(float depth)
{
	if (!UpdateProjection(m_fieldOfView, m_nearPlaneDepth, depth))
	{
		MarkError();

		return false;
	}

	return true;
}

/**
*/
void CPinholeCamera::SetViewport(int x, int y, int width, int height)
{
	m_viewport[0] = x;
	m_viewport[1] = y;
	m_viewport[2] = width;
	m_viewport[3] = height;

	CalculateProjectionMatrix();
}

/**
*/
void CPinholeCamera::Pitch(float angle)
{
	m_pitchRotation += angle;
}

/**
*/
void CPinholeCamera::Yaw(float angle)
{
	m_yawRotation += angle;
}

/**
*/
void CPinholeCamera::MoveFront(float step)
{
	m_eyeVectorDisplacement += step;
}

/**
*/
void CPinholeCamera::MoveSide(float step)
{
	m_sideVectorDisplacement += step;
}

/**
*/
void CPinholeCamera::MoveUp(float step)
{
	m_upVectorDisplacement += step;
}

/**
*/
void CPinholeCamera::ApplyTransform(void)
{
	glMatrixMode(GL_PROJECTION);

	glLoadMatrixf(m_projectionMatrix);

	glMatrixMode(GL_MODELVIEW);

	glLoadMatrixf(m_viewMatrix);

	if (m_cameraType == TRACKBALL_CAMERA)
	{
		glTranslatef(m_sideVectorDisplacement*m_viewMatrix[0] + m_upVectorDisplacement*m_viewMatrix[1] + m_eyeVectorDisplacement*m_viewMatrix[2], m_sideVectorDisplacement*m_viewMatrix[4] + m_upVectorDisplacement*m_viewMatrix[5] + m_eyeVectorDisplacement*m_viewMatrix[6], m_sideVectorDisplacement*m_viewMatrix[8] + m_upVectorDisplacement*m_viewMatrix[9] + m_eyeVectorDisplacement*m_viewMatrix[10]);

		glRotatef(m_pitchRotation, m_viewMatrix[0], m_viewMatrix[4], m_viewMatrix[8]);
		glRotatef(m_yawRotation, m_viewMatrix[1], m_viewMatrix[5], m_viewMatrix[9]);
	}
	else if (m_cameraType == FLYBY_CAMERA)
	{
		m_centerOfProjection[0] = m_viewMatrix[0]*m_viewMatrix[12] + m_viewMatrix[1]*m_viewMatrix[13] + m_viewMatrix[2]*m_viewMatrix[14];
		m_centerOfProjection[1] = m_viewMatrix[4]*m_viewMatrix[12] + m_viewMatrix[5]*m_viewMatrix[13] + m_viewMatrix[6]*m_viewMatrix[14];
		m_centerOfProjection[2] = m_viewMatrix[8]*m_viewMatrix[12] + m_viewMatrix[9]*m_viewMatrix[13] + m_viewMatrix[10]*m_viewMatrix[14];

		glTranslatef(-m_centerOfProjection[0], -m_centerOfProjection[1], -m_centerOfProjection[2]);

		glRotatef(m_pitchRotation, m_viewMatrix[0], m_viewMatrix[4], m_viewMatrix[8]);
		glRotatef(m_yawRotation, 0.0f, 1.0f, 0.0f);

		m_centerOfProjection[0] += m_sideVectorDisplacement*m_viewMatrix[0] + m_upVectorDisplacement*m_viewMatrix[1] + m_eyeVectorDisplacement*m_viewMatrix[2];
		m_centerOfProjection[1] += m_sideVectorDisplacement*m_viewMatrix[4] + m_upVectorDisplacement*m_viewMatrix[5] + m_eyeVectorDisplacement*m_viewMatrix[6];
		m_centerOfProjection[2] += m_sideVectorDisplacement*m_viewMatrix[8] + m_upVectorDisplacement*m_viewMatrix[9] + m_eyeVectorDisplacement*m_viewMatrix[10];

		glTranslatef(m_centerOfProjection[0], m_centerOfProjection[1], m_centerOfProjection[2]);
	}

	m_pitchRotation = 0.0f;
	m_yawRotation = 0.0f;

	m_eyeVectorDisplacement = 0.0f;
	m_sideVectorDisplacement = 0.0f;
	m_upVectorDisplacement = 0.0f;

	glGetFloatv(GL_MODELVIEW_MATRIX, m_viewMatrix);
}

/**
*/
CPinholeCamera::ProjectionTypeEnum CPinholeCamera::GetProjectionType(void) const
{
	return m_projectionType;
}

/**
*/
void CPinholeCamera::SetCameraType(CameraTypeEnum type)
{
	m_cameraType = type;
}

/**
*/
CPinholeCamera::CameraTypeEnum CPinholeCamera::GetCameraType(void) const
{
	return m_cameraType;
}

/**
*/
void CPinholeCamera::ResetViewMatrix(void)
{
	m_viewMatrix[0] = 1.0f;
	m_viewMatrix[1] = 0.0f;
	m_viewMatrix[2] = 0.0f;
	m_viewMatrix[3] = 0.0f;
	m_viewMatrix[4] = 0.0f;
	m_viewMatrix[5] = 1.0f;
	m_viewMatrix[6] = 0.0f;
	m_viewMatrix[7] = 0.0f;
	m_viewMatrix[8] = 0.0f;
	m_viewMatrix[9] = 0.0f;
	m_viewMatrix[10] = 1.0f;
	m_viewMatrix[11] = 0.0f;
	m_viewMatrix[12] = 0.0f;
	m_viewMatrix[13] = 0.0f;
	m_viewMatrix[14] = 0.0f;
	m_viewMatrix[15] = 1.0f;
}

/**
*/
void CPinholeCamera::SetViewMatrix(const float *viewMatrix)
{
	m_viewMatrix[0] = viewMatrix[0];
	m_viewMatrix[1] = viewMatrix[1];
	m_viewMatrix[2] = viewMatrix[2];
	m_viewMatrix[3] = viewMatrix[3];
	m_viewMatrix[4] = viewMatrix[4];
	m_viewMatrix[5] = viewMatrix[5];
	m_viewMatrix[6] = viewMatrix[6];
	m_viewMatrix[7] = viewMatrix[7];
	m_viewMatrix[8] = viewMatrix[8];
	m_viewMatrix[9] = viewMatrix[9];
	m_viewMatrix[10] = viewMatrix[10];
	m_viewMatrix[11] = viewMatrix[11];
	m_viewMatrix[12] = viewMatrix[12];
	m_viewMatrix[13] = viewMatrix[13];
	m_viewMatrix[14] = viewMatrix[14];
	m_viewMatrix[15] = viewMatrix[15];
}

/**
*/
const float *CPinholeCamera::GetViewMatrix(void) const
{
	return m_viewMatrix;
}

/**
*/
const float *CPinholeCamera::GetProjectionMatrix(void) const
{
	return m_projectionMatrix;
}

/**
*/
const int *CPinholeCamera::GetViewport(void) const
{
	return m_viewport;
}

/**
*/
const float *CPinholeCamera::GetImagePlaneMatrix(void) const
{
	if (m_projectionType == PERSPECTIVE_PROJECTION)
		return m_imagePlaneMatrix;

	return 0;
}

/**
*/
const float *CPinholeCamera::GetCenterOfProjection(void) 
{
	if (m_projectionType == PERSPECTIVE_PROJECTION)
	{
		m_centerOfProjection[0] = m_viewMatrix[0]*m_viewMatrix[12] + m_viewMatrix[1]*m_viewMatrix[13] + m_viewMatrix[2]*m_viewMatrix[14];
		m_centerOfProjection[1] = m_viewMatrix[4]*m_viewMatrix[12] + m_viewMatrix[5]*m_viewMatrix[13] + m_viewMatrix[6]*m_viewMatrix[14];
		m_centerOfProjection[2] = m_viewMatrix[8]*m_viewMatrix[12] + m_viewMatrix[9]*m_viewMatrix[13] + m_viewMatrix[10]*m_viewMatrix[14];

		return m_centerOfProjection;
	}

	return 0;
}

/**
*/
//const bool CPinholeCamera::SaveTransformToFile(const std::string &fileName) const
//{
//	ofstream fileStream;
//	int i,
//		j;
//
//	fileStream.open(fileName.c_str());
//
//	if (!fileStream)
//	{
//		MarkError();
//
//		return false;
//	}
//
//	for (i=0; i<4; i++)
//	{
//		for (j=0; j<4; j++)
//			fileStream << m_viewMatrix[i*4 + j] << " ";
//
//		fileStream << endl;
//	}
//
//	for (i=0; i<4; i++)
//	{
//		for (j=0; j<4; j++)
//			fileStream << m_projectionMatrix[i*4 + j] << " ";
//
//		fileStream << endl;
//	}
//
//	return true;
//}
//
///**
//*/
//bool CPinholeCamera::LoadTransformFromFile(const std::string &fileName)
//{
//	ifstream fileStream;
//	int i,
//		j;
//
//	fileStream.open(fileName.c_str());
//
//	if (!fileStream)
//	{
//		MarkError();
//
//		return false;
//	}
//
//	for (i=0; i<4; i++)
//		for (j=0; j<4; j++)
//			fileStream >> m_viewMatrix[i*4 + j];
//
//	for (i=0; i<4; i++)
//		for (j=0; j<4; j++)
//			fileStream >> m_projectionMatrix[i*4 + j];
//
//	return true;
//}

/**
*/
void CPinholeCamera::Create(void)
{
	m_projectionType = PERSPECTIVE_PROJECTION;
	
	m_cameraType = TRACKBALL_CAMERA;  
	
	m_leftClippingPlaneCoord = -1.0f;
	m_rightClippingPlaneCoord = 1.0f;
	m_bottomClippingPlaneCoord = -1.0f;
	m_topClippingPlaneCoord = 1.0f;
	
	m_fieldOfView = 45.0f;
	
	m_nearPlaneDepth = 1.0f;
	m_farPlaneDepth = 10.0f;
	
	m_viewport[0] = 0;
	m_viewport[1] = 0;
	m_viewport[2] = 1;
	m_viewport[3] = 1;

	m_pitchRotation = 0.0f;
	m_yawRotation = 0.0f;
	
	m_eyeVectorDisplacement = 0.0f;
	m_sideVectorDisplacement = 0.0f;
	m_upVectorDisplacement = 0.0f;
	
	ResetProjectionMatrix();

	ResetViewMatrix();

	ResetImagePlaneMatrix();

	m_centerOfProjection[0] = 0.0f;
	m_centerOfProjection[1] = 0.0f;
	m_centerOfProjection[2] = 0.0f;
}

/**
*/
void CPinholeCamera::Copy(const CPinholeCamera &pinholeCamera)
{
	m_projectionType = pinholeCamera.m_projectionType;
	
	m_cameraType = pinholeCamera.m_cameraType;
	
	m_leftClippingPlaneCoord = pinholeCamera.m_leftClippingPlaneCoord;
	m_rightClippingPlaneCoord = pinholeCamera.m_rightClippingPlaneCoord;
	m_bottomClippingPlaneCoord = pinholeCamera.m_bottomClippingPlaneCoord;
	m_topClippingPlaneCoord = pinholeCamera.m_topClippingPlaneCoord;
	
	m_fieldOfView = pinholeCamera.m_fieldOfView;
	
	m_nearPlaneDepth = pinholeCamera.m_nearPlaneDepth;
	m_farPlaneDepth = pinholeCamera.m_farPlaneDepth;
	
	m_viewport[0] = pinholeCamera.m_viewport[0];
	m_viewport[1] = pinholeCamera.m_viewport[1];
	m_viewport[2] = pinholeCamera.m_viewport[2];
	m_viewport[3] = pinholeCamera.m_viewport[3];

	m_pitchRotation = pinholeCamera.m_pitchRotation;
	m_yawRotation = pinholeCamera.m_yawRotation;
	
	m_eyeVectorDisplacement = pinholeCamera.m_eyeVectorDisplacement;
	m_sideVectorDisplacement = pinholeCamera.m_sideVectorDisplacement;
	m_upVectorDisplacement = pinholeCamera.m_upVectorDisplacement;
	
	m_viewMatrix[0] = pinholeCamera.m_viewMatrix[0];
	m_viewMatrix[1] = pinholeCamera.m_viewMatrix[1];
	m_viewMatrix[2] = pinholeCamera.m_viewMatrix[2];
	m_viewMatrix[3] = pinholeCamera.m_viewMatrix[3];
	m_viewMatrix[4] = pinholeCamera.m_viewMatrix[4];
	m_viewMatrix[5] = pinholeCamera.m_viewMatrix[5];
	m_viewMatrix[6] = pinholeCamera.m_viewMatrix[6];
	m_viewMatrix[7] = pinholeCamera.m_viewMatrix[7];
	m_viewMatrix[8] = pinholeCamera.m_viewMatrix[8];
	m_viewMatrix[9] = pinholeCamera.m_viewMatrix[9];
	m_viewMatrix[10] = pinholeCamera.m_viewMatrix[10];
	m_viewMatrix[11] = pinholeCamera.m_viewMatrix[11];
	m_viewMatrix[12] = pinholeCamera.m_viewMatrix[12];
	m_viewMatrix[13] = pinholeCamera.m_viewMatrix[13];
	m_viewMatrix[14] = pinholeCamera.m_viewMatrix[14];
	m_viewMatrix[15] = pinholeCamera.m_viewMatrix[15];

	m_projectionMatrix[0] = pinholeCamera.m_projectionMatrix[0];
	m_projectionMatrix[1] = pinholeCamera.m_projectionMatrix[1];
	m_projectionMatrix[2] = pinholeCamera.m_projectionMatrix[2];
	m_projectionMatrix[3] = pinholeCamera.m_projectionMatrix[3];
	m_projectionMatrix[4] = pinholeCamera.m_projectionMatrix[4];
	m_projectionMatrix[5] = pinholeCamera.m_projectionMatrix[5];
	m_projectionMatrix[6] = pinholeCamera.m_projectionMatrix[6];
	m_projectionMatrix[7] = pinholeCamera.m_projectionMatrix[7];
	m_projectionMatrix[8] = pinholeCamera.m_projectionMatrix[8];
	m_projectionMatrix[9] = pinholeCamera.m_projectionMatrix[9];
	m_projectionMatrix[10] = pinholeCamera.m_projectionMatrix[10];
	m_projectionMatrix[11] = pinholeCamera.m_projectionMatrix[11];
	m_projectionMatrix[12] = pinholeCamera.m_projectionMatrix[12];
	m_projectionMatrix[13] = pinholeCamera.m_projectionMatrix[13];
	m_projectionMatrix[14] = pinholeCamera.m_projectionMatrix[14];
	m_projectionMatrix[15] = pinholeCamera.m_projectionMatrix[15];

	m_imagePlaneMatrix[0] = pinholeCamera.m_imagePlaneMatrix[0];
	m_imagePlaneMatrix[1] = pinholeCamera.m_imagePlaneMatrix[1];
	m_imagePlaneMatrix[2] = pinholeCamera.m_imagePlaneMatrix[2];
	m_imagePlaneMatrix[3] = pinholeCamera.m_imagePlaneMatrix[3];
	m_imagePlaneMatrix[4] = pinholeCamera.m_imagePlaneMatrix[4];
	m_imagePlaneMatrix[5] = pinholeCamera.m_imagePlaneMatrix[5];
	m_imagePlaneMatrix[6] = pinholeCamera.m_imagePlaneMatrix[6];
	m_imagePlaneMatrix[7] = pinholeCamera.m_imagePlaneMatrix[7];
	m_imagePlaneMatrix[8] = pinholeCamera.m_imagePlaneMatrix[8];

	m_centerOfProjection[0] = pinholeCamera.m_centerOfProjection[0];
	m_centerOfProjection[1] = pinholeCamera.m_centerOfProjection[1];
	m_centerOfProjection[2] = pinholeCamera.m_centerOfProjection[2];
}

/**
*/
void CPinholeCamera::ResetProjectionMatrix(void)
{
	m_projectionMatrix[0] = 1.0f;
	m_projectionMatrix[1] = 0.0f;
	m_projectionMatrix[2] = 0.0f;
	m_projectionMatrix[3] = 0.0f;
	m_projectionMatrix[4] = 0.0f;
	m_projectionMatrix[5] = 1.0f;
	m_projectionMatrix[6] = 0.0f;
	m_projectionMatrix[7] = 0.0f;
	m_projectionMatrix[8] = 0.0f;
	m_projectionMatrix[9] = 0.0f;
	m_projectionMatrix[10] = 1.0f;
	m_projectionMatrix[11] = 0.0f;
	m_projectionMatrix[12] = 0.0f;
	m_projectionMatrix[13] = 0.0f;
	m_projectionMatrix[14] = 0.0f;
	m_projectionMatrix[15] = 1.0f;
}

/**
*/
void CPinholeCamera::CalculateProjectionMatrix(void)
{
	int viewportWidth,
		viewportHeight;
	float viewportAspect,
		nearPlaneWidth,
		nearPlaneHeight,
		nearPlaneHorizontalMidpoint;

	glMatrixMode(GL_PROJECTION);

	glPushMatrix();

	glLoadIdentity();

	viewportWidth = m_viewport[2] - m_viewport[0];
	viewportHeight = m_viewport[3] - m_viewport[1];

	viewportAspect = float(viewportWidth)/float(viewportHeight);

	if (m_projectionType == PERSPECTIVE_PROJECTION)
		gluPerspective(m_fieldOfView, viewportAspect, m_nearPlaneDepth, m_farPlaneDepth);
	else if (m_projectionType == ORTHOGONAL_PROJECTION)
	{
		nearPlaneHeight = m_topClippingPlaneCoord - m_bottomClippingPlaneCoord;
		nearPlaneWidth = nearPlaneHeight*viewportAspect;

		nearPlaneHorizontalMidpoint = 0.5f*(m_leftClippingPlaneCoord + m_rightClippingPlaneCoord);

		glOrtho(nearPlaneHorizontalMidpoint - 0.5f*nearPlaneWidth, nearPlaneHorizontalMidpoint + 0.5f*nearPlaneWidth, m_bottomClippingPlaneCoord, m_topClippingPlaneCoord, m_nearPlaneDepth, m_farPlaneDepth);
	}

	glGetFloatv(GL_PROJECTION_MATRIX, m_projectionMatrix);

	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
}

/**
*/
void CPinholeCamera::ResetImagePlaneMatrix(void)
{
	m_imagePlaneMatrix[0] = 1.0f;
	m_imagePlaneMatrix[1] = 0.0f;
	m_imagePlaneMatrix[2] = 0.0f;
	m_imagePlaneMatrix[3] = 0.0f;
	m_imagePlaneMatrix[4] = 1.0f;
	m_imagePlaneMatrix[5] = 0.0f;
	m_imagePlaneMatrix[6] = 0.0f;
	m_imagePlaneMatrix[7] = 0.0f;
	m_imagePlaneMatrix[8] = 1.0f;
}

/**
*/
void CPinholeCamera::CalculateImagePlaneMatrix(void)
{
	int windowWidth,
		windowHeight;
	float w,
		h,
		as,
		bs;
	
	windowWidth = m_viewport[2] - m_viewport[0];
	windowHeight = m_viewport[3] - m_viewport[1];

	if (windowWidth > windowHeight)
	{
		w = 2.0f*m_nearPlaneDepth*(float)tan(m_fieldOfView*0.00872664625);
		h = ((float)windowHeight/windowWidth)*w;
	}
	else
	{
		h = 2.0f*m_nearPlaneDepth*(float)tan(m_fieldOfView*0.00872664625);
		w = ((float)windowWidth/windowHeight)*h;
	}
	
	as = w/windowWidth;
	
	m_imagePlaneMatrix[0] = m_viewMatrix[0]*as;
	m_imagePlaneMatrix[1] = m_viewMatrix[4]*as;
	m_imagePlaneMatrix[2] = m_viewMatrix[8]*as;
	
	bs = h/windowHeight;
	
	m_imagePlaneMatrix[3] = -m_viewMatrix[1]*bs;
	m_imagePlaneMatrix[4] = -m_viewMatrix[5]*bs;
	m_imagePlaneMatrix[5] = -m_viewMatrix[9]*bs;
	
	m_imagePlaneMatrix[6] = m_viewMatrix[2]*m_nearPlaneDepth - 0.5f*(m_imagePlaneMatrix[0]*windowWidth + m_imagePlaneMatrix[3]*windowHeight);
	m_imagePlaneMatrix[7] = m_viewMatrix[6]*m_nearPlaneDepth - 0.5f*(m_imagePlaneMatrix[1]*windowWidth + m_imagePlaneMatrix[4]*windowHeight);
	m_imagePlaneMatrix[8] = m_viewMatrix[10]*m_nearPlaneDepth - 0.5f*(m_imagePlaneMatrix[2]*windowWidth + m_imagePlaneMatrix[5]*windowHeight);
}

/**
*/
bool CPinholeCamera::UpdateProjection(float leftClippingPlaneCoord, float rightClippingPlaneCoord, float bottomClippingPlaneCoord, float topClippingPlaneCoord, float nearPlaneDepth, float farPlaneDepth)
{
	if (m_projectionType != ORTHOGONAL_PROJECTION)
	{
		MarkError();

		return false;
	}

	m_leftClippingPlaneCoord = leftClippingPlaneCoord;
	m_rightClippingPlaneCoord = rightClippingPlaneCoord;
	m_bottomClippingPlaneCoord = bottomClippingPlaneCoord;
	m_topClippingPlaneCoord = topClippingPlaneCoord;

	m_nearPlaneDepth = nearPlaneDepth;
	m_farPlaneDepth = farPlaneDepth;

	CalculateProjectionMatrix();

	return true;
}

/**
*/
bool CPinholeCamera::UpdateProjection(float fieldOfView, float nearPlaneDepth, float farPlaneDepth)
{
	if (m_projectionType != PERSPECTIVE_PROJECTION)
	{
		MarkError();

		return false;
	}

	m_fieldOfView = fieldOfView;
	
	m_nearPlaneDepth = nearPlaneDepth;
	m_farPlaneDepth = farPlaneDepth;

	CalculateProjectionMatrix();

	CalculateImagePlaneMatrix();

	return true;
}

