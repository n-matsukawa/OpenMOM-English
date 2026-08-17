/*
cholesky.c

modified Cholesky method
equation : [a] * {x} = {b}
*/

#include "omm.h"
#include "complex.h"
#include "omm_prototype.h"

// single row
static void cholesky1_row(
	int simd, int i, int jmin, int jmax,
	real_t **a_r, real_t **a_i, d_complex_t *d, real_t *w_r, real_t *w_i)
{
	for (int j = jmin; j < jmax; j++) {
		double sum_r, sum_i;
		cdot(simd, j, a_r[j], a_i[j], w_r, w_i, &sum_r, &sum_i);

		//w[j] = f_sub(a[i][j], sum);
		w_r[j] = (real_t)(a_r[i][j] - sum_r);
		w_i[j] = (real_t)(a_i[i][j] - sum_i);

		//a[i][j] = f_div(w[j], d[j]);
		double tmp = 1 / ((d[j].r * d[j].r) + (d[j].i * d[j].i));
		a_r[i][j] = (real_t)(((w_r[j] * d[j].r) + (w_i[j] * d[j].i)) * tmp);
		a_i[i][j] = (real_t)(((w_i[j] * d[j].r) - (w_r[j] * d[j].i)) * tmp);

		//d[i] = f_sub(d[i], f_mul(w[j], a[i][j]);
		d[i].r -= (w_r[j] * a_r[i][j]) - (w_i[j] * a_i[i][j]);
		d[i].i -= (w_i[j] * a_r[i][j]) + (w_r[j] * a_i[i][j]);
	}
}

// cholesky decomposition
static void cholesky1(
	int nthread, int simd, int n,
	real_t **a_r, real_t **a_i, d_complex_t *d, real_t **w_r, real_t **w_i)
{
	for (int i = 0; i < n; i++) {
		d[i].r = a_r[i][i];
		d[i].i = a_i[i][i];
		a_r[i][i] = a_i[i][i] = 0;  // for SIMD
	}

	// number of blocks = n / nthread
	int nblock = (n + (nthread - 1)) / nthread;

	int ma = 1;
	for (int iblock = 0; iblock < nblock; iblock++) {

		// monitor
		if ((n >= 3000) && (iblock == (int)(nblock * sqrt(ma / 10.0)))) {printf("."); fflush(stdout); ma++;}

		// starting row number
		int i0 = iblock * nthread;

		// parallel part
		if (nthread > 1) {
#ifdef _OPENMP
#pragma omp parallel
#endif
			{
#ifdef _OPENMP
				int tid = omp_get_thread_num();
#else
				int tid = 0;
#endif
				int i = i0 + tid;
				if (i < n) {
					cholesky1_row(simd, i, 0, i0, a_r, a_i, d, w_r[tid], w_i[tid]);
				}
			}
		}

		// serial part or single thread
		for (int i = i0; (i < i0 + nthread) && (i < n); i++) {
			int j0 = (nthread > 1) ? i0 : 0;
			int tid = i - i0;
			cholesky1_row(simd, i, j0, i, a_r, a_i, d, w_r[tid], w_i[tid]);
		}
	}
}

// cholesky substitution (serial)
static void cholesky2(
	int n, real_t **a_r, real_t **a_i, const d_complex_t *b, d_complex_t *x,
	const d_complex_t *d, d_complex_t *w)
{
	// forward substitution
	for (int i = 0; i < n; i++) {
		d_complex_t sum = d_complex(0, 0);
		for (int j = 0; j < i; j++) {
			//sum = f_add(sum, f_mul(a[i][j], w[j]));
			sum.r += (a_r[i][j] * w[j].r) - (a_i[i][j] * w[j].i);
			sum.i += (a_i[i][j] * w[j].r) + (a_r[i][j] * w[j].i);
		}
		//w[i] = f_sub(b[i], sum);
		w[i].r = b[i].r - sum.r;
		w[i].i = b[i].i - sum.i;
	}

	// backward substitution
	for (int i = n - 1; i >= 0; i--) {
		d_complex_t sum = d_complex(0, 0);
		for (int j = i + 1; j < n; j++) {
			//sum = f_add(sum, f_mul(a[j][i], x[j]));
			sum.r += (a_r[j][i] * x[j].r) - (a_i[j][i] * x[j].i);
			sum.i += (a_i[j][i] * x[j].r) + (a_r[j][i] * x[j].i);
		}
		//x[i] = f_sub(f_div(w[i], d[i]), sum);
		double tmp = 1.0 / ((d[i].r * d[i].r) + (d[i].i * d[i].i));
		x[i].r = ((w[i].r * d[i].r) + (w[i].i * d[i].i)) * tmp - sum.r;
		x[i].i = ((w[i].i * d[i].r) - (w[i].r * d[i].i)) * tmp - sum.i;
	}
}

// cholesky
void cholesky(int nthread, int simd, int ifreq)
{
	size_t size = NElement * sizeof(d_complex_t);
	memset(Wv, 0, size);
	memset(Dv, 0, size);

	cholesky1(nthread, simd, NElement, Am_r, Am_i, Dv, Wv_r, Wv_i);
	cholesky2(NElement, Am_r, Am_i, Bv, &Cv[ifreq * NElement], Dv, Wv);
}
