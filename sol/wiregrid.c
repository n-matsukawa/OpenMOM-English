/*
wiregrid.c

make wire grid model
*/

#include "omm.h"
#include "vector.h"
#include "omm_prototype.h"

// center of a quadangle
static inline void center4(const double p1[3], const double p2[3], const double p3[3], const double p4[3],
	double p0[3], double eps)
{
	double v1[3], v2[3], v3[3];

	for (int k = 0; k < 3; k++) {
		v1[k] = p4[k] - p3[k];
		v2[k] = p2[k] - p3[k];
	}
	vector_outprod(v1, v2, v3);
	const double d1 = vector_inprod(v1, v2);
	const double s1 = vector_length(v3);

	for (int k = 0; k < 3; k++) {
		v1[k] = p1[k] - p4[k];
		v2[k] = p3[k] - p4[k];
	}
	vector_outprod(v1, v2, v3);
	const double d2 = vector_inprod(v1, v2);
	const double s2 = vector_length(v3);

	for (int k = 0; k < 3; k++) {
		v1[k] = p2[k] - p1[k];
		v2[k] = p4[k] - p1[k];
	}
	vector_outprod(v1, v2, v3);
	const double d3 = vector_inprod(v1, v2);
	const double s3 = vector_length(v3);

	for (int k = 0; k < 3; k++) {
		v1[k] = p3[k] - p2[k];
		v2[k] = p1[k] - p2[k];
	}
	vector_outprod(v1, v2, v3);
	const double d4 = vector_inprod(v1, v2);
	const double s4 = vector_length(v3);

	if ((s1 + s2 + s3 + s4) > eps) {
		for (int k = 0; k < 3; k++) {
			p0[k] = ((s1 * p1[k]) + (s2 * p2[k]) + (s3 * p3[k]) + (s4 * p4[k]))
			      / (s1 + s2 + s3 + s4);
		}
	}
	else if (fabs(d1 + d2 + d3 + d4) > eps * eps) {
		for (int k = 0; k < 3; k++) {
			p0[k] = ((d1 * p1[k]) + (d2 * p2[k]) + (d3 * p3[k]) + (d4 * p4[k]))
			      / (d1 + d2 + d3 + d4);
		}
	}
	else {
		for (int k = 0; k < 3; k++) {
			p0[k] = (p1[k] + p2[k] + p3[k] + p4[k]) / 4;
		}
	}
}

// coordinate conversion to XYZ
static inline void conv2xyz(int cosys, const double pos[3], double xyz[3])
{
	const double dtor = atan(1) / 45;

	if      (cosys == 1) {
		xyz[0] = pos[0];
		xyz[1] = pos[1];
		xyz[2] = pos[2];
	}
	else if (cosys == 2) {
		xyz[0] = pos[0] * cos(pos[1] * dtor);
		xyz[1] = pos[0] * sin(pos[1] * dtor);
		xyz[2] = pos[2];
	}
	else if (cosys == 3) {
		xyz[0] = pos[0] * sin(pos[1] * dtor) * cos(pos[2] * dtor);
		xyz[1] = pos[0] * sin(pos[1] * dtor) * sin(pos[2] * dtor);
		xyz[2] = pos[0] * cos(pos[1] * dtor);
	}
	else {
		xyz[0] = xyz[1] = xyz[2] = 0;
	}
}

// two lines are equal ?
static inline int equalline(double aline[2][3], double bline[2][3], double eps)
{
	double d00 = 0;
	double d01 = 0;
	double d10 = 0;
	double d11 = 0;
	for (int k = 0; k < 3; k++) {
		d00 += fabs(aline[0][k] - bline[0][k]);
		d01 += fabs(aline[0][k] - bline[1][k]);
		d10 += fabs(aline[1][k] - bline[0][k]);
		d11 += fabs(aline[1][k] - bline[1][k]);
	}

	return ((d00 < eps) && (d11 < eps)) || ((d01 < eps) && (d10 < eps));
}

