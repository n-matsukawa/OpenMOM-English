/*
OpenMOM Version 4.2.0

solver
*/

#define MAIN
#include "omm.h"
#undef MAIN

#include "omm_prototype.h"

static void args(int, char *[], int *, int *, int *, int *, int *, char []);

int main(int argc, char *argv[])
{
	const char errfmt[] = "*** file %s open error.\n";
	char str[BUFSIZ];
	double cpu[] = {0, 0, 0, 0};
	FILE *fp_in = NULL, *fp_out = NULL, *fp_log = NULL;

	// arguments
	int nthread = 1;
	int simd = 1;
	int prompt = 0;
	int check = 0;
	int zmatrix = 0;
	char fn_in[BUFSIZ] = "";
	args(argc, argv, &nthread, &simd, &prompt, &check, &zmatrix, fn_in);

#if defined(__NEC__) || defined(__FUJITSU)
	simd = 0;
#endif

	// set number of threads
#ifdef _OPENMP
	omp_set_num_threads(nthread);
#else
	nthread = 1;
#endif

	// input data
	if ((fp_in = fopen(fn_in, "r")) == NULL) {
		fprintf(stderr, errfmt, fn_in);
		exit(1);
	}
	if (input_data(fp_in)) {
		exit(1);
	}
	fclose(fp_in);

	// cpu time
	const double t0 = cputime();

	// open log file
	if ((fp_log = fopen(FN_log, "w")) == NULL) {
		fprintf(stderr, errfmt, FN_log);
		exit(1);
	}

	// monitor
	sprintf(str, "<<< %s Ver.%d.%d.%d >>>", PROGRAM, VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD);
	monitor1(fp_log, str);

	// alloc (1)
	memalloc(1, nthread, simd);

	// alloc (2)
	memalloc(2, nthread, simd);

	// make wire grid model
	wiregrid();

	// output element.log
	elementlog();

	// plot geometry and exit
	if (Plot3dGeom) {
		plot3dGeom();
		exit(0);
	}

	// monitor
	monitor2(fp_log, nthread, simd);

	// solve
	cpu[0] = cputime() - t0;
	const int monitor = 1;
	solve(nthread, simd, zmatrix, check, monitor, cpu);
	const double t3 = cputime();

	// output input impedance
	zfeed();
	monitor3(fp_log);

	// free (2)
	memfree(2, nthread, simd);

	// output current.log
	currentlog();

	// output files
	monitor4(fp_log);

	// write omm.out
	if ((fp_out = fopen(FN_out, "wb")) == NULL) {
		printf(errfmt, FN_out);
		exit(1);
	}
	writeout(fp_out);
	fclose(fp_out);

	// cpu time
	cpu[3] = cputime() - t3;
	monitor5(fp_log, cpu);

	// close log file
	fclose(fp_log);

	// free (1)
	//memfree(1, nthread, simd);

	// prompt
	if (prompt) getchar();

	return 0;
}

static void args(int argc, char *argv[],
	int *nthread, int *simd, int *prompt, int *check, int *zmatrix, char fn_in[])
{
	const char usage[] = "Usage : omm [-n <thread>] [-nosimd|-sse|-avx] [-zmatrix] <datafile>";

	if (argc < 2) {
		printf("%s\n", usage);
		exit(0);
	}

	while (--argc) {
		++argv;
		if (!strcmp(*argv, "-n")) {
			if (--argc) {
				*nthread = atoi(*++argv);
				if (*nthread <= 0) *nthread = 1;
			}
			else {
				break;
			}
		}
		else if (!strcmp(*argv, "-nosimd")) {
			*simd = 0;
		}
		else if (!strcmp(*argv, "-sse")) {
			*simd = 1;
		}
		else if (!strcmp(*argv, "-avx")) {
			*simd = 2;
		}
		else if (!strcmp(*argv, "-prompt")) {
			*prompt = 1;
		}
		else if (!strcmp(*argv, "-check")) {
			*check = 1;  // debug
		}
		else if (!strcmp(*argv, "-zmatrix")) {
			*zmatrix = 1;
		}
		else if (!strcmp(*argv, "--help")) {
			printf("%s\n", usage);
			exit(0);
		}
		else if (!strcmp(*argv, "--version")) {
			printf("%s Ver.%d.%d.%d\n", PROGRAM, VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD);
			exit(0);
		}
		else {
			strcpy(fn_in, *argv);
		}
	}
}
