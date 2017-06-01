#ifndef MYMATH_INCLUDED
#define MYMATH_INCLUDED

#include <cmath>
#include <limits>

// Smallest such that ((1.0 + DBL_EPSILON) != 1.0).
#ifndef DBL_EPSILON
#define DBL_EPSILON 2.2204460492503131e-016
#endif // #ifndef DBL_EPSILON

namespace MyMath 
{
  /**
   */
  template <typename T>
  T Bias(T bias, T t)
  {
    return T(pow(t, T(log(bias))/T(log(T(0.5)))));
  }

  /**
   */
  template <typename T> 
  T Pi(void)
  {
    return T(3.1415926535897932384626433832795);
  }

  /**
   */
  template <typename T>
  T CosineInterpolate(T y0, T y1, T mu)
  {
    T mu2;
		
    mu2 = (T(1.0) - T(cos(mu*Pi<T>())))/T(2.0);
		
    return (y0*(T(1.0) - mu2) + y1*mu2);
  }

  /**
   */
  template <typename T>
  const T *CrossProduct(const T *u, const T *v, T *result)
  {
    result[0] = u[1]*v[2] - u[2]*v[1];
    result[1] = u[2]*v[0] - u[0]*v[2];
    result[2] = u[0]*v[1] - u[1]*v[0];

    return result;
  }

  /**
   */
  template <typename T> 
  T DegreesToRadians(T degrees)
  {
    return degrees*Pi<T>()/T(180.0);
  }

  /**
     3x3 matrix determinant.
  */
  template <typename T> 
  T Determinant(T m00, T m01, T m02, T m10, T m11, T m12, T m20, T m21, T m22)
  {
    T minorDeterminant[3],
      determinant;

    minorDeterminant[0] = m11*m12 - m21*m22;
    minorDeterminant[1] = m01*m02 - m21*m22;
    minorDeterminant[2] = m01*m02 - m11*m12;

    determinant = m00*minorDeterminant[0] - m10*minorDeterminant[1] + m20*minorDeterminant[2];

    return determinant;
  }

  /**
     4x4 matrix determinant.
  */
  template <typename T>
  float Determinant(T m00, T m01, T m02, T m03, T m10, T m11, T m12, T m13, T m20, T m21, T m22, T m23, T m30, T m31, T m32, T m33)
  {
    T minorDeterminant[4],
      determinant;

    minorDeterminant[0] = m11*(m22*m33 - m23*m32) - m12*(m21*m33 - m23*m31) + m13*(m21*m32 - m22*m31);
    minorDeterminant[1] = m10*(m22*m33 - m23*m32) - m12*(m20*m33 - m23*m30) + m13*(m20*m32 - m22*m30);
    minorDeterminant[2] = m10*(m21*m33 - m23*m31) - m11*(m20*m33 - m23*m30) + m13*(m20*m31 - m21*m30);
    minorDeterminant[3] = m10*(m21*m32 - m22*m31) - m11*(m20*m32 - m22*m30) + m12*(m20*m31 - m21*m30);

    determinant = m00*minorDeterminant[0] - m01*minorDeterminant[1] + m02*minorDeterminant[2] - m03*minorDeterminant[3];

    return determinant;
  }

  /**
   */
  template <typename T, int dimension>
  T DotProduct(const T *u, const T *v)
  {
    T result;
    int i;

    result = T(0);

    for (i=0; i<dimension; i++)
      result += u[i]*v[i];

    return result;
  }

  /**
     Unashamedly copied from 
     http://cobweb.ecn.purdue.edu/~ebertd/texture/perlin/perlin.c
  */
  template <typename T>
  T Gain(T gain, T t)
  {
    T p;
		
    if (t < T(1.0e-16))
      return T(0);
    else if (t > T(1.0 - 1.0e-16))
      return T(1.0);

    p = T(log(T(1.0) - gain)/T(log(T(0.5))));

    if (t < T(0.5))
      return T(pow(T(2.0)*t, p))/T(2.0);
    else
      return T(1.0) - T(pow(T(2.0)*(T(1.0) - t), p))/T(2.0);
  }

#ifdef max
#undef max
#endif // #ifdef max
  /**
   */
  template <typename T>
  T GetMaximumPositiveValue(void)
  {
    return std::numeric_limits<T>::max();
  }

#ifdef min
#undef min
#endif // #ifdef min
  /**
   */
  template <typename T>
  T GetMinimumPositiveValue(void)
  {
    return std::numeric_limits<T>::min();
  }

  /**
   */
  template <typename T>
  T LinearInterpolate(T a, T b, T mu)
  {
    return (a*(T(1.0) - mu) + b*mu);
  }

  /**
   */
  template <typename T, int dimension>
  void LinearInterpolate(const T *a, const T *b, T mu, T *result)
  {
    int i;

    for (i=0; i<dimension; i++)
      result[i] = a[i]*(T(1.0) - mu) + b[i]*mu;
  }

