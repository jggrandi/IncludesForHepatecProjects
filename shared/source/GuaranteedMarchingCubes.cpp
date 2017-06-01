#include <cmath>

#include <ErrorManager.h>
#include <GuaranteedMarchingCubes.h>

#include "LookUpTable.h"

using namespace std;

/**
*/
CGuaranteedMarchingCubes::CGuaranteedMarchingCubes(void)
{
	// Move to a new 'Create' function?
	SetupVertexPool();
}

/**
*/
CGuaranteedMarchingCubes::~CGuaranteedMarchingCubes(void)
{
	// I must be careful with the destruction of this object. It is possible 
	// that the object is sharing the triangle mesh with other objects, and thus 
	// I don't have to clear it.
	//DeleteMesh();
}

/**
*/
bool CGuaranteedMarchingCubes::GenerateMesh(float isoLevel)
{
	int effectiveWidth,
		effectiveHeight,
		effectiveDepth;
	float xCoord,
		yCoord,
		zCoord;

	if (!m_image.get())
	{ 
		MarkError();

		return false;
	}

	DeleteMesh();

	m_isoLevel = isoLevel;

	if (!m_gridWidth || !m_gridHeight || !m_gridDepth)
		if (!SetGridSize(m_image->GetWidth(), m_image->GetHeight(), m_image->GetDepth()))
		{
			MarkError();

			return false;
		}

	init_temps();

	effectiveWidth = m_gridWidth - 1;
	effectiveHeight = m_gridHeight - 1;
	effectiveDepth = m_gridDepth - 1;

	compute_intersection_points(isoLevel);

	for (_k=1; _k<effectiveDepth; _k++)
	{
		zCoord = _k*m_cellDepth;

		for (_j=1; _j<effectiveHeight; _j++)
		{
			yCoord = _j*m_cellHeight;

			for (_i=1; _i<effectiveWidth; _i++) 
			{
				xCoord = _i*m_cellWidth;

				_lut_entry = 0;

				if ((_cube[0] = m_image->GetValue(xCoord, yCoord, zCoord) - isoLevel) > 0.0f) 
					_lut_entry += 1;
				if ((_cube[1] = m_image->GetValue(xCoord + m_cellWidth, yCoord, zCoord) - isoLevel) > 0.0f) 
					_lut_entry += 2;
				if ((_cube[2] = m_image->GetValue(xCoord + m_cellWidth, yCoord + m_cellHeight, zCoord) - isoLevel) > 0.0f) 
					_lut_entry += 4;
				if ((_cube[3] = m_image->GetValue(xCoord, yCoord + m_cellHeight, zCoord) - isoLevel) > 0.0f) 
					_lut_entry += 8;
				if ((_cube[4] = m_image->GetValue(xCoord, yCoord, zCoord + m_cellDepth) - isoLevel) > 0.0f) 
					_lut_entry += 16;
				if ((_cube[5] = m_image->GetValue(xCoord + m_cellWidth, yCoord, zCoord + m_cellDepth) - isoLevel) > 0.0f) 
					_lut_entry += 32;
				if ((_cube[6] = m_image->GetValue(xCoord + m_cellWidth, yCoord + m_cellHeight, zCoord + m_cellDepth) - isoLevel) > 0.0f) 
					_lut_entry += 64;
				if ((_cube[7] = m_image->GetValue(xCoord, yCoord + m_cellHeight, zCoord + m_cellDepth) - isoLevel) > 0.0f) 
					_lut_entry += 128;
				
				process_cube();
			}
		}
	}

	return true;
}

/**
*/
void CGuaranteedMarchingCubes::SetupVertexPool(void)
{
	m_triangleMesh->m_vertexSize = 3;

	m_triangleMesh->m_mode = CVertexPool::TRIANGLES;

	m_activeEdges->m_vertexSize = 3;

	m_activeEdges->m_mode = CVertexPool::LINES;
}

/**
Init temporary structures (must set sizes before call).
*/
void CGuaranteedMarchingCubes::init_temps(void)
{
	_x_verts.assign(m_gridWidth*m_gridHeight*m_gridDepth, -1);
	_y_verts.assign(m_gridWidth*m_gridHeight*m_gridDepth, -1);
	_z_verts.assign(m_gridWidth*m_gridHeight*m_gridDepth, -1);
}