// size of bounding box
static inline double bounding(void)
{
	const double inf = 1e10;
	double x1 = 0, x2 = 0, y1 = 0, y2 = 0, z1 = 0, z2 = 0;
	double xmin, ymin, zmin;
	double xmax, ymax, zmax;
	xmin = ymin = zmin = +inf;
	xmax = ymax = zmax = -inf;
	for (int ig = 0; ig < NGeometry; ig++) {
		geometry_t *g = &Geometry[ig];
		int nv = (g->shape == WIRE) ? 2 : 4;
		for (int iv = 0; iv < nv; iv++) {
			if      (g->cosys == XYZ) {
				x1 = x2 = g->pos[iv][0];  // X
				y1 = y2 = g->pos[iv][1];  // Y
				z1 = z2 = g->pos[iv][2];  // Z
			}
			else if (g->cosys == CYLINDER) {
				x1 = y1 = -g->pos[iv][0];  // -rho
				x2 = y2 = +g->pos[iv][0];  // +rho
				z1 = z2 =  g->pos[iv][2];  // Z
			}
			else if (g->cosys == POLAR) {
				x1 = y1 = z1 = -g->pos[iv][0];  // -r
				x2 = y2 = z2 = +g->pos[iv][0];  // +r
			}
			xmin = MIN(xmin, x1 + g->offset[0]);
			xmax = MAX(xmax, x2 + g->offset[0]);
			ymin = MIN(ymin, y1 + g->offset[1]);
			ymax = MAX(ymax, y2 + g->offset[1]);
			zmin = MIN(zmin, z1 + g->offset[2]);
			zmax = MAX(zmax, z2 + g->offset[2]);
		}
	}

	//printf("%e %e %e %e %e %e\n", xmin, xmax, ymin, ymax, zmin, zmax);
	return (fabs(xmin - xmax) + fabs(ymin - ymax) + fabs(zmin - zmax));
}

// set wire element data
static inline void setelement(const double cm[3], const double cp[3], geometry_t g, element_t *e)
{
	// to xyz
	double dm[3], dp[3];
	conv2xyz(g.cosys, cm, dm);
	conv2xyz(g.cosys, cp, dp);

	// minus/plus position of the element
	for (int k = 0; k < 3; k++) {
		e->pos[1][k] = dm[k] + g.offset[k];
		e->pos[2][k] = dp[k] + g.offset[k];
	}

	// center length of the element
	double sum = 0;
	for (int k = 0; k < 3; k++) {
		double dl = e->pos[1][k] - e->pos[2][k];
		sum += dl * dl;
	}
	e->lng[0] = sqrt(sum);

	// radius
	e->radius = g.iradius ? g.radius : (g.radius * e->lng[0]);
}

// check double count of elements
static inline int count2(int ie, double eps)
{
	double aline[2][3], bline[2][3];

	if (ie <= 0) return 0;

	for (int k = 0; k < 3; k++) {
		aline[0][k] = Element[ie].pos[1][k];
		aline[1][k] = Element[ie].pos[2][k];
	}

	int neq = 0;
	for (int je = 0; je < ie; je++) {
		for (int k = 0; k < 3; k++) {
			bline[0][k] = Element[je].pos[1][k];
			bline[1][k] = Element[je].pos[2][k];
		}
		if (equalline(aline, bline, eps)) {
			neq++;
		}
	}

	return neq;
}

