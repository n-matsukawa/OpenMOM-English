/*
allocfree.c

alloc and free
*/

#include "omm.h"

#ifdef _SIMD
#include <immintrin.h>  // SSE/AVX
#endif

// alloc
void memalloc(int step, int nthread, int simd)
{
	size_t size;

	if (step == 1) {
		// element
		assert(LElement > 0);
		size = LElement * sizeof(element_t);
		Element = (element_t *)malloc(size);

		// feed
		int nfeed = 0;
		for (int n = 0; n < NGeometry; n++) {
			if (Geometry[n].ifeed) {
				nfeed++;
			}
		}
		if (nfeed > 0) {
			NFeed = nfeed;
			size = NFeed * NFrequency * sizeof(d_complex_t);
			Zin = (d_complex_t *)malloc(size);
		}

		// current
		assert(NFrequency > 0);
		size = NFrequency * LElement * sizeof(d_complex_t);
		Cv = (d_complex_t *)malloc(size);
	}
	else if (step == 2) {
		assert(LElement > 0);
		assert(nthread > 0);
		// matrix and wirk vector
		Am_r = (real_t **)malloc(LElement * sizeof(real_t *));
		Am_i = (real_t **)malloc(LElement * sizeof(real_t *));
		Wv_r = (real_t **)malloc(nthread * sizeof(real_t *));
		Wv_i = (real_t **)malloc(nthread * sizeof(real_t *));
		if (simd == 0) {
			// matrix
			for (int i = 0; i < LElement; i++) {
				size = (i + 1) * sizeof(real_t);
				Am_r[i] = (real_t *)malloc(size);
				Am_i[i] = (real_t *)malloc(size);
				memset(Am_r[i], 0, size);
				memset(Am_i[i], 0, size);
			}
			// work vector
			for (int i = 0; i < nthread; i++) {
				size = LElement * sizeof(real_t);
				Wv_r[i] = (real_t *)malloc(size);
				Wv_i[i] = (real_t *)malloc(size);
				memset(Wv_r[i], 0, size);
				memset(Wv_i[i], 0, size);
			}
		}
#ifdef _SIMD
		else {
			const int unitbyte = 16 * simd;  // SSE/AVX=16/32bytes=128/256bit
			const int unitnum = unitbyte / sizeof(real_t);
			// matrix
			for (int i = 0; i < LElement; i++) {
				const int len = (((i + 1) + (unitnum - 1)) / unitnum) * unitnum;
				size = len * sizeof(real_t);
				Am_r[i] = (real_t *)_mm_malloc(size, unitbyte);
				Am_i[i] = (real_t *)_mm_malloc(size, unitbyte);
				//Am_r[i] = _aligned_malloc(size, unitbyte);
				//Am_i[i] = _aligned_malloc(size, unitbyte);
				memset(Am_r[i], 0, size);
				memset(Am_i[i], 0, size);
			}
			// work vector
			for (int i = 0; i < nthread; i++) {
				const int len = ((LElement + (unitnum - 1)) / unitnum) * unitnum;
				size = len * sizeof(real_t);
				Wv_r[i] = (real_t *)_mm_malloc(size, unitbyte);
				Wv_i[i] = (real_t *)_mm_malloc(size, unitbyte);
				//Wv_r[i] = _aligned_malloc(size, unitbyte);
				//Wv_i[i] = _aligned_malloc(size, unitbyte);
				memset(Wv_r[i], 0, size);
				memset(Wv_i[i], 0, size);
			}
		}
#endif
		// vector
		size = LElement * sizeof(d_complex_t);
		Bv = (d_complex_t *)malloc(size);
		Wv = (d_complex_t *)malloc(size);
		Dv = (d_complex_t *)malloc(size);
	}
}

// free
void memfree(int step, int nthread, int simd)
{
	if (step == 2) {
		// matrix and work vector
		if (simd == 0) {
			for (int n = 0; n < LElement; n++) {
				free(Am_r[n]);
				free(Am_i[n]);
			}
			for (int i = 0; i < nthread; i++) {
				free(Wv_r[i]);
				free(Wv_i[i]);
			}
		}
#ifdef _SIMD
		else {
			for (int n = 0; n < LElement; n++) {
				_mm_free(Am_r[n]);
				_mm_free(Am_i[n]);
			}
			for (int i = 0; i < nthread; i++) {
				_mm_free(Wv_r[i]);
				_mm_free(Wv_i[i]);
			}
		}
#endif
		free(Am_r);
		free(Am_i);
		free(Wv_r);
		free(Wv_i);

		// vector
		free(Bv);
		free(Wv);
		free(Dv);
	}
	else if (step == 1) {
		free(Element);
		free(Zin);
		free(Cv);
	}
}
