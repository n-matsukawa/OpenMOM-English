/*
planewave

plane wave field
*/

#include "omm.h"
#include "complex.h"

// cross vector : v = r x c
static void rc_cross(const double r[], const d_complex_t c[], d_complex_t v[])
{
	v[0] = d_sub(d_rmul(r[1], c[2]), d_rmul(r[2], c[1]));
	v[1] = d_sub(d_rmul(r[2], c[0]), d_rmul(r[0], c[2]));
	v[2] = d_sub(d_rmul(r[0], c[1]), d_rmul(r[1], c[0]));
}


// inner product
static double in_prod(const double v1[], const double v2[])
{
	return (v1[0] * v2[0]) + (v1[1] * v2[1]) + (v1[2] * v2[2]);
}


// E-incidence
static d_complex_t einc(double t1, double p1, int pol, double a, double r)
{
	assert((pol >= 1) && (pol <= 5));

	d_complex_t ret = d_complex(0, 0);

	if      (pol == 1) {
		// V
		ret = d_complex(-t1, 0);
	}
	else if (pol == 2) {
		// H
		ret = d_complex(+p1, 0);
	}
	else if (pol == 3) {
		// RHCP
		ret = d_rmul(sqrt(0.5), d_complex(+t1, +p1));
	}
	else if (pol == 4) {
		// LHCP
		ret = d_rmul(sqrt(0.5), d_complex(+t1, -p1));
	}
	else if (pol == 5) {
		// elliptical
		const double dtor = atan(1) / 45;
		const double major = + t1 * cos(a * dtor) + p1 * sin(a * dtor);
		const double minor = - t1 * sin(a * dtor) + p1 * cos(a * dtor);
		ret = d_rmul(1 / sqrt(1 + (r * r)), d_complex(major, r * minor));
	}

	return ret;
}


void planewave(const double pos[], d_complex_t ei[], d_complex_t hi[],
               double theta, double phi, int pol, double a, double r, double kwave)
{
	assert((pol >= 1) && (pol <= 5));

	double r1[3], t1[3], p1[3], ri[3];
	const double dtor = atan(1) / 45;

	const double sint = sin(theta * dtor);
	const double cost = cos(theta * dtor);
	const double sinp = sin(phi   * dtor);
	const double cosp = cos(phi   * dtor);

	// unit vector in r, theta and phi
	r1[0] = + sint * cosp;
	r1[1] = + sint * sinp;
	r1[2] = + cost;
	t1[0] = + cost * cosp;
	t1[1] = + cost * sinp;
	t1[2] = - sint;
	p1[0] =        - sinp;
	p1[1] =        + cosp;
	p1[2] =          0;

	// phase factor
	const double phs = kwave * in_prod(pos, r1);
	const d_complex_t pfac = d_exp(phs);

	// E
	for (int k = 0; k < 3; k++) {
		ei[k] = d_mul(einc(t1[k], p1[k], pol, a, r), pfac);
	}

	// propagation vector = -r1
	for (int k = 0; k < 3; k++) {
		ri[k] = -r1[k];
	}

	// H = r x E
	rc_cross(ri, ei, hi);
}