/*
share edge of a plane
0 : not share or greater division -> equal division
n : share and lesser division -> unequal division

ig : geometry No. (=0,...)
ic : edge No. (=0/1/2/3)
*/
static inline int shareedge(int ig, int ic, double eps)
{
	double aline[2][3], bline[2][3];
	const int idx[4][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}};

	if ((ig < 0) || (ig >= NGeometry)) return 0;
	if ((ic < 0) || (ic > 3)) return 0;
	geometry_t *g = &Geometry[ig];
	if (g->shape != PLANE) return 0;
	if (g->cosys != XYZ) return 0;

	for (int k = 0; k < 3; k++) {
		aline[0][k] = g->pos[idx[ic][0]][k] + g->offset[k];
		aline[1][k] = g->pos[idx[ic][1]][k] + g->offset[k];
	}
	int adiv = g->div[ic % 2];

	for (int jg = 0; jg < NGeometry; jg++) {
		if (ig == jg) continue;
		geometry_t *g2 = &Geometry[jg];
		if (g2->shape != PLANE) continue;
		if (g2->cosys != XYZ) continue;
		for (int iv = 0; iv < 4; iv++) {
			for (int k = 0; k < 3; k++) {
				bline[0][k] = g2->pos[idx[iv][0]][k] + g2->offset[k];
				bline[1][k] = g2->pos[idx[iv][1]][k] + g2->offset[k];
			}
			const int bdiv = g2->div[iv % 2];
			if (equalline(aline, bline, eps) && (adiv < bdiv)) {
				return bdiv;
			}
		}
	}

	return 0;
}