  /**
     The scalar triple product of three vectors A, B, and C is denoted [A,B,C] 
     and defined by
     [A,B,C]	= A.(BxC)
     = B.(CxA)
     = C.(AxB)
     = det(ABC)
     |A_1 A_2 A_3|
     = |B_1 B_2 B_3|
     |C_1 C_2 C_3|
     where A.B denotes a dot product, AxB denotes a cross product, det(A)=|A| 
     denotes a determinant, and A_i, B_i, and C_i are components of the vectors 
     A, B, and C, respectively.
  */
  template <typename T>
  T TripleProduct(const T *A, const T *B, const T *C)
  {
    T crossBC[3];
		
    CrossProduct<T>(B, C, crossBC);
		
    return DotProduct<T, 3>(A, crossBC);
  }

  /**
     Unashamedly copied from 
     http://www.math.niu.edu/~rusin/known-math/99/tetr_interp

     (SIC)
     ... a simple linear interpolation in barycentric coordinates 
     (c1,c2,...,c_n,c_{n+1}) within the simplex defined by N+1 points and N+1 
     values in R^n. In one dimension it would correspond to interpolate
	
     x1 -> f1
     x2 -> f2
	
     f(c1*x1+c2*x2) = c1*f1 + c2*f2    
	
     (c1+c2=1), all c_i>=0. In two dimensions
	
     P1=(x1,y1) -> f1
     P2=(x2,y2) -> f2
     P3=(x3,y3) -> f3
	
     f(c1*P1+c2*P2+c3*P3) = c1*f1+c2*f2+c3*f3
	
     c1+c2+c3=1, all c_i>=0. In three dimensions
	
     P1=(x1,y1,z1) -> f1
     P2=(x2,y2,z2) -> f2
     P3=(x3,y3,z3) -> f3
     P4=(x4,y4,z4) -> f4
	
     f(c1*P1+c2*P2+c3*P3+c4*P4) := c1*f1+c2*f2+c3*f3+c3*f4 
	
     c1+c2+c3+c4=1, all c_i>=0. For any set of (a) 2 points which are not 
     identical, (b) 3 points which are not on the same straigth line and (c) 4 
     points which are not in the same plane. There is one and only one affine 
     mapping from R, R^2, and R^3 into a linear space E such that f(P_i)=f_i, 
     and that's probably the mapping you want because this is the "natural" 
     mapping. Any set of 4 nonnegative real numbers (c1,c2,c3,c4) with sum 1 - in
     the so-called barycentric coordinate system relative to the tetrahedron 
     P1,P2,P3,P4 - will yield some (x,y,z)=c1*P1+c2*P2+c3*P3+c4*P4 inside the 
     tetrahedron and some f by f=c1*f1+c2*f2+c3*f3+c4*f4 where f fulfills the 
     four initial conditions f(P_i)=f_i if c_i=1 and the rest of the c_j=0 and 
     where f is the unique affine mapping defined by these conditions. If one 
     or more of the c_i are negative then the point is outside the tetrahedron 
     but the mapping is still a continuation of the same mapping.
	
     You may rewrite the above formula in a asymetric way by choosing any P_i 
     as the "anchor point", for instance P1 
	
     f((1-c2-c3-c4)P1 + c2P2 + c3P3 + c4)
     = f(P1 + c2(P2-P1) + c3(P3-P1) + c3(P4-P1))
     = f1 + c2*(f2-f1) + c3*(f3-f1) + c4*(f4-f1)
	
     where P is in the tetrahedron if and only if c2,c3,c4 are >=0 and c2+c3+c4<=1, 
     i.e. c1=1-c2-c3-c4 >=0.
	
     No problem so far. What is your problem? If you start from the c_i your 
     problem is solved. Do you want to get the corresponding c_i for an arbitrary 
     (x,y,z)?

     This may be written in an elegant way. The c2,c3,c4 are the solutions of the 
     linear equation system

     x2-x1 x3-x1 x4-x1   c2   x-x1
     y2-y1 y3-y1 y4-y1 * c3 = y-y1
     z2-z1 z3-z1 z4-z1   c4   z-z1
	
     for c2,c3,c4 where x,y,z are the input parameters. If you substitute 
     Q2:=P2-P1, Q3:=P3-P1, Q4=P4-P1 then 
	
     x-x1 
     c2 = < y-y1 , [Q3.Q4]/det(Q2.Q3.Q4) >
     z-y1 
	
     x-x1 
     c3 = < y-y1 , [Q4.Q2]/det(Q2,Q3,Q4) >
     z-y1 
	
     x-x1 
     c4 = < y-y1 , [Q2.Q3]/det(Q2,Q3,Q4) >
     z-y1 

     Where <*.*> denotes the dot product and [*,*] the cross product and 
     det(*,*,*) the determinant which may be evaluated as 
	
     det(Q2,Q3,Q4) = <Q2,[Q3,Q4]> = <Q3,[Q4,Q2]> = <Q4,[Q2,Q3]>
	
     f(x,y,z) = f1 + c2*(f2-f1) + c3*(f3-f1) + c4*(f4-f1)
     = (1-c2-c3-c4)*f1 + c2*f2 + c3*f3 *c4*f4
  */
  template <typename T, int dimension>
  void LinearInterpolationInsideTetrahedron(const T *P1, const T *f1, const T *P2, const T *f2, const T *P3, const T *f3, const T *P4, const T *f4, const T *p, T *f)
  {
    int i;
    T Q2[3],
      Q3[3],
      Q4[3],
      Qp[3],
      determinantQ2Q3Q4,
      crossQ3Q4[3],
      crossQ4Q2[3],
      crossQ2Q3[3],
      c2, 
      c3,
      c4;

    for (i=0; i<3; i++)
      {
	Q2[i] = P2[i] - P1[i];
	Q3[i] = P3[i] - P1[i];
	Q4[i] = P4[i] - P1[i];

	Qp[i] = p[i] - P1[i];
      }

    determinantQ2Q3Q4 = TripleProduct<T>(Q2, Q3, Q4);

    CrossProduct<T>(Q3, Q4, crossQ3Q4);

    for (i=0; i<3; i++)
      crossQ3Q4[i] /= determinantQ2Q3Q4;

    c2 = DotProduct<T, 3>(Qp, crossQ3Q4);

    CrossProduct<T>(Q4, Q2, crossQ4Q2);

    for (i=0; i<3; i++)
      crossQ4Q2[i] /= determinantQ2Q3Q4;

    c3 = DotProduct<T, 3>(Qp, crossQ4Q2);

    CrossProduct<T>(Q2, Q3, crossQ2Q3);

    for (i=0; i<3; i++)
      crossQ2Q3[i] /= determinantQ2Q3Q4;

    c4 = DotProduct<T, 3>(Qp, crossQ2Q3);

    for (i=0; i<dimension; i++)
      f[i] = (T(1.0) - c2 - c3 - c4)*f1[i] + c2*f2[i] + c3*f3[i] + c4*f4[i];
  }

