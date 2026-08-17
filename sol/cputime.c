/*
cputime.c

get current cpu time [sec]
*/

#include <time.h>

double cputime(void)
{
#ifdef _WIN32
	return (double)clock() / CLOCKS_PER_SEC;
#else
	// link option : -lrt (< glibc 2.17)
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return (ts.tv_sec + (ts.tv_nsec * 1e-9));
#endif
}