void wiregrid(void)
{
	// initialize

	for (int ie = 0; ie < LElement; ie++) {
		Element[ie].ifeed = Element[ie].iload = 0;
	}

	// zero distance (machine epsilon)

	const double eps = EPS * bounding();
	//printf("eps=%e\n", eps);

	// loop on geometries

	int ne = 0;
	for (int ig = 0; ig < NGeometry; ig++) {
		geometry_t g = Geometry[ig];
		double (*p)[3] = g.pos;

		// wire
		if (g.shape == WIRE) {
			int n1 = g.div[0];
			for (int i1 = 0; i1 < n1; i1++) {
				// minus/plus node position
				double am = (double)(i1 + 0) / n1;
				double ap = (double)(i1 + 1) / n1;
				double cm[3], cp[3];
				for (int k = 0; k < 3; k++) {
					cm[k] = ((1 - am) * p[0][k]) + (am * p[1][k]);
					cp[k] = ((1 - ap) * p[0][k]) + (ap * p[1][k]);
				}

				// set wire element data
				element_t *e = &Element[ne];
				setelement(cm, cp, g, e);

				// skip zero length
				if (e->lng[0] < eps) continue;

				// skip double count
				if (count2(ne, eps)) continue;

				// skip element on the ground
				if (IGround && (fabs(e->pos[1][IGround - 1]) + fabs(e->pos[2][IGround - 1]) < EPS)) continue;

				// (for plot)
				e->border = 1;
				e->wire = 1;

				// feed
				if (g.ifeed && ((2 * i1 == n1) || (2 * i1 + 1 == n1))) {
					e->ifeed = 1;
					e->feed[0] = g.feed[0];
					e->feed[1] = g.feed[1];
				}

				// load
				if (g.iload && ((2 * i1 == n1) || (2 * i1 + 1 == n1))) {
					e->iload = g.iload;
					e->load = g.load;
				}

				ne++;
			}
		}

		// plane
		else if (g.shape == PLANE) {
			int n1 = g.div[0];
			int n2 = g.div[1];
			double *ae[4];
			int bdiv[4];
			double p1[3], p2[3], p3[3], p4[3];
			double cm[3], cp[3];

			// division of four edges : bdiv

			for (int ic = 0; ic < 4; ic++) {
				int adiv = g.div[ic % 2];
				bdiv[ic] = shareedge(ig, ic, eps);
				ae[ic] = (double *)malloc((adiv + 1) * sizeof(double));
				for (int i = 0; i <= adiv; i++) {
					ae[ic][i] = !bdiv[ic] ? (double)i / adiv :
						floor((i * bdiv[ic]) / (double)adiv + 0.5) / bdiv[ic];
					//printf("%d %d %d %f\n", ig, ic, i, ae[ic][i]);
				}
			}

			// direction 1-2

			int i2s = !bdiv[0] ? 0 : 1;
			int i2e = !bdiv[2] ? n2 : n2 - 1;
			for (int i2 = i2s; i2 <= i2e; i2++) {
				for (int i1 = 1; i1 <= n1; i1++) {

					for (int k = 0; k < 3; k++) {
						p1[k] = ((1 - ae[0][i1 - 1]) * p[0][k]) + (ae[0][i1 - 1] * p[1][k]);
						p2[k] = ((1 - ae[1][i2    ]) * p[1][k]) + (ae[1][i2    ] * p[2][k]);
						p3[k] = ((1 - ae[2][i1 - 1]) * p[3][k]) + (ae[2][i1 - 1] * p[2][k]);
						p4[k] = ((1 - ae[3][i2    ]) * p[0][k]) + (ae[3][i2    ] * p[3][k]);
					}
					center4(p1, p2, p3, p4, cm, eps);

					for (int k = 0; k < 3; k++) {
						p1[k] = ((1 - ae[0][i1    ]) * p[0][k]) + (ae[0][i1    ] * p[1][k]);
						p2[k] = ((1 - ae[1][i2    ]) * p[1][k]) + (ae[1][i2    ] * p[2][k]);
						p3[k] = ((1 - ae[2][i1    ]) * p[3][k]) + (ae[2][i1    ] * p[2][k]);
						p4[k] = ((1 - ae[3][i2    ]) * p[0][k]) + (ae[3][i2    ] * p[3][k]);
					}
					center4(p1, p2, p3, p4, cp, eps);

					// set wire element data
					element_t *e = &Element[ne];
					setelement(cm, cp, g, e);

					// skip zero length
					if (e->lng[0] < eps) continue;

					// skip double count
					if (count2(ne, eps)) continue;

					// skip element on the ground
					if (IGround && (fabs(e->pos[1][IGround - 1]) + fabs(e->pos[2][IGround - 1]) < EPS)) continue;

					// (for plot)
					e->border = (i2 == 0) || (i2 == n2);
					e->wire = 0;

					ne++;
				}
			}

			// direction 1-4

			int i1s = !bdiv[3] ? 0 : 1;
			int i1e = !bdiv[1] ? n1 : n1 - 1;
			for (int i1 = i1s; i1 <= i1e; i1++) {
				for (int i2 = 1; i2 <= n2; i2++) {

					for (int k = 0; k < 3; k++) {
						p1[k] = ((1 - ae[0][i1    ]) * p[0][k]) + (ae[0][i1    ] * p[1][k]);
						p2[k] = ((1 - ae[1][i2 - 1]) * p[1][k]) + (ae[1][i2 - 1] * p[2][k]);
						p3[k] = ((1 - ae[2][i1    ]) * p[3][k]) + (ae[2][i1    ] * p[2][k]);
						p4[k] = ((1 - ae[3][i2 - 1]) * p[0][k]) + (ae[3][i2 - 1] * p[3][k]);
					}
					center4(p1, p2, p3, p4, cm, eps);

					for (int k = 0; k < 3; k++) {
						p1[k] = ((1 - ae[0][i1    ]) * p[0][k]) + (ae[0][i1    ] * p[1][k]);
						p2[k] = ((1 - ae[1][i2    ]) * p[1][k]) + (ae[1][i2    ] * p[2][k]);
						p3[k] = ((1 - ae[2][i1    ]) * p[3][k]) + (ae[2][i1    ] * p[2][k]);
						p4[k] = ((1 - ae[3][i2    ]) * p[0][k]) + (ae[3][i2    ] * p[3][k]);
					}
					center4(p1, p2, p3, p4, cp, eps);

					// set wire element data
					element_t *e = &Element[ne];
					setelement(cm, cp, g, e);

					// skip zero length
					if (e->lng[0] < eps) continue;

					// skip double count
					if (count2(ne, eps)) continue;

					// skip element on the ground
					if (IGround && (fabs(e->pos[1][IGround - 1]) + fabs(e->pos[2][IGround - 1]) < EPS)) continue;

					// (for plot)
					e->border = (i1 == 0) || (i1 == n1);
					e->wire = 0;

					ne++;
				}
			}

			for (int ic = 0; ic < 4; ic++) {
				free(ae[ic]);
			}
		}
	}

	// total number of elements
	NElement = ne;

	// center position and unit vector
	for (int ie = 0; ie < NElement; ie++) {
		for (int k = 0; k < 3; k++) {
			Element[ie].pos[0][k] = (Element[ie].pos[1][k] + Element[ie].pos[2][k]) / 2;
			Element[ie].tan[k] = (Element[ie].pos[2][k] - Element[ie].pos[1][k]) / Element[ie].lng[0];
		}
	}

	// lengths of minus and plus segment : lng[1/2]
	int neq[3];
	for (int ie = 0; ie < NElement; ie++) {
		for (int m = 1; m <= 2; m++) {
			double *pos = Element[ie].pos[m];
			double sum = 0;
			neq[m] = 0;
			for (int je = 0; je < NElement; je++) {
				double dif1 = 0;
				double dif2 = 0;
				for (int k = 0; k < 3; k++) {
					dif1 += fabs(Element[je].pos[1][k] - pos[k]);
					dif2 += fabs(Element[je].pos[2][k] - pos[k]);
				}
				if ((dif1 < eps) || (dif2 < eps)) {
					sum += Element[je].lng[0];
					neq[m]++;
				}
			}
			// average of connecting elements
			//printf("%d %d %d %f %f\n", ie + 1, m, neq[m], sum, sum / neq[m]);
			Element[ie].lng[m] = sum / neq[m];
			//Element[ie].lng[m] = Element[ie].lng[0];  // debug !
		}
/*
		open = 0 : both ends are connected.
		     = 1 : minus end is open and plus end is connected.
		     = 2 : plus end is open and minus end is connected.
		     = 3 : both ends are open.
*/
		if ((neq[1] >  1) && (neq[2] >  1)) Element[ie].iopen = 0;
		if ((neq[1] == 1) && (neq[2] >  1)) Element[ie].iopen = 1;
		if ((neq[1] >  1) && (neq[2] == 1)) Element[ie].iopen = 2;
		if ((neq[1] == 1) && (neq[2] == 1)) Element[ie].iopen = 3;
		if (IGround) {
			if      ((Element[ie].iopen == 1) && (fabs(Element[ie].pos[1][IGround - 1]) < EPS)) {
				Element[ie].iopen = 0;
			}
			else if ((Element[ie].iopen == 2) && (fabs(Element[ie].pos[2][IGround - 1]) < EPS)) {
				Element[ie].iopen = 0;
			}
		}
	}
/*
	// debug
	for (int ie = 0; ie < NElement; ie++) {
		element_t e = Element[ie];
		//printf("%d %d %d\n",   ie,e.ifeed,e.iload);
		//printf("%d %e %e %e\n",ie,e.pos[0][0],e.pos[0][1],e.pos[0][2]);
		//printf("%d %e %e %e\n",ie,e.pos[1][0],e.pos[1][1],e.pos[1][2]);
		//printf("%d %e %e %e\n",ie,e.pos[2][0],e.pos[2][1],e.pos[2][2]);
		//printf("%5d %e %e %e\n",ie,e.lng[0],e.lng[1],e.lng[2]);
		//printf("%5d %.5f %.5f %.5f\n",ie,e.tan[0],e.tan[1],e.tan[2]);
		//printf("%5d %e\n",      ie,e.radius);
		//printf("%d %d\n",      ie,e.open);
	}
*/
}
