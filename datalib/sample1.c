/*
sample1.c

OpenMOMデータ作成ライブラリ、サンプルプログラム No.1

コンパイル+実行:
Windows + VC++:
> cl.exe /O2 sample1.c omm_datalib.c
> sample1.exe
Linux + gcc:
$ gcc -O sample1.c omm_datalib.c -o sample1
$ ./sample1
*/

#include "omm_datalib.h"

int main(void)
{
	// initialize

	omm_init();

	// title

	omm_title("sample1");

	// geometry

	omm_geometry_zline(-25e-3, +25e-3, 0e-3, 0e-3, 25);  // dipole
	omm_feed(1, 0);

	// frequency

	omm_frequency(2e9, 3e9, 10);

	// current distribution

	//omm_plotcurrent(1, 1);

	// frequency char.

	omm_plotsmith();
	omm_plotzin(1, 0, 0, 0);
	omm_plotyin(1, 0, 0, 0);
	omm_plotref(1, 0, 0, 0);

	// far0d

	//omm_plotfar0d(90, 0, 1, 0, 0, 0);

	// far1d

	omm_plotfar1d('X', 180, 0);
	//omm_far1dstyle(0);
	//omm_far1dcomponent(1, 0, 0);
	//omm_far1ddb(1);
	//omm_far1dnorm();
	//omm_far1dscale(-30, +10, 4);

	// far2d

	//omm_plotfar2d(18, 36);
	//omm_far2dcomponent(1, 0, 0, 0, 0, 0, 0);
	//omm_far2ddb(1);
	//omm_far2dscale(-20, +10);
	//omm_far2dobj(0.5);

	// near1d

	//omm_plotnear1d("E", +50e-3, +50e-3, 0e-3, 0e-3, -50e-3, +50e-3, 100);
	//omm_near1ddb(1);
	//omm_near1dscale(-30, +20, 5);

	// near2d

	//omm_plotnear2d("E", 'X', 10e-3, -50e-3, +50e-3, -50e-3, +50e-3, 20, 20);
	//omm_near2ddb(1);
	//omm_near2dscale(-30, +20);
	//omm_near2dcontour(0);
	//omm_near2dobj(1);

	// output

	omm_outdata("sample1.omm");

	return 0;
}