/**
Compute the intersection points.
*/
void CGuaranteedMarchingCubes::compute_intersection_points(float isoLevel)
//-----------------------------------------------------------------------------
{
	int effectiveWidth,
		effectiveHeight,
		effectiveDepth;
	float xCoord,
		yCoord,
		zCoord;

	effectiveWidth = m_gridWidth - 1;
	effectiveHeight = m_gridHeight - 1;
	effectiveDepth = m_gridDepth - 1;

	for (_k=1; _k<effectiveDepth; _k++)
	{
		zCoord = _k*m_cellDepth;

		for (_j=1; _j<effectiveHeight; _j++)
		{
			yCoord = _j*m_cellHeight;

			for (_i=1; _i<effectiveWidth; _i++) 
			{
				xCoord = _i*m_cellWidth;

				_cube[0] = m_image->GetValue(xCoord, yCoord, zCoord) - isoLevel;
				
				if (_i < (effectiveWidth - 1))
					_cube[1] = m_image->GetValue(xCoord + m_cellWidth, yCoord , zCoord) - isoLevel;
				else
					_cube[1] = _cube[0];
				
				if (_j < (effectiveHeight - 1)) 
					_cube[3] = m_image->GetValue(xCoord, yCoord + m_cellHeight, zCoord) - isoLevel;
				else
					_cube[3] = _cube[0];
				
				if (_k< (effectiveDepth - 1)) 
					_cube[4] = m_image->GetValue(xCoord, yCoord, zCoord + m_cellDepth) - isoLevel;
				else
					_cube[4] = _cube[0];
				
				if (fabs(_cube[0]) < FLT_EPSILON) 
					_cube[0] = FLT_EPSILON;
				if (fabs(_cube[1]) < FLT_EPSILON) 
					_cube[1] = FLT_EPSILON;
				if (fabs(_cube[3]) < FLT_EPSILON) 
					_cube[3] = FLT_EPSILON;
				if (fabs(_cube[4]) < FLT_EPSILON) 
					_cube[4] = FLT_EPSILON;
				
				if (_cube[0] < 0)
				{
					if (_cube[1] > 0) 
						set_x_vert(add_x_vertex(), _i, _j, _k);
					if (_cube[3] > 0) 
						set_y_vert(add_y_vertex(), _i, _j, _k);
					if (_cube[4] > 0) 
						set_z_vert(add_z_vertex(), _i, _j, _k);
				}
				else
				{
					if (_cube[1] < 0)
						set_x_vert(add_x_vertex(), _i, _j, _k);
					if (_cube[3] < 0) 
						set_y_vert(add_y_vertex(), _i, _j, _k);
					if (_cube[4] < 0) 
						set_z_vert(add_z_vertex(), _i, _j, _k);
				}
			}
		}
	}
}

/**
* sets the pre-computed vertex index on the lower horizontal edge of a specific cube
* \param val the index of the new vertex
* \param i abscisse of the cube
* \param j ordinate of the cube
* \param k height of the cube
*/
void CGuaranteedMarchingCubes::set_x_vert(const int val, const int i, const int j, const int k) 
{ 
	_x_verts[i + j*m_gridWidth + k*m_gridWidth*m_gridHeight] = val;
}

/**
* sets the pre-computed vertex index on the lower longitudinal edge of a specific cube
* \param val the index of the new vertex
* \param i abscisse of the cube
* \param j ordinate of the cube
* \param k height of the cube
*/
void CGuaranteedMarchingCubes::set_y_vert(const int val, const int i, const int j, const int k) 
{ 
	_y_verts[i + j*m_gridWidth + k*m_gridWidth*m_gridHeight] = val;
}

/**
* sets the pre-computed vertex index on the lower vertical edge of a specific cube
* \param val the index of the new vertex
* \param i abscisse of the cube
* \param j ordinate of the cube
* \param k height of the cube
*/
void CGuaranteedMarchingCubes::set_z_vert(const int val, const int i, const int j, const int k) 
{ 
	_z_verts[i + j*m_gridWidth + k*m_gridWidth*m_gridHeight] = val;
}

/**
*/
int CGuaranteedMarchingCubes::add_x_vertex(void)
{
	float orgCoord[3],
		destCoord[3];

	orgCoord[0] = destCoord[0] = (float)_i*m_cellWidth;
	orgCoord[1] = destCoord[1] = (float)_j*m_cellHeight;
	orgCoord[2] = destCoord[2] = (float)_k*m_cellDepth;

	orgCoord[0] += m_cellWidth;

	TransformEdge(orgCoord, destCoord);

	return FindIntersection(orgCoord, destCoord);
}

/**
*/
int CGuaranteedMarchingCubes::add_y_vertex(void)
{
	float orgCoord[3],
		destCoord[3];

	orgCoord[0] = destCoord[0] = (float)_i*m_cellWidth;
	orgCoord[1] = destCoord[1] = (float)_j*m_cellHeight;
	orgCoord[2] = destCoord[2] = (float)_k*m_cellDepth;

	orgCoord[1] += m_cellHeight;

	TransformEdge(orgCoord, destCoord);

	return FindIntersection(orgCoord, destCoord);
}

/**
*/
int CGuaranteedMarchingCubes::add_z_vertex(void)
{
	float orgCoord[3],
		destCoord[3];

	orgCoord[0] = destCoord[0] = (float)_i*m_cellWidth;
	orgCoord[1] = destCoord[1] = (float)_j*m_cellHeight;
	orgCoord[2] = destCoord[2] = (float)_k*m_cellDepth;

	orgCoord[2] += m_cellDepth;

	TransformEdge(orgCoord, destCoord);

	return FindIntersection(orgCoord, destCoord);
}

