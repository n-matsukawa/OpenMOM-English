/*
sample3.c

OpenMOMデータ作成ライブラリ、サンプルプログラム No.3

コンパイル+実行:
Windows + VC++:
> cl /O2 sample3.c omm_datalib.c
> sample2.exe
Linux + gcc:
$ gcc -O sample3.c omm_datalib.c -o sample2
$ ./sample2
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "omm_datalib.h"

int main(void)
{
	const double H = 10e-3;   // 高さ[m]
	const double L = 30e-3;   // 長さ[m]
	const double d = 1e-3;    // 要素長[m]
	const double x0 =  5e-3;  // 給電線開始位置[m]
	const double x1 = 15e-3;  // 給電線終了位置[m]
	const int div = 5;         // 位置分割数
	const double freq1 = 2e9;  // 開始周波数[Hz]
	const double freq2 = 3e9;  // 終了周波数{Hz]
	const int freqdiv = 20;    // 周波数分割数
	const char name[] = "sample3";
	char fn_data[BUFSIZ], title[BUFSIZ], cmd[BUFSIZ], str[BUFSIZ];

	// delete output files

	sprintf(str, "%s.log", name);
	remove(str);

#ifdef _WIN32
	sprintf(str, "%s.ev2", name);
	remove(str);
#endif

	// loop

	for (int loop = 0; loop <= div; loop++) {

		// initialize

		omm_init();

		// title

		sprintf(title, "%s_%03d", name, loop);
		omm_title(title);

		// geometry

		// グラウンド
		omm_ground();

		// 短絡線
		omm_geometry_zline(0, H, 0, 0, NINT(H, d));

		// 放射線
		omm_geometry_xline(0, L, 0, H, NINT(L, d));

		// 給電線
		const double x = x0 + (x1 - x0) * loop / div;
		omm_geometry_zline(0, d, x, 0, 1);
		omm_feed(1, 0);
		omm_geometry_zline(d, H, x, 0, NINT(H, d) - 1);

		// frequency

		omm_frequency(freq1, freq2, freqdiv);

		// frequency char.

		//omm_plotsmith();
		//omm_plotzin(1, 0, 0, 0);
		omm_plotyin(2, -50e-3, 50e-3, 10);
		//omm_plotref(1, 0, 0, 0);

		// far1d field

		//omm_plotfar1d('X', 72, 0);

		//omm_far1dstyle(0);
		//omm_far1dcomponent(1, 0, 0);
		//omm_far1ddb(1);
		//omm_far1dscale(-30, +10, 4);

		// output

		sprintf(fn_data, "%s_%03d.omm", name, loop);
		omm_outdata(fn_data);

		// solver, post, append result

#ifdef _WIN32
		sprintf(cmd, "omm.exe -n 8 %s", fn_data);
		system(cmd);

		sprintf(cmd, "omm_post.exe -n 8 %s", fn_data);
		system(cmd);

		sprintf(cmd, "type omm.log >> %s.log", name);
		system(cmd);

		sprintf(cmd, "type ev.ev2 >> %s.ev2", name);
		system(cmd);
#else
		sprintf(cmd, "./omm -n 8 %s", fn_data);
		system(cmd);

		sprintf(cmd, "./omm_post -n 8 -html %s", fn_data);
		system(cmd);

		sprintf(cmd, "cat omm.log >> %s.log", name);
		system(cmd);

		sprintf(str, "%s_%03d.htm", name, loop);
		remove(str);
		rename("ev2d.htm", str);
#endif
	}

	return 0;
}
