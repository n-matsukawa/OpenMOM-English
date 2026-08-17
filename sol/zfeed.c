/*
zfeed.c

input impedance
*/

#include "omm.h"
#include "complex.h"

void zfeed(void)
{
	int id = 0;
	for (int ie = 0; ie < NElement; ie++) {
		if (Element[ie].ifeed) {
			d_complex_t vin = d_complex(Element[ie].feed[0] * cos(Element[ie].feed[1] * DTOR),
			                            Element[ie].feed[0] * sin(Element[ie].feed[1] * DTOR));
			for (int ifreq = 0; ifreq < NFrequency; ifreq++) {
				Zin[id++] = d_div(vin, Cv[(ifreq * NElement) + ie]);
			}
		}
	}
}