/**
*/
int CGuaranteedMarchingCubes::add_c_vertex(void)
{
	float u,
		x, 
		y, 
		z,
		nx,
		ny,
		nz;
	int vid,
		vertexIndex;
	const float *vertexCoord,
		*normalCoord;

	u = 0.0f;

	x = 0.0f;
	y = 0.0f;
	z = 0.0f;

	nx = 0.0f;
	ny = 0.0f;
	nz = 0.0f;

	// Computes the average of the intersection points of the cube.
	vid = get_x_vert(_i, _j, _k);

	if (vid != -1) 
	{ 
		++u; 

		vertexCoord = m_triangleMesh->GetVertex(vid);
		normalCoord = m_triangleMesh->GetNormal(vid);

		x += *vertexCoord++;
		y += *vertexCoord++;
		z += *vertexCoord;

		nx += *normalCoord++; 
		ny += *normalCoord++; 
		nz += *normalCoord; 
	}

	vid = get_y_vert(_i + 1, _j, _k);

	if (vid != -1) 
	{ 
		++u; 

		vertexCoord = m_triangleMesh->GetVertex(vid);
		normalCoord = m_triangleMesh->GetNormal(vid);

		x += *vertexCoord++;
		y += *vertexCoord++;
		z += *vertexCoord;

		nx += *normalCoord++; 
		ny += *normalCoord++; 
		nz += *normalCoord; 
	}

	vid = get_x_vert(_i, _j + 1, _k);

	if (vid != -1) 
	{ 
		++u; 

		vertexCoord = m_triangleMesh->GetVertex(vid);
		normalCoord = m_triangleMesh->GetNormal(vid);

		x += *vertexCoord++;
		y += *vertexCoord++;
		z += *vertexCoord;

		nx += *normalCoord++; 
		ny += *normalCoord++; 
		nz += *normalCoord; 
	}

	vid = get_y_vert(_i, _j, _k);

	if (vid != -1) 
	{ 
		++u; 

		vertexCoord = m_triangleMesh->GetVertex(vid);
		normalCoord = m_triangleMesh->GetNormal(vid);

		x += *vertexCoord++;
		y += *vertexCoord++;
		z += *vertexCoord;

		nx += *normalCoord++; 
		ny += *normalCoord++; 
		nz += *normalCoord; 
	}

	vid = get_x_vert(_i, _j, _k+ 1);

	if (vid != -1) 
	{ 
		++u; 

		vertexCoord = m_triangleMesh->GetVertex(vid);
		normalCoord = m_triangleMesh->GetNormal(vid);

		x += *vertexCoord++;
		y += *vertexCoord++;
		z += *vertexCoord;

		nx += *normalCoord++; 
		ny += *normalCoord++; 
		nz += *normalCoord; 
	}

	vid = get_y_vert(_i + 1, _j, _k+ 1);

	if (vid != -1) 
	{ 
		++u; 

		vertexCoord = m_triangleMesh->GetVertex(vid);
		normalCoord = m_triangleMesh->GetNormal(vid);

		x += *vertexCoord++;
		y += *vertexCoord++;
		z += *vertexCoord;

		nx += *normalCoord++; 
		ny += *normalCoord++; 
		nz += *normalCoord; 
	}

	vid = get_x_vert(_i, _j + 1, _k+ 1);

	if (vid != -1) 
	{ 
		++u; 

		vertexCoord = m_triangleMesh->GetVertex(vid);
		normalCoord = m_triangleMesh->GetNormal(vid);

		x += *vertexCoord++;
		y += *vertexCoord++;
		z += *vertexCoord;

		nx += *normalCoord++; 
		ny += *normalCoord++; 
		nz += *normalCoord; 
	}

	vid = get_y_vert(_i, _j, _k+ 1);

	if (vid != -1) 
	{ 
		++u; 

		vertexCoord = m_triangleMesh->GetVertex(vid);
		normalCoord = m_triangleMesh->GetNormal(vid);

		x += *vertexCoord++;
		y += *vertexCoord++;
		z += *vertexCoord;

		nx += *normalCoord++; 
		ny += *normalCoord++; 
		nz += *normalCoord; 
	}

	vid = get_z_vert(_i, _j, _k);

	if (vid != -1) 
	{ 
		++u; 

		vertexCoord = m_triangleMesh->GetVertex(vid);
		normalCoord = m_triangleMesh->GetNormal(vid);

		x += *vertexCoord++;
		y += *vertexCoord++;
		z += *vertexCoord;

		nx += *normalCoord++; 
		ny += *normalCoord++; 
		nz += *normalCoord; 
	}

	vid = get_z_vert(_i + 1, _j, _k);

	if (vid != -1) 
	{ 
		++u; 

		vertexCoord = m_triangleMesh->GetVertex(vid);
		normalCoord = m_triangleMesh->GetNormal(vid);

		x += *vertexCoord++;
		y += *vertexCoord++;
		z += *vertexCoord;

		nx += *normalCoord++; 
		ny += *normalCoord++; 
		nz += *normalCoord; 
	}

	vid = get_z_vert(_i + 1, _j + 1, _k);

	if (vid != -1) 
	{ 
		++u; 

		vertexCoord = m_triangleMesh->GetVertex(vid);
		normalCoord = m_triangleMesh->GetNormal(vid);

		x += *vertexCoord++;
		y += *vertexCoord++;
		z += *vertexCoord;

		nx += *normalCoord++; 
		ny += *normalCoord++; 
		nz += *normalCoord; 
	}

	vid = get_z_vert(_i, _j + 1, _k);

	if (vid != -1) 
	{ 
		++u; 

		vertexCoord = m_triangleMesh->GetVertex(vid);
		normalCoord = m_triangleMesh->GetNormal(vid);

		x += *vertexCoord++;
		y += *vertexCoord++;
		z += *vertexCoord;

		nx += *normalCoord++; 
		ny += *normalCoord++; 
		nz += *normalCoord; 
	}

	x /= u;
	y /= u;
	z /= u;

	u = sqrtf(nx*nx + ny*ny + nz*nz);
	
	if (u > 0)
	{
		nx /= u;
		ny /= u;
		nz /= u;
	}

	vertexIndex = m_triangleMesh->GetVertexCount();

	m_triangleMesh->AddVertex(x, y, z);
	m_triangleMesh->AddNormal(nx, ny, nz);

	return vertexIndex;
}

