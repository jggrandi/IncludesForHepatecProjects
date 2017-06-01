#ifndef GUARANTEEDMARCHINGCUBES_INCLUDED
#define GUARANTEEDMARCHINGCUBES_INCLUDED

#include <map>
#include <vector>

#include <IMarchingMethod.h>

class CGuaranteedMarchingCubes
	: public IMarchingMethod
{
public:
	CGuaranteedMarchingCubes(void);
	~CGuaranteedMarchingCubes(void);
	bool GenerateMesh(float isoLevel);
protected:
	void SetupVertexPool(void);
private:
	void init_temps(void);
	void compute_intersection_points(float isoLevel);
	inline void set_x_vert(const int val, const int i, const int j, const int k);
	inline void set_y_vert(const int val, const int i, const int j, const int k);
	inline void set_z_vert(const int val, const int i, const int j, const int k);
	/** adds a vertex on the current horizontal edge */
	int add_x_vertex(void);
	/** adds a vertex on the current longitudinal edge */
	int add_y_vertex(void);
	/** adds a vertex on the current vertical edge */
	int add_z_vertex(void);
	/** adds a vertex inside the current cube */
	int add_c_vertex(void);
	/**
	* accesses the pre-computed vertex index on the lower horizontal edge of a specific cube
	* \param i abscisse of the cube
	* \param j ordinate of the cube
	* \param k height of the cube
	*/
	inline int get_x_vert(const int i, const int j, const int k) const;
	/**
	* accesses the pre-computed vertex index on the lower longitudinal edge of a specific cube
	* \param i abscisse of the cube
	* \param j ordinate of the cube
	* \param k height of the cube
	*/
	inline int get_y_vert(const int i, const int j, const int k) const;
	/**
	* accesses the pre-computed vertex index on the lower vertical edge of a specific cube
	* \param i abscisse of the cube
	* \param j ordinate of the cube
	* \param k height of the cube
	*/
	inline int get_z_vert(const int i, const int j, const int k) const;
	/** tesselates one cube */
	void process_cube(void);
	/**
	* routine to add a triangle to the mesh
	* \param trig the code for the triangle as a sequence of edges index
	* \param n    the number of triangles to produce
	* \param v12  the index of the interior vertex to use, if necessary
	*/
	void add_triangle(const char* trig, char n, int v12 = -1);
	/** tests if the components of the tesselation of the cube should be connected by the interior of an ambiguous face */
	bool test_face(char face);
	/** tests if the components of the tesselation of the cube should be connected through the interior of the cube */
	bool test_interior(char s);
protected:
	/**< abscisse of the active cube */
	int _i;
	/**< height of the active cube */
	int _j;
	/**< ordinate of the active cube */
	int _k;
	// Values of the implicit function on the active cube.
	float _cube[8];
	// Cube sign representation in [0..255].
	unsigned char _lut_entry;
	/**< pre-computed vertex indices on the lower horizontal edge of each cube */
	std::vector<int> _x_verts;
	/**< pre-computed vertex indices on the lower longitudinal edge of each cube */
	std::vector<int> _y_verts;
	/**< pre-computed vertex indices on the lower vertical edge of each cube */
	std::vector<int> _z_verts;
	/**< case of the active cube in [0..15] */
	unsigned char _case;
	/**< configuration of the active cube */
	unsigned char _config;
	/**< subconfiguration of the active cube */
	unsigned char _subconfig;
};

#endif // GUARANTEEDMARCHINGCUBES_INCLUDED

