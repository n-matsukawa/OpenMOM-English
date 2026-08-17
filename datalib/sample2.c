/*
sample2.c

OpenMOMデータ作成ライブラリ、サンプルプログラム No.2

コンパイル+実行:
Windows + VC++:
> cl /O2 sample2.c omm_datalib.c
> sample2.exe
Linux + gcc:
$ gcc -O sample2.c omm_datalib.c -o sample2
$ ./sample2
*/

#include "omm_datalib.h"

int main(void)
{
	const double ha = 25e-3;   // アンテナ高さ[m]
	const double lx = 50e-3;   // グラウンドX長さ[m]
	const double ly = 50e-3;   // グラウンドY長さ[m]
	const double lz = 25e-3;   // グラウンドZ長さ[m]
	const int nx = 10;         // グラウンドX分割数
	const int ny = 10;         // グラウンドY分割数
	const int nz = 5;          // グラウンドZ分割数
	const double freq1 = 2e9;  // 開始周波数[Hz]
	const double freq2 = 4e9;  // 終了周波数{Hz]
	const int fdiv = 4;        // 周波数分割数
	const char title[] = "sample2";
	const char fnout[] = "sample2.omm";

	// initialize

	omm_init();

	// title

	omm_title(title);

	// geometry

	omm_geometry_zline(0, ha / 10, 0, 0, 1);  // feed
	omm_feed(1, 0);

	omm_geometry_zline(ha / 10, ha, 0, 0, 9);  // monopole

	omm_geometry_box(1, -lx / 2, +lx / 2, -ly / 2, +ly / 2, 0, -lz, nx, ny, nz);  // ground

	// frequency

	omm_frequency(freq1, freq2, fdiv);

	// current distribution

	//omm_plotcurrent(1, 1);

	// frequency char.

	//omm_plotsmith();
	//omm_plotzin(1, 0, 0, 0);
	//omm_plotyin(1, 0, 0, 0);
	//omm_plotref(1, 0, 0, 0);

	// far0d

	//omm_plotfar0d(90, 0, 1, 0, 0, 0);

	// far1d

	omm_plotfar1d('X', 72, 0);
	//omm_far1dstyle(0);
	//omm_far1dcomponent(1, 0, 0);
	//omm_far1ddb(1);
	//omm_far1dscale(-30, +10, 4);

	// far2d

	omm_plotfar2d(18, 36);
	//omm_far2dcomponent(1, 0, 0, 0, 0, 0, 0);
	//omm_far2ddb(1);
	//omm_far2dscale(-20, +10);
	//omm_far2dobj(0.5);

	// near1d

	//omm_plotnear1d("E", +50e-3, +50e-3, 0e-3, 0e-3, -50e-3, +50e-3, 100);
	//omm_near1ddb(1);
	//omm_near1dscale(-30, +20, 5);

	// near2d

	//omm_plotnear2d("E", 'X', 30e-3, -50e-3, +50e-3, -50e-3, +50e-3, 20, 20);
	//omm_near2ddb(1);
	//omm_near2dscale(-30, +20);
	//omm_near2dcontour(0);
	//omm_near2dobj(2);

	// output

	omm_outdata(fnout);

	return 0;
}
