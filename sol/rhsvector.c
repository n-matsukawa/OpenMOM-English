/*
rhsvector.c

setup RHS vector
*/

#include "omm.h"
#include "complex.h"
#include "omm_prototype.h"

void rhsvector(int ifreq)
{
	for (int ie = 0; ie < NElement; ie++) {
		Bv[ie].r = Bv[ie].i = 0;
	}

	if (NFeed) {
		// feed
		for (int ie = 0; ie < NElement; ie++) {
			if (Element[ie].ifeed) {
				Bv[ie].r = Element[ie].feed[0] * cos(Element[ie].feed[1] * DTOR);
				Bv[ie].i = Element[ie].feed[0] * sin(Element[ie].feed[1] * DTOR);
			}
		}
	}
	else if (IPlanewave) {
		// plane wave
		d_complex_t ei[3], hi[3];
		const double kwave = (2 * PI * Frequency[ifreq]) / C;
		for (int ie = 0; ie < NElement; ie++) {
			planewave(Element[ie].pos[0], ei, hi, Planewave.theta, Planewave.phi, Planewave.pol, Planewave.a, Planewave.r, kwave);
			const double ctmp_r
				= (ei[0].r * Element[ie].tan[0])
				+ (ei[1].r * Element[ie].tan[1])
				+ (ei[2].r * Element[ie].tan[2]);
			const double ctmp_i
				= (ei[0].i * Element[ie].tan[0])
				+ (ei[1].i * Element[ie].tan[1])
				+ (ei[2].i * Element[ie].tan[2]);
			Bv[ie].r += Element[ie].lng[0] * ctmp_r;
			Bv[ie].i += Element[ie].lng[0] * ctmp_i;
		}
	}

	// check zero source
	double sum = 0;
	for (int ie = 0; ie < NElement; ie++) {
		sum += d_abs(Bv[ie]);
	}
	if (sum < EPS) {
		printf("%s\n", "*** no source.");
		//exit(1);
	}
}