/**
* accesses the pre-computed vertex index on the lower horizontal edge of a specific cube
* \param i abscisse of the cube
* \param j ordinate of the cube
* \param k height of the cube
*/
int CGuaranteedMarchingCubes::get_x_vert(const int i, const int j, const int k) const 
{ 
	return _x_verts[i + j*m_gridWidth + k*m_gridWidth*m_gridHeight] ; 
}

/**
* accesses the pre-computed vertex index on the lower longitudinal edge of a specific cube
* \param i abscisse of the cube
* \param j ordinate of the cube
* \param k height of the cube
*/
int CGuaranteedMarchingCubes::get_y_vert(const int i, const int j, const int k) const 
{ 
	return _y_verts[i + j*m_gridWidth + k*m_gridWidth*m_gridHeight] ; 
}

/**
* accesses the pre-computed vertex index on the lower vertical edge of a specific cube
* \param i abscisse of the cube
* \param j ordinate of the cube
* \param k height of the cube
*/
int CGuaranteedMarchingCubes::get_z_vert(const int i, const int j, const int k) const 
{ 
	return _z_verts[i + j*m_gridWidth + k*m_gridWidth*m_gridHeight] ; 
}

/**
Process a unit cube.
*/
void CGuaranteedMarchingCubes::process_cube(void)
{
	int v12;

	v12 = -1;

	_case = cases[_lut_entry][0] ;
	_config = cases[_lut_entry][1] ;
	_subconfig = 0;

	switch (_case)
	{
	case 0:
		break;
	case 1:
		add_triangle(tiling1[_config], 1);
		break;
	case 2:
		add_triangle(tiling2[_config], 2);
		break;
	case 3:
		if (test_face(test3[_config]))
			add_triangle(tiling3_2[_config], 4); // 3.2
		else
			add_triangle(tiling3_1[_config], 2); // 3.1
		break;
	case 4:
		if (test_interior(test4[_config]))
			add_triangle(tiling4_1[_config], 2); // 4.1.1
		else
			add_triangle(tiling4_2[_config], 6); // 4.1.2
		break;
	case 5:
		add_triangle(tiling5[_config], 3);
		break;
	case 6:
		if (test_face(test6[_config][0]))
			add_triangle(tiling6_2[_config], 5); // 6.2
		else
		{
			if (test_interior(test6[_config][1]))
				add_triangle(tiling6_1_1[_config], 3); // 6.1.1
			else
				add_triangle(tiling6_1_2[_config], 7); // 6.1.2
		}
		break;
	case 7:
		if (test_face(test7[_config][0])) 
			_subconfig +=  1 ;
		if (test_face(test7[_config][1])) 
			_subconfig +=  2 ;
		if (test_face(test7[_config][2])) 
			_subconfig +=  4 ;
		
		switch (_subconfig)
		{
		case 0:
			add_triangle(tiling7_1[_config], 3); 
			break;
		case 1:
			add_triangle(tiling7_2[_config][0], 5); 
			break;
		case 2:
			add_triangle(tiling7_2[_config][1], 5); 
			break;
		case 3:
			v12 = add_c_vertex();
			
			add_triangle(tiling7_3[_config][0], 9, v12); 
			break;
		case 4:
			add_triangle(tiling7_2[_config][2], 5); 
			break;
		case 5:
			v12 = add_c_vertex();
			
			add_triangle(tiling7_3[_config][1], 9, v12); 
			break;
		case 6:
			v12 = add_c_vertex();
			
			add_triangle(tiling7_3[_config][2], 9, v12); 
			break;
		case 7:
			if (test_interior(test7[_config][3]))
				add_triangle(tiling7_4_2[_config], 9);
			else
				add_triangle(tiling7_4_1[_config], 5);
			break;
		};
		break;
	case 8:
		add_triangle(tiling8[_config], 2);
		break;
	case 9:
		add_triangle(tiling9[_config], 4);
		break;
	case 10:
		if (test_face(test10[_config][0]))
		{
			if (test_face(test10[_config][1]))
				add_triangle(tiling10_1_1_[_config], 4); // 10.1.1
			else
			{
				v12 = add_c_vertex();

				add_triangle(tiling10_2[_config], 8, v12); // 10.2
			}
		}
		else
		{
			if (test_face(test10[_config][1]))
			{
				v12 = add_c_vertex();
				
				add_triangle(tiling10_2_[_config], 8, v12); // 10.2
			}
			else
			{
				if (test_interior(test10[_config][2]))
					add_triangle(tiling10_1_1[_config], 4); // 10.1.1
				else
					add_triangle(tiling10_1_2[_config], 8); // 10.1.2
			}
		}
		break;
	case 11:
		add_triangle(tiling11[_config], 4);
		break;
	case 12:
		if (test_face(test12[_config][0]))
		{
			if (test_face(test12[_config][1]))
				add_triangle(tiling12_1_1_[_config], 4); // 12.1.1
			else
			{
				v12 = add_c_vertex();

				add_triangle(tiling12_2[_config], 8, v12); // 12.2
			}
		}
		else
		{
			if (test_face(test12[_config][1]))
			{
				v12 = add_c_vertex();

				add_triangle(tiling12_2_[_config], 8, v12); // 12.2
			}
			else
			{
				if (test_interior(test12[_config][2]))
					add_triangle(tiling12_1_1[_config], 4); // 12.1.1
				else
					add_triangle(tiling12_1_2[_config], 8); // 12.1.2
			}
		}
		break;

	case 13:
		if (test_face(test13[_config][0])) 
			_subconfig +=  1 ;
		if (test_face(test13[_config][1])) 
			_subconfig +=  2 ;
		if (test_face(test13[_config][2])) 
			_subconfig +=  4 ;
		if (test_face(test13[_config][3])) 
			_subconfig +=  8 ;
		if (test_face(test13[_config][4])) 
			_subconfig += 16 ;
		if (test_face(test13[_config][5])) 
			_subconfig += 32 ;
		
		switch (subconfig13[_subconfig])
		{
		case 0:/* 13.1 */
			add_triangle(tiling13_1[_config], 4); 
			break;
		case 1:/* 13.2 */
			add_triangle(tiling13_2[_config][0], 6); 
			break;
		case 2:/* 13.2 */
			add_triangle(tiling13_2[_config][1], 6); 
			break;
		case 3:/* 13.2 */
			add_triangle(tiling13_2[_config][2], 6); 
			break;
		case 4:/* 13.2 */
			add_triangle(tiling13_2[_config][3], 6); 
			break;
		case 5:/* 13.2 */
			add_triangle(tiling13_2[_config][4], 6); 
			break;
		case 6:/* 13.2 */
			add_triangle(tiling13_2[_config][5], 6); 
			break;
		case 7:/* 13.3 */
			v12 = add_c_vertex();

			add_triangle(tiling13_3[_config][0], 10, v12); 
			break;
		case 8:/* 13.3 */
			v12 = add_c_vertex();
			
			add_triangle(tiling13_3[_config][1], 10, v12); 
			break;
		case 9:/* 13.3 */
			v12 = add_c_vertex();
			
			add_triangle(tiling13_3[_config][2], 10, v12); 
			break;
		case 10:/* 13.3 */
			v12 = add_c_vertex();
			
			add_triangle(tiling13_3[_config][3], 10, v12); 
			break;
		case 11:/* 13.3 */
			v12 = add_c_vertex();
			
			add_triangle(tiling13_3[_config][4], 10, v12); 
			break;
		case 12:/* 13.3 */
			v12 = add_c_vertex();
			
			add_triangle(tiling13_3[_config][5], 10, v12); 
			break;
		case 13:/* 13.3 */
			v12 = add_c_vertex();
			
			add_triangle(tiling13_3[_config][6], 10, v12); 
			break;
		case 14:/* 13.3 */
			v12 = add_c_vertex();
			
			add_triangle(tiling13_3[_config][7], 10, v12); 
			break;
		case 15:/* 13.3 */
			v12 = add_c_vertex();
			
			add_triangle(tiling13_3[_config][8], 10, v12); 
			break;
		case 16:/* 13.3 */
			v12 = add_c_vertex();
			
			add_triangle(tiling13_3[_config][9], 10, v12); 
			break;
		case 17:/* 13.3 */
			v12 = add_c_vertex();
			
			add_triangle(tiling13_3[_config][10], 10, v12); 
			break;
		case 18:/* 13.3 */
			v12 = add_c_vertex();
			
			add_triangle(tiling13_3[_config][11], 10, v12); 
			break;
		case 19:/* 13.4 */
			v12 = add_c_vertex();

			add_triangle(tiling13_4[_config][0], 12, v12); 
			break;
		case 20:/* 13.4 */
			v12 = add_c_vertex();
			
			add_triangle(tiling13_4[_config][1], 12, v12); 
			break;
		case 21:/* 13.4 */
			v12 = add_c_vertex();
			
			add_triangle(tiling13_4[_config][2], 12, v12); 
			break;
		case 22:/* 13.4 */
			v12 = add_c_vertex();
			
			add_triangle(tiling13_4[_config][3], 12, v12); 
			break;
		case 23:/* 13.5 */
			_subconfig = 0;

			if (test_interior(test13[_config][6]))
				add_triangle(tiling13_5_1[_config][0], 6);
			else
				add_triangle(tiling13_5_2[_config][0], 10);
			break;
		case 24:/* 13.5 */
			_subconfig = 1 ;
			
			if (test_interior(test13[_config][6]))
				add_triangle(tiling13_5_1[_config][1], 6);
			else
				add_triangle(tiling13_5_2[_config][1], 10);
			break;
		case 25:/* 13.5 */
			_subconfig = 2 ;
			
			if (test_interior(test13[_config][6]))
				add_triangle(tiling13_5_1[_config][2], 6);
			else
				add_triangle(tiling13_5_2[_config][2], 10);
			break;
		case 26:/* 13.5 */
			_subconfig = 3 ;
			
			if (test_interior(test13[_config][6]))
				add_triangle(tiling13_5_1[_config][3], 6);
			else
				add_triangle(tiling13_5_2[_config][3], 10);
			break;
		case 27:/* 13.3 */
			v12 = add_c_vertex();

			add_triangle(tiling13_3_[_config][0], 10, v12); 
			break;
		case 28:/* 13.3 */
			v12 = add_c_vertex();
			
			add_triangle(tiling13_3_[_config][1], 10, v12); 
			break;
		case 29:/* 13.3 */
			v12 = add_c_vertex();
			
			add_triangle(tiling13_3_[_config][2], 10, v12); 
			break;
		case 30:/* 13.3 */
			v12 = add_c_vertex();
			
			add_triangle(tiling13_3_[_config][3], 10, v12); 
			break;
		case 31:/* 13.3 */
			v12 = add_c_vertex();
			
			add_triangle(tiling13_3_[_config][4], 10, v12); 
			break;
		case 32:/* 13.3 */
			v12 = add_c_vertex();
			
			add_triangle(tiling13_3_[_config][5], 10, v12); 
			break;
		case 33:/* 13.3 */
			v12 = add_c_vertex();
			
			add_triangle(tiling13_3_[_config][6], 10, v12); 
			break;
		case 34:/* 13.3 */
			v12 = add_c_vertex();
			
			add_triangle(tiling13_3_[_config][7], 10, v12); 
			break;
		case 35:/* 13.3 */
			v12 = add_c_vertex();
			
			add_triangle(tiling13_3_[_config][8], 10, v12); 
			break;
		case 36:/* 13.3 */
			v12 = add_c_vertex();
			
			add_triangle(tiling13_3_[_config][9], 10, v12); 
			break;
		case 37:/* 13.3 */
			v12 = add_c_vertex();
			
			add_triangle(tiling13_3_[_config][10], 10, v12); 
			break;
		case 38:/* 13.3 */
			v12 = add_c_vertex();
			
			add_triangle(tiling13_3_[_config][11], 10, v12); 
			break;
		case 39:/* 13.2 */
			add_triangle(tiling13_2_[_config][0], 6); 
			break;
		case 40:/* 13.2 */
			add_triangle(tiling13_2_[_config][1], 6); 
			break;
		case 41:/* 13.2 */
			add_triangle(tiling13_2_[_config][2], 6); 
			break;
		case 42:/* 13.2 */
			add_triangle(tiling13_2_[_config][3], 6); 
			break;
		case 43:/* 13.2 */
			add_triangle(tiling13_2_[_config][4], 6); 
			break;
		case 44:/* 13.2 */
			add_triangle(tiling13_2_[_config][5], 6); 
			break;
		case 45:/* 13.1 */
			add_triangle(tiling13_1_[_config], 4); 
			break;
		default:
			ReportError("Marching Cubes: Impossible case 13?\n");
		}
		break;
	case 14:
		add_triangle(tiling14[_config], 4);
		break;
	};
}

