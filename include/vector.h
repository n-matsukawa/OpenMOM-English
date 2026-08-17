/*
vector.h
*/

#include <math.h>

// inner product
static inline double vector_inprod(const double v1[3], const double v2[3])
{
	return (v1[0] * v2[0]) +
	       (v1[1] * v2[1]) +
	       (v1[2] * v2[2]);
}

// outer product
static inline void vector_outprod(const double v1[3], const double v2[3], double v3[3])
{
	v3[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
	v3[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
	v3[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);
}

// length^2
static inline double vector_length2(const double v[3])
{
	return (v[0] * v[0]) +
	       (v[1] * v[1]) +
	       (v[2] * v[2]);
}

// length
static inline double vector_length(const double v[3])
{
	return sqrt(vector_length2(v));
}

// normalize
static inline void vector_normalize(double v[3])
{
	double d;
	if ((d = vector_length(v)) > 0) {
		v[0] /= d;
		v[1] /= d;
		v[2] /= d;
	}
}

// distance^2
static inline double vector_distance2(const double v1[3], const double v2[3])
{
	return (v1[0] - v2[0]) * (v1[0] - v2[0]) +
	       (v1[1] - v2[1]) * (v1[1] - v2[1]) +
	       (v1[2] - v2[2]) * (v1[2] - v2[2]);
}

// distance
static inline double vector_distance(const double v1[3], const double v2[3])
{
	return sqrt(vector_distance2(v1, v2));
}

// differ or not (fast)
static inline double vector_diff(const double v1[3], const double v2[3])
{
	return fabs(v1[0] - v2[0]) +
	       fabs(v1[1] - v2[1]) +
	       fabs(v1[2] - v2[2]);
}

// distance (2d)
static inline double vector2d_distance(const double p1[2], const double p2[2])
{
	return sqrt((p1[0] - p2[0]) * (p1[0] - p2[0])
	          + (p1[1] - p2[1]) * (p1[1] - p2[1]));
}
