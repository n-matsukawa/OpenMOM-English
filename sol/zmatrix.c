/*
zmatrix.c

setup impedance matrix
*/

#include "omm.h"

// Green's function
static inline d_complex_t green(double r, double s1, double s2, double kwave, double radius)
{
	d_complex_t g;

	const double arg = kwave * r;
	if (arg > EPS) {
		const double fctr = 1 / (4 * PI * r);
		g.r = +cos(arg) * fctr;
		g.i = -sin(arg) * fctr;
	}
	else {
		const double ds = (s1 + s2) / 2;
		const double a = ds / (2 * radius);
		g.r = log(a + sqrt((a * a) + 1)) / (2 * PI * ds);
		g.i = -kwave / (4 * PI);
	}

	return g;
}


// matrix element
static inline void zelement(int iground, element_t *ei, element_t *ej, double kwave, real_t *z_r, real_t *z_i)
{
	double pci[3], pmi[3], ppi[3];
	double pcj[3], pmj[3], ppj[3];
	double dx, dy, dz, dd;

	// ground image
	double sgn[3];
	sgn[0] = (iground == 1) ? -1 : +1;
	sgn[1] = (iground == 2) ? -1 : +1;
	sgn[2] = (iground == 3) ? -1 : +1;

	// position
	for (int k = 0; k < 3; k++) {
		pci[k] = ei->pos[0][k];
		pmi[k] = ei->pos[1][k];
		ppi[k] = ei->pos[2][k];
		pcj[k] = ej->pos[0][k] * sgn[k];
		pmj[k] = ej->pos[1][k] * sgn[k];
		ppj[k] = ej->pos[2][k] * sgn[k];
	}

	// element length (center/minus/plus)
	const double lci = ei->lng[0];
	const double lcj = ej->lng[0];
	const double lmi = ei->lng[1];
	const double lmj = ej->lng[1];
	const double lpi = ei->lng[2];
	const double lpj = ej->lng[2];

	// wire radius
	const double rwire = (ei->radius + ej->radius) / 2;
	//const double rwire = MIN(ei->radius, ej->radius);
	//const double rwire = sqrt(ei->radius * ej->radius);

	// center to center
	dx = pci[0] - pcj[0];
	dy = pci[1] - pcj[1];
	dz = pci[2] - pcj[2];
	dd = sqrt((dx * dx) + (dy * dy) + (dz * dz));
	d_complex_t gcc = green(dd, lci, lcj, kwave, rwire);

	// minus to minus
	dx = pmi[0] - pmj[0];
	dy = pmi[1] - pmj[1];
	dz = pmi[2] - pmj[2];
	dd = sqrt((dx * dx) + (dy * dy) + (dz * dz));
	d_complex_t gmm = green(dd, lmi, lmj, kwave, rwire);

	// minus to plus
	dx = pmi[0] - ppj[0];
	dy = pmi[1] - ppj[1];
	dz = pmi[2] - ppj[2];
	dd = sqrt((dx * dx) + (dy * dy) + (dz * dz));
	d_complex_t gmp = green(dd, lmi, lpj, kwave, rwire);

	// plus to minus
	dx = ppi[0] - pmj[0];
	dy = ppi[1] - pmj[1];
	dz = ppi[2] - pmj[2];
	dd = sqrt((dx * dx) + (dy * dy) + (dz * dz));
	d_complex_t gpm = green(dd, lpi, lmj, kwave, rwire);

	// plus to plus
	dx = ppi[0] - ppj[0];
	dy = ppi[1] - ppj[1];
	dz = ppi[2] - ppj[2];
	dd = sqrt((dx * dx) + (dy * dy) + (dz * dz));
	d_complex_t gpp = green(dd, lpi, lpj, kwave, rwire);

	const double aip
		= (ei->tan[0] * ej->tan[0] * sgn[0])
		+ (ei->tan[1] * ej->tan[1] * sgn[1])
		+ (ei->tan[2] * ej->tan[2] * sgn[2]);

	const double wss = (kwave * lci) * (kwave * lcj);

	*z_r = (real_t)(+(ETA0 / kwave) * (gpp.i - gpm.i - gmp.i + gmm.i - (wss * aip * gcc.i)));
	*z_i = (real_t)(-(ETA0 / kwave) * (gpp.r - gpm.r - gmp.r + gmm.r - (wss * aip * gcc.r)));
}


void zmatrix(int ifreq)
{
	const double kwave = (2 * PI * Frequency[ifreq]) / C;

	int i;
#ifdef _OPENMP
#pragma omp parallel for schedule(static, 1)
#endif
	for (i = 0; i < NElement; i++) {
		for (int j = 0; j <= i; j++) {
			real_t z_r, z_i;

			zelement(0, &Element[i], &Element[j], kwave, &z_r, &z_i);
			Am_r[i][j] = z_r;
			Am_i[i][j] = z_i;

			// ground image
			if (IGround) {
				zelement(IGround, &Element[i], &Element[j], kwave, &z_r, &z_i);
				Am_r[i][j] -= z_r;
				Am_i[i][j] -= z_i;
			}
		}
	}

	// load
	const double omega = 2 * PI * Frequency[ifreq];
	for (i = 0; i < NElement; i++) {
		const int iload = Element[i].iload;
		if (iload) {
			const double load = Element[i].load;
			if      (iload == 1) {
				// R
				Am_r[i][i] += (real_t)load;
			}
			else if (iload == 2) {
				// L
				Am_i[i][i] += (real_t)(omega * load);
			}
			else if ((iload == 3) && (fabs(load) > 1e-20)) {
				// C
				Am_i[i][i] -= (real_t)(1 / (omega * load));
			}
		}
	}
/*
	// debug
	double sum_r = 0;
	double sum_i = 0;
	for (i = 0; i < NElement; i++) {
		for (int j = 0; j <= i; j++) {
			sum_r += fabs(Am_r[i][j]);
			sum_i += fabs(Am_i[i][j]);
		}
	}
	printf("%d %d %e %e\n", NElement, ifreq, sum_r, sum_i);
*/
}