/**
Adding triangles.
*/
void CGuaranteedMarchingCubes::add_triangle(const char* trig, char n, int v12)
{
	int tv[3],
		t;
	
	for(t=0; t<3*n ; t++)
	{
		switch (trig[t])
		{
		case 0: 
			tv[t%3] = get_x_vert(_i, _j, _k); 
			break;
		case 1: 
			tv[t%3] = get_y_vert(_i + 1, _j, _k); 
			break;
		case 2: 
			tv[t%3] = get_x_vert(_i, _j + 1, _k); 
			break;
		case 3: 
			tv[t%3] = get_y_vert(_i, _j, _k); 
			break;
		case 4: 
			tv[t%3] = get_x_vert(_i, _j, _k + 1); 
			break;
		case 5: 
			tv[t%3] = get_y_vert(_i + 1, _j, _k + 1); 
			break;
		case 6: 
			tv[t%3] = get_x_vert(_i, _j + 1,_k + 1); 
			break;
		case 7: 
			tv[t%3] = get_y_vert(_i, _j, _k + 1); 
			break;
		case 8: 
			tv[t%3] = get_z_vert(_i, _j, _k); 
			break;
		case 9: 
			tv[t%3] = get_z_vert(_i + 1, _j, _k); 
			break;
		case 10: 
			tv[t%3] = get_z_vert(_i + 1, _j + 1, _k); 
			break;
		case 11: 
			tv[t%3] = get_z_vert(_i, _j + 1, _k); 
			break;
		case 12: 
			tv[t%3] = v12; 
			break;
		default: 
			break;
		}

		if (tv[t%3] == -1)
			//ReportError("Marching Cubes: invalid triangle");
			// TEST ONLY!
			return;

		if (t%3 == 2)
			m_triangleMesh->AddTriangle(tv[0], tv[1], tv[2]);
	}
}

