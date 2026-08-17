/*
solve.c

solve current distribution
*/

#include "omm.h"
#include "complex.h"
#include "omm_prototype.h"

// check solution (option)
static void checksolution(int ifreq)
{
	zmatrix(ifreq);

	double emax = 0;
	for (int ie = 0; ie < NElement; ie++) {
		d_complex_t sum = d_complex(0, 0);
		for (int je = 0; je < NElement; je++) {
			d_complex_t z = (ie >= je) ? d_complex(Am_r[ie][je], Am_i[ie][je])
			                           : d_complex(Am_r[je][ie], Am_i[je][ie]);
			d_complex_t c = Cv[(ifreq * NElement) + je];
			sum = d_add(sum, d_mul(z, c));
		}
		sum = d_sub(sum, d_complex(Bv[ie].r, Bv[ie].i));
		emax = MAX(emax, d_abs(sum));
	}
	printf("  error=%.3e", emax);
}


// output zmatrix.log
static void zmatrix_log(FILE *fp)
{
	for (int ie = 0; ie < NElement; ie++) {
		for (int je = 0; je < NElement; je++) {
			double tmp_r = (je <= ie) ? Am_r[ie][je] : Am_r[je][ie];
			double tmp_i = (je <= ie) ? Am_i[ie][je] : Am_i[je][ie];
			fprintf(fp, "%6d %6d %18.10e %18.10e\n", ie + 1, je + 1, tmp_r, tmp_i);
		}
	}
}


void solve(int nthread, int simd, int zmatrixlog, int check, int monitor, double cpu[])
{
	// initialize current
	size_t size = NFrequency * NElement * sizeof(d_complex_t);
	memset(Cv, 0, size);

	// output zmatrix.log
	FILE *fp = NULL;
	if (zmatrixlog) {
		fp = fopen("zmatrix.log", "w");
		fprintf(fp, "%d %d\n", NElement, NFrequency);
	}

	// loop on frequency
	for (int ifreq = 0; ifreq < NFrequency; ifreq++) {
		// monitor
		if (monitor) {
			printf("                (%3d/%d)", ifreq + 1, NFrequency);
			fflush(stdout);
		}

		// setup impedance matrix
		const double t1 = cputime();
		zmatrix(ifreq);
		cpu[1] += cputime() - t1;

		// setup RHS vector
		rhsvector(ifreq);

		// output zmatrix.log
		if (zmatrixlog) {
			zmatrix_log(fp);
		}

		// solve linear equations
		const double t2 = cputime();
		cholesky(nthread, simd, ifreq);
		cpu[2] += cputime() - t2;

		// check solution (option)
		if (check) {
			checksolution(ifreq);
		}

		// monitor
		if (monitor) {
			printf("%s\n", "");
			fflush(stdout);
		}
	}

	// close
	if (zmatrixlog) {
		fclose(fp);
	}
}
