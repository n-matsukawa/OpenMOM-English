/*
plot3dGeom.c

plot geometry 3D
*/

#include "omm.h"
#include "ev.h"

void plot3dGeom(void)
{
	// initialize
	ev3d_init();

	// new page
	ev3d_newPage();

	// wire elements
	for (int ie = 0; ie < NElement; ie++) {
		element_t *e = &Element[ie];
		if      (e->ifeed) {
			ev3d_setColor(255, 0, 0);
		}
		else if (e->iload) {
			ev3d_setColor(0, 255, 0);
		}
		else {
			ev3d_setColor(0, 0, 0);
		}
		const double x1 = e->pos[1][0];
		const double y1 = e->pos[1][1];
		const double z1 = e->pos[1][2];
		const double x2 = e->pos[2][0];
		const double y2 = e->pos[2][1];
		const double z2 = e->pos[2][2];
		ev3d_drawLine(x1, y1, z1, x2, y2, z2);

		// node
		if ((cGeom3d > 0) && ((e->wire && pGeom3d[0]) || (!e->wire && pGeom3d[1]))) {
			const double dh = cGeom3d * e->lng[0];
			ev3d_setColor(0, 0, 0);
			for (int k = 1; k <= 2; k++) {
				const double x = e->pos[k][0];
				const double y = e->pos[k][1];
				const double z = e->pos[k][2];
				ev3d_drawBox(x - dh, y - dh, z - dh, x + dh, y + dh, z + dh);
			}
		}
	}

	// ground
	if (IGround) {
		// bounding box
		double dmin[] = {+1e10, +1e10, +1e10};
		double dmax[] = {-1e10, -1e10, -1e10};
		for (int ie = 0; ie < NElement; ie++) {
			for (int m = 0; m < 3; m++) {
				const double dp = Element[ie].pos[1][m];  // minus end
				const double dm = Element[ie].pos[2][m];  // plus end
				dmin[m] = MIN(dmin[m], MIN(dp, dm));
				dmax[m] = MAX(dmax[m], MAX(dp, dm));
			}
		}
		// abs
		for (int m = 0; m < 3; m++) {
			dmin[m] = fabs(dmin[m]);
			dmax[m] = fabs(dmax[m]);
		}
		// half length
		const double h =
			MAX(dmin[0], MAX(dmin[1], MAX(dmin[2], MAX(dmax[0], MAX(dmax[1], dmax[2])))));
		//printf("%f %f %f %f %f %f %f\n", dmin[0], dmin[1], dmin[2], dmax[0], dmax[1], dmax[2], h);
		// draw
		const char dir = (IGround == 1) ? 'X' : (IGround == 2) ? 'Y' : 'Z';
		ev3d_setColor(128, 128, 128);
		ev3d_drawRectangle(dir, 0, -h, -h, +h, +h);
	}

	// title
	char str[BUFSIZ];
	ev3d_setColor(0, 0, 0);
	ev3d_drawTitle(Title);
	sprintf(str, "No. of geometries = %d", NGeometry);
	ev3d_drawTitle(str);
	sprintf(str, "No. of elements = %d", NElement);
	ev3d_drawTitle(str);

	// output HTML
	ev3d_html_size(500, 500);
	ev3d_file(0, FN_geom3d_0, 0);
	ev3d_output();

	// output ev3
	ev3d_file(1, FN_geom3d_1, 0);
	ev3d_output();

	// message
	printf("output : %s, %s\n", FN_geom3d_0, FN_geom3d_1);
}