/**
Test a face. If face>0 return true if the face contains a part of the surface.
*/
bool CGuaranteedMarchingCubes::test_face(char face)
{
	float A,
		B,
		C,
		D;

	switch (face)
	{
	case -1: 
	case 1:  
		A = _cube[0];
		B = _cube[4];
		C = _cube[5];
		D = _cube[1];
		break ;
	case -2: 
	case 2:  
		A = _cube[1];
		B = _cube[5];
		C = _cube[6];
		D = _cube[2];
		break ;
	case -3: 
	case 3:  
		A = _cube[2];
		B = _cube[6];
		C = _cube[7];
		D = _cube[3];
		break ;
	case -4: 
	case 4:  
		A = _cube[3];
		B = _cube[7];
		C = _cube[4];
		D = _cube[0];
		break ;
	case -5: 
	case 5:  
		A = _cube[0];
		B = _cube[3];
		C = _cube[2];
		D = _cube[1];
		break ;
	case -6: 
	case 6:  
		A = _cube[4];
		B = _cube[7];
		C = _cube[6];
		D = _cube[5];
		break ;
	default: 
		ReportError("Invalid face code.");
		A = B = C = D = 0;
	};

	if (fabs(A*C - B*D) < FLT_EPSILON)
		return face >= 0;
	
	// face and A invert signs.
	return (face*A*(A*C - B*D) >= 0);
}

