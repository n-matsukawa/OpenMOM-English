/*
writeout.c

write omm.out
*/

#include "omm.h"

void writeout(FILE *fp)
{
	size_t num = 0;

	num += fwrite(Title,       sizeof(char),        256,                   fp);
	num += fwrite(&NElement,   sizeof(int),         1,                     fp);
	num += fwrite(&NFrequency, sizeof(int),         1,                     fp);
	num += fwrite(&NFeed,      sizeof(int),         1,                     fp);
	num += fwrite(&IPlanewave, sizeof(int),         1,                     fp);
	num += fwrite(&IGround,    sizeof(int),         1,                     fp);
	num += fwrite(&Z0,         sizeof(double),      1,                     fp);
	num += fwrite(&Planewave,  sizeof(planewave_t), 1,                     fp);
	num += fwrite(Element,     sizeof(element_t),   NElement,              fp);
	num += fwrite(Frequency,   sizeof(double),      NFrequency,            fp);
	num += fwrite(Cv,          sizeof(d_complex_t), NFrequency * NElement, fp);
	num += fwrite(Zin,         sizeof(d_complex_t), NFeed * NFrequency,    fp);

	fwrite(&num, sizeof(size_t), 1, fp);
}
