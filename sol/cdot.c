/*
cdot.c

dot product (complex)
*/

#ifdef _SIMD
#include <immintrin.h>  // SSE/AVX
#endif

#ifdef _DOUBLE
#define real_t double
#else
#define real_t float
#endif

void cdot(
	int simd, int n,
	const real_t *a_r, const real_t *a_i,
	const real_t *b_r, const real_t *b_i,
	double *x_r, double *x_i)
{
	double sum_r = 0;
	double sum_i = 0;

	// no SIMD
	if      (simd == 0) {
		for (int i = 0; i < n; i++) {
			sum_r += (a_r[i] * b_r[i]) - (a_i[i] * b_i[i]);
			sum_i += (a_r[i] * b_i[i]) + (a_i[i] * b_r[i]);
		}
	}
	// SIMD
#ifdef _SIMD
#ifndef _DOUBLE
	// single precision
	else if (simd == 1) {
		// SSE
		// a, b : aligned 16 bytes
#ifdef _WIN32
		__declspec(align(16)) float sumr[4], sumi[4];
#else
		__attribute__((aligned(16))) float sumr[4], sumi[4];
		//alignas(16) float sumr[4], sumi[4];
#endif
		__m128 sr, si, v1r, v1i, v2r, v2i;
		sr = _mm_setzero_ps();
		si = _mm_setzero_ps();
		for (int i = 0; i < n; i += 4) {
			v1r = _mm_load_ps(a_r + i);
			v1i = _mm_load_ps(a_i + i);
			v2r = _mm_load_ps(b_r + i);
			v2i = _mm_load_ps(b_i + i);
			sr = _mm_add_ps(sr, _mm_sub_ps(_mm_mul_ps(v1r, v2r),
			                               _mm_mul_ps(v1i, v2i)));
			si = _mm_add_ps(si, _mm_add_ps(_mm_mul_ps(v1r, v2i),
			                               _mm_mul_ps(v1i, v2r)));
		}
		_mm_store_ps(sumr, sr);
		_mm_store_ps(sumi, si);
		sum_r = sumr[0] + sumr[1] + sumr[2] + sumr[3];
		sum_i = sumi[0] + sumi[1] + sumi[2] + sumi[3];
	}
	else if (simd == 2) {
		// AVX
		// a, b : aligned 32 bytes
#ifdef _WIN32
		__declspec(align(32)) float sumr[8], sumi[8];
#else
		__attribute__((aligned(32))) float sumr[8], sumi[8];
		//alignas(32) float sumr[8], sumi[8];
#endif
		__m256 sr, si, v1r, v1i, v2r, v2i;
		sr = _mm256_setzero_ps();
		si = _mm256_setzero_ps();
		for (int i = 0; i < n; i += 8) {
			v1r = _mm256_load_ps(a_r + i);
			v1i = _mm256_load_ps(a_i + i);
			v2r = _mm256_load_ps(b_r + i);
			v2i = _mm256_load_ps(b_i + i);
			sr = _mm256_add_ps(sr, _mm256_sub_ps(_mm256_mul_ps(v1r, v2r),
			                                     _mm256_mul_ps(v1i, v2i)));
			si = _mm256_add_ps(si, _mm256_add_ps(_mm256_mul_ps(v1r, v2i),
			                                     _mm256_mul_ps(v1i, v2r)));
		}
		_mm256_store_ps(sumr, sr);
		_mm256_store_ps(sumi, si);
		sum_r = sumr[0] + sumr[1] + sumr[2] + sumr[3]
		      + sumr[4] + sumr[5] + sumr[6] + sumr[7];
		sum_i = sumi[0] + sumi[1] + sumi[2] + sumi[3]
		      + sumi[4] + sumi[5] + sumi[6] + sumi[7];
	}
#else  // _DOUBLE
	// double precision
	else if (simd == 1) {
		// SSE
		// a, b : aligned 16 bytes
#ifdef _WIN32
		__declspec(align(16)) double sumr[2], sumi[2];
#else
		__attribute__((aligned(16))) double sumr[2], sumi[2];
		//alignas(16) double sumr[2], sumi[2];
#endif
		__m128d sr, si, v1r, v1i, v2r, v2i;
		sr = _mm_setzero_pd();
		si = _mm_setzero_pd();
		for (int i = 0; i < n; i += 2) {
			v1r = _mm_load_pd(a_r + i);
			v1i = _mm_load_pd(a_i + i);
			v2r = _mm_load_pd(b_r + i);
			v2i = _mm_load_pd(b_i + i);
			sr = _mm_add_pd(sr, _mm_sub_pd(_mm_mul_pd(v1r, v2r),
			                               _mm_mul_pd(v1i, v2i)));
			si = _mm_add_pd(si, _mm_add_pd(_mm_mul_pd(v1r, v2i),
			                               _mm_mul_pd(v1i, v2r)));
		}
		_mm_store_pd(sumr, sr);
		_mm_store_pd(sumi, si);
		sum_r = sumr[0] + sumr[1];
		sum_i = sumi[0] + sumi[1];
	}
	else if (simd == 2) {
		// AVX
		// a, b : aligned 32 bytes
#ifdef _WIN32
		__declspec(align(32)) double sumr[4], sumi[4];
#else
		__attribute__((aligned(32))) double sumr[4], sumi[4];
		//alignas(32) double sumr[4], sumi[4];
#endif
		__m256d sr, si, v1r, v1i, v2r, v2i;
		sr = _mm256_setzero_pd();
		si = _mm256_setzero_pd();
		for (int i = 0; i < n; i += 4) {
			v1r = _mm256_load_pd(a_r + i);
			v1i = _mm256_load_pd(a_i + i);
			v2r = _mm256_load_pd(b_r + i);
			v2i = _mm256_load_pd(b_i + i);
			sr = _mm256_add_pd(sr, _mm256_sub_pd(_mm256_mul_pd(v1r, v2r),
			                                     _mm256_mul_pd(v1i, v2i)));
			si = _mm256_add_pd(si, _mm256_add_pd(_mm256_mul_pd(v1r, v2i),
			                                     _mm256_mul_pd(v1i, v2r)));
		}
		_mm256_store_pd(sumr, sr);
		_mm256_store_pd(sumi, si);
		sum_r = sumr[0] + sumr[1] + sumr[2] + sumr[3];
		sum_i = sumi[0] + sumi[1] + sumi[2] + sumi[3];
	}
#endif  // _DOUBLE
#endif  // _SIMD

	*x_r = sum_r;
	*x_i = sum_i;
}
/*
// double precision (no SIMD)
void zdot(
	int n,
	const double *a_r, const double *a_i,
	const double *b_r, const double *b_i,
	double *x_r, double *x_i)
{
	double sum_r = 0;
	double sum_i = 0;

	for (int i = 0; i < n; i++) {
		sum_r += (a_r[i] * b_r[i]) - (a_i[i] * b_i[i]);
		sum_i += (a_r[i] * b_i[i]) + (a_i[i] * b_r[i]);
	}

	*x_r = sum_r;
	*x_i = sum_i;
}
*/
