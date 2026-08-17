/*
monitor.c
*/

#include "omm.h"
#include "complex.h"
#include "omm_prototype.h"

// title or message
static void monitor1_(FILE *fp, const char msg[])
{
	if (fp == NULL) return;

	fprintf(fp, "%s\n", msg);
	fflush(fp);
}


// condition
static void monitor2_(FILE *fp, int nthread, int simd)
{
	if (fp == NULL) return;

	const char *strsimd[] = {"", " (SSE)", " (AVX)"};
	const char *strgnd[] = {"NO", "X=0", "Y=0", "Z=0"};
	const char *strpol[] = {"V", "H", "RHCP", "LHCP", "Elliptical"};
	char byte[BUFSIZ];
	strcpy(byte, (sizeof(real_t) == 4) ? "single" : "double");
	time_t now;
	time(&now);

	int nfeed = 0;
	int nload = 0;
	int nopen = 0;
	for (int n = 0; n < NElement; n++) {
		if (Element[n].ifeed) nfeed++;
		if (Element[n].iload) nload++;
		if (Element[n].iopen) nopen++;
	}

	fprintf(fp, "No. of threads     = %d%s (%s)\n", nthread, strsimd[simd], byte);
	fprintf(fp, "%s", ctime(&now));
	fprintf(fp, "Title = %s\n", Title);
	fprintf(fp, "No. of geometries  = %d\n", NGeometry);
	fprintf(fp, "No. of elements    = %d\n", NElement);
	fprintf(fp, "No. of feeds       = %d\n", nfeed);
	fprintf(fp, "No. of loads       = %d\n", nload);
	fprintf(fp, "No. of open ends   = %d\n", nopen);
	fprintf(fp, "Ground             = %s\n", strgnd[IGround]);
	fprintf(fp, "Plane wave         = %s\n", (!IPlanewave ? "NO" : strpol[Planewave.pol - 1]));
	fprintf(fp, "Memory size [MB]   = %d\n", (int)(1e-6 * sizeof(real_t) * NElement * NElement) + 1);
	fprintf(fp, "No. of frequencies = %d\n", NFrequency);
	fflush(fp);
}


// input impedance
static void monitor3_(FILE *fp)
{
	if (fp == NULL) return;

	if (NFeed) {
		const d_complex_t z0 = d_complex(Z0, 0);
		fprintf(fp, "=== input impedance (Z0=%.1f[ohm]) ===\n", Z0);
		for (int ifeed = 0; ifeed < NFeed; ifeed++) {
			fprintf(fp, "  feed# = %d\n", ifeed + 1);
			fprintf(fp, "  %s\n", "frequency[Hz] Rin[ohm]   Xin[ohm]    Gin[mS]    Bin[mS]    Ref[dB]       VSWR");
			for (int ifreq = 0; ifreq < NFrequency; ifreq++) {
				const d_complex_t zin = Zin[(ifeed * NFrequency) + ifreq];
				const d_complex_t yin = d_inv(zin);
				const double gamma = d_abs(d_div(d_sub(zin, z0), d_add(zin, z0)));
				const double ref = 20 * log10(MAX(gamma, EPS));
				const double vswr = (fabs(1 - gamma) > EPS) ? (1 + gamma) / (1 - gamma) : 1000;
				fprintf(fp, "%13.5e%11.3f%11.3f%11.3f%11.3f%11.3f%11.3f\n",
					Frequency[ifreq], zin.r, zin.i, yin.r * 1e3, yin.i * 1e3, ref, vswr);
			}
		}
	}
	else if (IPlanewave) {
		double e[7];
		d_complex_t etheta, ephi;
		fprintf(fp, "%s\n", "=== cross section ===");
		fprintf(fp, "  %s\n", "frequency[Hz] backward[m*m]    forward[m*m]");
		for (int ifreq = 0; ifreq < NFrequency; ifreq++) {
			const double fctr = farfactor(ifreq);
			// BCS
			farfield(ifreq, Planewave.theta, Planewave.phi, fctr, &etheta, &ephi);
			farComponent(etheta, ephi, e);
			const double bcs = e[0] * e[0];
			// FCS
			farfield(ifreq, 180 - Planewave.theta, Planewave.phi + 180, fctr, &etheta, &ephi);
			farComponent(etheta, ephi, e);
			const double fcs = e[0] * e[0];
			fprintf(fp, "%13.5e%16.5e%16.5e\n", Frequency[ifreq], bcs, fcs);
		}
	}

	fflush(fp);
}


// output files
static void monitor4_(FILE *fp)
{
	fprintf(fp, "=== output files ===\n");
	fprintf(fp, "%s, %s, %s, %s\n", FN_log, FN_out, FN_element, FN_current);
	fflush(fp);
}


// cpu time
static void monitor5_(FILE *fp, const double cpu[])
{
	if (fp == NULL) return;

	time_t now;
	time(&now);

	fprintf(fp, "%s\n", "=== normal end ===");
	fprintf(fp, "%s", ctime(&now));
	fprintf(fp, "%s\n", "=== cpu time [sec] ===");
	fprintf(fp, "part-1 : %10.3f\n", cpu[0]);
	fprintf(fp, "part-2 : %10.3f\n", cpu[1]);
	fprintf(fp, "part-3 : %10.3f\n", cpu[2]);
	fprintf(fp, "part-4 : %10.3f\n", cpu[3]);
	fprintf(fp, "%s\n", "---------------------");
	fprintf(fp, "total  : %10.3f\n", cpu[0] + cpu[1] + cpu[2] + cpu[3]);
	fflush(fp);
}


void monitor1(FILE *fp, const char msg[])
{
	monitor1_(fp,     msg);
	monitor1_(stdout, msg);
}


void monitor2(FILE *fp, int nthread, int simd)
{
	monitor2_(fp,     nthread, simd);
	monitor2_(stdout, nthread, simd);
}


void monitor3(FILE *fp)
{
	monitor3_(fp);
	monitor3_(stdout);
}


void monitor4(FILE *fp)
{
	monitor4_(fp);
	monitor4_(stdout);
}


void monitor5(FILE *fp, const double cpu[])
{
	monitor5_(fp,     cpu);
	monitor5_(stdout, cpu);
}
