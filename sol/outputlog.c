/*
outputlog.c
*/

#include "omm.h"
#include "complex.h"

// element data
void elementlog(void)
{

	FILE *fp;
	if ((fp = fopen(FN_element, "w")) == NULL) {
		printf("*** %s open error.\n", FN_element);
		return;
	}

	fprintf(fp, "%d\n", NElement);
	for (int ie = 0; ie < NElement; ie++) {
		element_t *e = &Element[ie];
		double lng = e->lng[0];
		fprintf(fp, "%6d%13.4e%13.4e%13.4e%13.4e%13.4e%13.4e%13.4e%2d\n",
			ie + 1, e->pos[0][0], e->pos[0][1], e->pos[0][2],
			lng * e->tan[0], lng * e->tan[1], lng * e->tan[2], e->radius, e->iopen);
	}

	fclose(fp);
}

// current data
void currentlog(void)
{
	FILE *fp;
	if ((fp = fopen(FN_current, "w")) == NULL) {
		printf("*** %s open error.\n", FN_current);
		return;
	}

	fprintf(fp, "%d %d\n", NElement, NFrequency);
	for (int ifreq = 0; ifreq < NFrequency; ifreq++) {
		for (int ie = 0; ie < NElement; ie++) {
			d_complex_t c = Cv[(ifreq * NElement) + ie];
			d_complex_t ctmp = d_rmul(1e3, c);
			int id = 0;
			if (Element[ie].ifeed) id += 1;
			if (Element[ie].iload) id += 2;
			fprintf(fp, "%6d%13.4e%9.3f%13.4e%13.4e%2d\n",
			        ie + 1, d_abs(ctmp), d_deg(ctmp), ctmp.r, ctmp.i, id);
		}
	}

	fclose(fp);
}