  /**
   */
  template <typename T>
  T Magnitude(T x, T y)
  {
    T result;

    result = x*x + y*y;

    return T(sqrt(result));
  }

  /**
   */
  template <typename T>
  T Magnitude(T x, T y, T z)
  {
    T result;

    result = x*x + y*y + z*z;

    return T(sqrt(result));
  }

  /**
   */
  template <typename T, int dimension>
  T Magnitude(const T *v)
  {
    T result;
    int i;

    result = T(0);

    for (i=0; i<dimension; i++)
      result += v[i]*v[i];

    return T(sqrt(result));
  }

  /**
   */
  template <typename T, int dimension>
  T Normalize(T *v)
  {
    T norma;
    int i;

    norma = T(0);

    for (i=0; i<dimension; i++)
      norma += v[i]*v[i];

    norma = T(sqrt(norma));

    if (norma > DBL_EPSILON)
      for (i=0; i<dimension; i++)
	v[i] /= norma;

    return norma;
  }

  /**
   */
  template <typename T> 
  T RadiansToDegrees(T radians)
  {
    return radians*T(180.0)/Pi<T>();
  }

  /**
   */
  template <typename T, int dimension>
  T SquaredMagnitude(const T *v)
  {
    T result;
    unsigned int i;

    result = T(0);

    for (i=0; i<dimension; i++)
      result += v[i]*v[i];

    return result;
  }

  /**
   */
  template <typename T, int dimension>
  T SquaredDifference(const T *a, const T *b)
  {
    T difference,
      result;
    unsigned int i;

    result = T(0);

    for (i=0; i<dimension; i++)
      {
	difference = a[i] - b[i];

	result += difference*difference;
      }

    return result;
  }

  /**
   */
  template <typename T>
  T SquaredDifference(T firstPointX, T firstPointY, T secondPointX, T secondPointY)
  {
    T difference,
      result;

    difference = firstPointX - secondPointX;

    result = difference*difference;

    difference = firstPointY - secondPointY;

    result += difference*difference;

    return result;
  }

  template <typename T, int dimension>
  void TriangleCircumcenter(const T* A, const T* B, const T* C, T* circumcenter)
  {
	  T a,
		  b,
		  c;
	  int i;

	  a = SquaredDifference<T, dimension>(B, C);
	  b = SquaredDifference<T, dimension>(A, C);
	  c = SquaredDifference<T, dimension>(A, B);

	  for (i=0; i<dimension; ++i)
		  circumcenter[i] = (A[i]*(a*(b + c - a)) + B[i]*(b*(a + c - b)) + C[i]*(c*(a + b - c)))/
			  (T(2)*(a*b + a*c + b*c) - (a*a + b*b + c*c));
  }
}

#endif // MYMATH_INCLUDED