/**
Test the interior of a cube.
if s == 7, return true  if the interior is empty.
if s ==-7, return false if the interior is empty.
*/
bool CGuaranteedMarchingCubes::test_interior(char s)
{
	float t, 
		At=0, 
		Bt=0, 
		Ct=0, 
		Dt=0, 
		a, 
		b;
	char test =  0,
		// reference edge of the triangulation.
		edge = -1; 

	At = 0.0f;
	Bt = 0.0f;
	Ct = 0.0f;
	Dt = 0.0f;

	test =  0;
	edge = -1; 

	switch (_case)
	{
	case 4:
	case 10:
		a = (_cube[4] - _cube[0])*(_cube[6] - _cube[2]) - (_cube[7] - _cube[3])*(_cube[5] - _cube[1]);
		b =  _cube[2]*(_cube[4] - _cube[0]) + _cube[0]*(_cube[6] - _cube[2]) - _cube[1]*(_cube[7] - _cube[3]) - _cube[3]*(_cube[5] - _cube[1]);
		t = -b/(2*a);

		if (t<0 || t>1) 
			return s>0;

		At = _cube[0] + (_cube[4] - _cube[0])*t;
		Bt = _cube[3] + (_cube[7] - _cube[3])*t;
		Ct = _cube[2] + (_cube[6] - _cube[2])*t;
		Dt = _cube[1] + (_cube[5] - _cube[1])*t;
		break ;
	case 6:
	case 7:
	case 12:
	case 13:
		switch (_case)
		{
		case 6: 
			edge = test6[_config][2];
			break;
		case 7: 
			edge = test7[_config][4]; 
			break;
		case 12: 
			edge = test12[_config][3];
			break;
		case 13: 
			edge = tiling13_5_1[_config][_subconfig][0]; 
			break;
		}

		switch (edge)
		{
		case 0:
			t = _cube[0]/(_cube[0] - _cube[1]);
			
			At = 0;
			Bt = _cube[3] + (_cube[2] - _cube[3])*t;
			Ct = _cube[7] + (_cube[6] - _cube[7])*t;
			Dt = _cube[4] + (_cube[5] - _cube[4])*t;
			break;
		case 1:
			t = _cube[1]/(_cube[1] - _cube[2]);
			
			At = 0;
			Bt = _cube[0] + (_cube[3] - _cube[0])*t;
			Ct = _cube[4] + (_cube[7] - _cube[4])*t;
			Dt = _cube[5] + (_cube[6] - _cube[5])*t;
			break ;
		case 2:
			t  = _cube[2]/(_cube[2] - _cube[3]);
			
			At = 0;
			Bt = _cube[1] + (_cube[0] - _cube[1])*t;
			Ct = _cube[5] + (_cube[4] - _cube[5])*t;
			Dt = _cube[6] + (_cube[7] - _cube[6])*t;
			break ;
		case 3:
			t = _cube[3]/(_cube[3] - _cube[0]);
			
			At = 0;
			Bt = _cube[2] + (_cube[1] - _cube[2])*t;
			Ct = _cube[6] + (_cube[5] - _cube[6])*t;
			Dt = _cube[7] + (_cube[4] - _cube[7])*t;
			break ;
		case 4:
			t  = _cube[4]/(_cube[4] - _cube[5]);
			
			At = 0;
			Bt = _cube[7] + (_cube[6] - _cube[7])*t;
			Ct = _cube[3] + (_cube[2] - _cube[3])*t;
			Dt = _cube[0] + (_cube[1] - _cube[0])*t;
			break ;
		case 5:
			t  = _cube[5]/(_cube[5] - _cube[6]);
			
			At = 0;
			Bt = _cube[4] + (_cube[7] - _cube[4])*t;
			Ct = _cube[0] + (_cube[3] - _cube[0])*t;
			Dt = _cube[1] + (_cube[2] - _cube[1])*t;
			break ;
		case 6:
			t  = _cube[6]/(_cube[6] - _cube[7]);
			
			At = 0;
			Bt = _cube[5] + (_cube[4] - _cube[5])*t;
			Ct = _cube[1] + (_cube[0] - _cube[1])*t;
			Dt = _cube[2] + (_cube[3] - _cube[2])*t;
			break ;
		case 7:
			t  = _cube[7]/(_cube[7] - _cube[4]);
			
			At = 0;
			Bt = _cube[6] + (_cube[5] - _cube[6])*t;
			Ct = _cube[2] + (_cube[1] - _cube[2])*t;
			Dt = _cube[3] + (_cube[0] - _cube[3])*t;
			break ;
		case 8:
			t  = _cube[0]/(_cube[0] - _cube[4]);
			
			At = 0;
			Bt = _cube[3] + (_cube[7] - _cube[3])*t;
			Ct = _cube[2] + (_cube[6] - _cube[2])*t;
			Dt = _cube[1] + (_cube[5] - _cube[1])*t;
			break ;
		case 9:
			t  = _cube[1]/(_cube[1] - _cube[5]);
			
			At = 0;
			Bt = _cube[0] + (_cube[4] - _cube[0])*t;
			Ct = _cube[3] + (_cube[7] - _cube[3])*t;
			Dt = _cube[2] + (_cube[6] - _cube[2])*t;
			break ;
		case 10:
			t  = _cube[2]/(_cube[2] - _cube[6]);
			
			At = 0;
			Bt = _cube[1] + (_cube[5] - _cube[1])*t;
			Ct = _cube[0] + (_cube[4] - _cube[0])*t;
			Dt = _cube[3] + (_cube[7] - _cube[3])*t;
			break ;
		case 11:
			t  = _cube[3]/(_cube[3] - _cube[7]);
			
			At = 0;
			Bt = _cube[2] + (_cube[6] - _cube[2])*t;
			Ct = _cube[1] + (_cube[5] - _cube[1])*t;
			Dt = _cube[0] + (_cube[4] - _cube[0])*t;
			break ;
		default: 
			ReportError("Invalid edge.");
			break;
		}
		break;
	default:
		ReportError("Invalid ambiguous case.");
		break;
	}

	if (At >= 0) 
		test++;
	if (Bt >= 0) 
		test += 2;
	if (Ct >= 0) 
		test += 4;
	if (Dt >= 0) 
		test += 8;
	switch (test)
	{
	case 0: 
		return s>0;
	case 1: 
		return s>0;
	case 2: 
		return s>0;
	case 3: 
		return s>0;
	case 4: 
		return s>0;
	case 5: 
		if (At*Ct - Bt*Dt <  FLT_EPSILON) 
			return s>0; 
		break;
	case 6: 
		return s>0;
	case 7: 
		return s<0;
	case 8: 
		return s>0;
	case 9: 
		return s>0;
	case 10: 
		if (At*Ct - Bt*Dt >= FLT_EPSILON) 
			return s>0; 
		break;
	case 11: 
		return s<0;
	case 12: 
		return s>0;
	case 13: 
		return s<0;
	case 14: 
		return s<0;
	case 15: 
		return s<0;
	}

	return s<0;
}

