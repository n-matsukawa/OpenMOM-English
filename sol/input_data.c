/*
input_data.c

input data
*/

#include "omm.h"
#include "omm_prototype.h"

static int datageometry(int, char *[], geometry_t []);

#define MAXTOKEN 100

int input_data(FILE *fp)
{
	int    version = 0;
	char   strline[BUFSIZ], strkey[BUFSIZ], strsave[BUFSIZ];
	char   *token[MAXTOKEN];
	const char sep[] = " \t";     // separator
	const int array_inc = 1000;   // reduce alloc times
	int    array_siz = array_inc;
	int    iradiusall = 0;
	double radiusall = 0.2;       // default ratio = radius / length
	int    ngeom = 1;

	// initialize

	Geometry = (geometry_t *)malloc(array_inc * sizeof(geometry_t));

	strcpy(Title, "");
	NGeometry = 0;
	NFrequency = 0;
	IPlanewave = 0;

	Z0 = 50;
	IGround = 0;

	Plot3dGeom = 0;

	cGeom3d = 0.05;
	pGeom3d[0] = 1;
	pGeom3d[1] = 0;

	// read

	int nline = 0;
	while (fgets(strline, sizeof(strline), fp) != NULL) {
		// skip a vacant line
		if (strlen(strline) <= 1) continue;

		// skip a comment line
		if (strline[0] == '#') continue;

		// delete "\n"
		//printf("%ld\n", strlen(strline));
		if (strstr(strline, "\r\n") != NULL) {
			strline[strlen(strline) - 2] = '\0';
		}
		else if ((strstr(strline, "\r") != NULL) || (strstr(strline, "\n") != NULL)) {
			strline[strlen(strline) - 1] = '\0';
		}
		//printf("%ld\n", strlen(strline));

		// "end" -> break
		if (!strcmp(strline, "end")) break;

		// save "strline"
		strcpy(strsave, strline);

		// token ("strline" is destroyed)
		int ntoken = tokenize(strline, sep, token, MAXTOKEN);
		//for (int i = 0; i < ntoken; i++) printf("%d %s\n", i, token[i]);

		// check number of data and "=" (exclude header)
		if ((nline > 0) && ((ntoken < 3) || strcmp(token[1], "="))) continue;

		// keyword
		strcpy(strkey, token[0]);

		if (nline == 0) {
			if (strcmp(strkey, "OpenMOM")) {
				fprintf(stderr, "*** not OpenMOM data\n");
				return 1;
			}
			if (ntoken > 1) {
				version = 10 * atoi(token[1]);  // major version
			}
			if (ntoken > 2) {
				version += atoi(token[2]);  // minor version
			}
			//printf("version=%d\n", version);
			nline++;
		}
		else if (!strcmp(strkey, "title")) {
			strcpy(Title, strchr(strsave, '=') + 2);
			//strncpy(Title, strchr(strsave, '=') + 2, sizeof(Title));
			//printf("title = %s\n", Title);
		}
		else if (!strcmp(strkey, "geometry")) {
			if ((ntoken < 4) || (atoi(token[2]) < 1) || (atoi(token[2]) > 2)) {
				fprintf(stderr, "*** invalid geometry data #%d\n", NGeometry + 1);
				return 1;
			}

			geometry_t g[6];
			ngeom = datageometry(ntoken, token, g);
			//printf("ngeom=%d\n", ngeom);
			if (ngeom < 1) {
				fprintf(stderr, "*** invalid geometry data #%d\n", NGeometry + 1);
				return 1;
			}
			assert(ngeom <= 6);

			// realloc
			//printf("%d+%d=%d/%d > %d/%d\n", NGeometry, ngeom, NGeometry + ngeom, array_inc, NGeometry, array_inc);
			if (((NGeometry + ngeom) / array_inc) > (NGeometry / array_inc)) {
				array_siz += array_inc;
				Geometry = (geometry_t *)realloc(Geometry, array_siz * sizeof(geometry_t));
			}
			assert(array_siz >= NGeometry + ngeom);

			for (int igeom = 0; igeom < ngeom; igeom++) {
				Geometry[NGeometry] = g[igeom];
				// initialize
				Geometry[NGeometry].ifeed = 0;
				Geometry[NGeometry].iload = 0;
				Geometry[NGeometry].iradius = 0;
				Geometry[NGeometry].offset[0] = Geometry[NGeometry].offset[1] = Geometry[NGeometry].offset[2] = 0;
				//printf("geometery : %d %d %d\n", NGeometry, Geometry[NGeometry].shape, Geometry[NGeometry].cosys);
				//printf("%.3e %.3e %.3e %.3e %.3e %.3e\n", Geometry[NGeometry].pos[0][0], Geometry[NGeometry].pos[0][1], Geometry[NGeometry].pos[0][2], Geometry[NGeometry].pos[1][0], Geometry[NGeometry].pos[1][1], Geometry[NGeometry].pos[1][2]);
				//printf("%.3e %.3e %.3e %.3e %.3e %.3e\n", Geometry[NGeometry].pos[2][0], Geometry[NGeometry].pos[2][1], Geometry[NGeometry].pos[2][2], Geometry[NGeometry].pos[3][0], Geometry[NGeometry].pos[3][1], Geometry[NGeometry].pos[3][2]);
				//printf("%d %d\n", Geometry[NGeometry].div[0], Geometry[NGeometry].div[1]);
				NGeometry++;
			}
		}
		else if (!strcmp(strkey, "feed")) {
			if ((ntoken < 4) || (NGeometry < 1)) {
				fprintf(stderr, "*** invalid feed data #%d\n", NGeometry);
				return 1;
			}
			Geometry[NGeometry - 1].ifeed = 1;
			for (int n = 0; n < 2; n++) {
				Geometry[NGeometry - 1].feed[n] = atof(token[2 + n]);
			}
			//printf("feed : %d %d %e %e\n", NGeometry, Geometry[NGeometry - 1].ifeed, Geometry[NGeometry - 1].feed[0], Geometry[NGeometry - 1].feed[1]);
		}
		else if (!strcmp(strkey, "load")) {
			if ((ntoken < 4) || (NGeometry < 1)
				|| ((atoi(token[2]) < 1) || (atoi(token[2]) > 3))) {
				fprintf(stderr, "*** invalid load data #%d\n", NGeometry);
				return 1;
			}
			Geometry[NGeometry - 1].iload = atoi(token[2]);
			Geometry[NGeometry - 1].load = atof(token[3]);
			//printf("load : %d %d %e\n", NGeometry, Geometry[NGeometry - 1].iload, Geometry[NGeometry - 1].load);
		}
		else if (!strcmp(strkey, "radius")) {
			if ((ntoken < 3) || (NGeometry < 1)) {
				fprintf(stderr, "*** invalid radius data #%d\n", NGeometry);
				return 1;
			}
			Geometry[NGeometry - 1].iradius = 1;
			Geometry[NGeometry - 1].radius = atof(token[2]);
			//printf("radius : %d %d %e\n", NGeometry, Geometry[NGeometry - 1].iradius, Geometry[NGeometry - 1].radius);
		}
		else if (!strcmp(strkey, "offset")) {
			if ((ntoken < 5) || (NGeometry < 1)) {
				fprintf(stderr, "*** invalid offset data #%d\n", NGeometry);
				return 1;
			}
			for (int ig = 1; ig <= ngeom; ig++) {
				for (int n = 0; n < 3; n++) {
					Geometry[NGeometry - ig].offset[n] = atof(token[2 + n]);
				}
			}
			//printf("offset : %d %e %e %e\n", NGeometry, Geometry[NGeometry - 1].offset[0], Geometry[NGeometry - 1].offset[1], Geometry[NGeometry - 1].offset[2]);
		}
		else if (!strcmp(strkey, "name")) {
			;
		}
		else if (!strcmp(strkey, "planewave")) {
			if (ntoken > 4) {
				IPlanewave = 1;
				Planewave.theta = atof(token[2]);
				Planewave.phi   = atof(token[3]);
				Planewave.pol   = atoi(token[4]);
				if ((Planewave.pol < 1) || (Planewave.pol > 5)) {
					fprintf(stderr, "*** invalid planewave polarization data\n");
					return 1;
				}
				if ((Planewave.pol == 5) && (ntoken > 6)) {
					Planewave.a = atof(token[5]);
					Planewave.r = atof(token[6]);
				}
			}
		}
		else if (!strcmp(strkey, "z0")) {
			Z0 = atof(token[2]);
		}
		else if (!strcmp(strkey, "ground")) {
			IGround = atoi(token[2]);
			if ((version < 42) && IGround) {
				IGround = 3;  // Z only
			}
		}
		else if (!strcmp(strkey, "radiusall")) {
			if (ntoken > 3) {
				// "1 radius" -> iradiusall = 1, radiusall = radius
				// "2 ratio"  -> iradiusall = 0, radiusall = ratio
				iradiusall = (atoi(token[2]) == 1) ? 1 : 0;
				radiusall = atof(token[3]);
			}
		}
		else if (!strcmp(strkey, "frequency")) {
			if (ntoken > 4) {
				double f0 = atof(token[2]);
				double f1 = atof(token[3]);
				int fdiv  = atoi(token[4]);
				if (fdiv < 0) {
					fprintf(stderr, "*** invalid frequncy data\n");
					return 1;
				}
				double df = (fdiv > 0) ? (f1 - f0) / fdiv : 0;
				NFrequency = fdiv + 1;
				Frequency = (double *)malloc(NFrequency * sizeof(double));
				for (int n = 0; n < NFrequency; n++) {
					Frequency[n] = f0 + (n * df);
				}
			}
		}
		else if (!strcmp(strkey, "plot3dgeom")) {
			Plot3dGeom = atoi(token[2]);
		}
		else if (!strcmp(strkey, "geom3dnode")) {
			if (ntoken > 4) {
				cGeom3d = atof(token[2]);
				pGeom3d[0] = atoi(token[3]);
				pGeom3d[1] = atoi(token[4]);
			}
		}
	}

	// feed -> no plane wave incidence
	if (NFeed) {
		IPlanewave = 0;
	}

	// error check
	if (!NGeometry) {
		fprintf(stderr, "*** No geometry data.\n");
		return 1;
	}
	if ((IGround < 0) || (IGround > 3)) {
		fprintf(stderr, "*** invalid ground.\n");
		return 1;
	}

	// default wire radius
	for (int n = 0; n < NGeometry; n++) {
		if (!Geometry[n].iradius) {
			Geometry[n].iradius = iradiusall;
			Geometry[n].radius = radiusall;
		}
	}

	// element size (reduce malloc)
	int isum = 0;
	for (int n = 0; n < NGeometry; n++) {
		if      (Geometry[n].shape == WIRE ) {
			isum += Geometry[n].div[0];
		}
		else if (Geometry[n].shape == PLANE) {
			isum += Geometry[n].div[0] * (Geometry[n].div[1] + 1)
			      + Geometry[n].div[1] * (Geometry[n].div[0] + 1);
		}
	}
	LElement = isum;

	return 0;
}


// make a wire geometry
static void wiregeometry(
	geometry_t *g,
	double x1, double x2,
	double y1, double y2,
	double z1, double z2,
	int div)
{
	// pos[0/1][0/1/2]
	// div[0]
	g->pos[0][0] = x1;
	g->pos[1][0] = x2;
	g->pos[0][1] = y1;
	g->pos[1][1] = y2;
	g->pos[0][2] = z1;
	g->pos[1][2] = z2;
	g->div[0] = div;
}


// make a plane geometry
static void planegeometry(
	geometry_t *g,
	double x1, double x2, double x3, double x4,
	double y1, double y2, double y3, double y4,
	double z1, double z2, double z3, double z4,
	int div1, int div2)
{
	// pos[0/1/2/3][0/1/2]
	// div[0/1]
	g->pos[0][0] = x1;
	g->pos[1][0] = x2;
	g->pos[2][0] = x3;
	g->pos[3][0] = x4;
	g->pos[0][1] = y1;
	g->pos[1][1] = y2;
	g->pos[2][1] = y3;
	g->pos[3][1] = y4;
	g->pos[0][2] = z1;
	g->pos[1][2] = z2;
	g->pos[2][2] = z3;
	g->pos[3][2] = z4;
	g->div[0] = div1;
	g->div[1] = div2;
}


/* parse geometry data
(1) wire unit
WIRE1  : 1 1 X1 X2 Y1 Y2 Z1 Z2 N
WIRE2  : 1 2 rho1 rho2 phi1 phi2 Z1 Z2 N
WIRE3  : 1 3 r1 r2 theta1 theta2 phi1 phi2 N
XLINE  : 1 11 X1 X2 Y Z NX
YLINE  : 1 12 Y1 Y2 Z X NY
ZLINE  : 1 13 Z1 Z2 X Y NZ
(2) plane unit
PLANE1 : 2 1 X1 X2 X3 X4 Y1 Y2 Y3 Y4 Z1 Z2 Z3 Z4 N12 N14
PLANE2 : 2 2 rho1 rho2 rho3 rho4 phi1 phi2 phi3 phi4 Z1 Z2 Z3 Z4 N12 N14
PLANE3 : 2 3 r1 r2 r3 r4 theta1 theta2 theta3 theta4 phi1 phi2 phi3 phi4 N12 N14
XRECT  : 2 11 X Y1 Y2 Z1 Z2 NY NZ
YRECT  : 2 12 Y Z1 Z2 X1 X2 NZ NX
ZRECT  : 2 13 Z X1 X2 Y1 Y2 NX NY
BOX1   : 2 21 X1 X2 Y1 Y2 Z1 Z2 NX NY NZ
BOX2   : 2 22 rho1 rho2 phi1 phi2 Z1 Z2 Nrho Nphi NZ
BOX3   : 2 23 r1 r2 theta1 theta2 phi1 phi2 Nr Ntheta Nphi
*/
static int datageometry(int ntoken, char *token[], geometry_t g[])
{
	int ngeom = 0;

	int gtype = atoi(token[2]);
	int gkind = atoi(token[3]);
	//printf("%d %d\n", gtype, gkind);

	if      (gtype == WIRE) {
		if ((gkind == WIRE1) || (gkind == WIRE2) || (gkind == WIRE3)) {
			if (ntoken < 11) return 0;
			if (atoi(token[10]) <= 0) return 0;
			g[0].shape = WIRE;
			g[0].cosys = gkind % 10;  // =1/2/3
			wiregeometry(g,
				atof(token[4]), atof(token[5]),
				atof(token[6]), atof(token[7]),
				atof(token[8]), atof(token[9]),
				atoi(token[10]));
			ngeom = 1;
		}
		else if (gkind == XLINE) {
			if (ntoken < 9) return 0;
			if (atoi(token[8]) <= 0) return 0;
			g[0].shape = WIRE;
			g[0].cosys = XYZ;
			wiregeometry(g,
				atof(token[4]), atof(token[5]),
				atof(token[6]), atof(token[6]),
				atof(token[7]), atof(token[7]),
				atoi(token[8]));
			ngeom = 1;
		}
		else if (gkind == YLINE) {
			if (ntoken < 9) return 0;
			if (atoi(token[8]) <= 0) return 0;
			g[0].shape = WIRE;
			g[0].cosys = XYZ;
			wiregeometry(g,
				atof(token[7]), atof(token[7]),
				atof(token[4]), atof(token[5]),
				atof(token[6]), atof(token[6]),
				atoi(token[8]));
			ngeom = 1;
		}
		else if (gkind == ZLINE) {
			if (ntoken < 9) return 0;
			if (atoi(token[8]) <= 0) return 0;
			g[0].shape = WIRE;
			g[0].cosys = XYZ;
			wiregeometry(g,
				atof(token[6]), atof(token[6]),
				atof(token[7]), atof(token[7]),
				atof(token[4]), atof(token[5]),
				atoi(token[8]));
			ngeom = 1;
		}
	}
	else if (atoi(token[2]) == PLANE) {
		if ((gkind == PLANE1) || (gkind == PLANE2) || (gkind == PLANE3)) {
			if (ntoken < 18) return 0;
			if ((atoi(token[16]) <= 0) || (atoi(token[17]) <= 0)) return 0;
			g[0].shape = PLANE;
			g[0].cosys = gkind % 10;
			planegeometry(g,
				atof(token[4]),  atof(token[5]),  atof(token[6]),  atof(token[7]),
				atof(token[8]),  atof(token[9]),  atof(token[10]), atof(token[11]),
				atof(token[12]), atof(token[13]), atof(token[14]), atof(token[15]),
				atoi(token[16]), atoi(token[17]));
			ngeom = 1;
		}
		else if (gkind == XRECT) {
			if (ntoken < 11) return 0;
			if ((atoi(token[9]) <= 0) || (atoi(token[10]) <= 0)) return 0;
			g[0].shape = PLANE;
			g[0].cosys = XYZ;
			planegeometry(g,
				atof(token[4]),  atof(token[4]),  atof(token[4]),  atof(token[4]),
				atof(token[5]),  atof(token[6]),  atof(token[6]),  atof(token[5]),
				atof(token[7]),  atof(token[7]),  atof(token[8]),  atof(token[8]),
				atoi(token[9]),  atoi(token[10]));
			ngeom = 1;
		}
		else if (gkind == YRECT) {
			if (ntoken < 11) return 0;
			if ((atoi(token[9]) <= 0) || (atoi(token[10]) <= 0)) return 0;
			g[0].shape = PLANE;
			g[0].cosys = XYZ;
			planegeometry(g,
				atof(token[7]),  atof(token[7]),  atof(token[8]),  atof(token[8]),
				atof(token[4]),  atof(token[4]),  atof(token[4]),  atof(token[4]),
				atof(token[5]),  atof(token[6]),  atof(token[6]),  atof(token[5]),
				atoi(token[9]),  atoi(token[10]));
			ngeom = 1;
		}
		else if (gkind == ZRECT) {
			if (ntoken < 11) return 0;
			if ((atoi(token[9]) <= 0) || (atoi(token[10]) <= 0)) return 0;
			g[0].shape = PLANE;
			g[0].cosys = XYZ;
			planegeometry(g,
				atof(token[5]),  atof(token[6]),  atof(token[6]),  atof(token[5]),
				atof(token[7]),  atof(token[7]),  atof(token[8]),  atof(token[8]),
				atof(token[4]),  atof(token[4]),  atof(token[4]),  atof(token[4]),
				atoi(token[9]),  atoi(token[10]));
			ngeom = 1;
		}
		else if ((gkind == BOX1) || (gkind == BOX2) || (gkind == BOX3)) {
			if (ntoken < 13) return 0;
			if ((atoi(token[10]) <= 0) || (atoi(token[11]) <= 0) || (atoi(token[12]) <= 0)) return 0;
			g[0].shape = g[1].shape = g[2].shape = g[3].shape = g[4].shape = g[5].shape = PLANE;
			g[0].cosys = g[1].cosys = g[2].cosys = g[3].cosys = g[4].cosys = g[5].cosys = gkind % 10;
			const double x1 = atof(token[4]);
			const double x2 = atof(token[5]);
			const double y1 = atof(token[6]);
			const double y2 = atof(token[7]);
			const double z1 = atof(token[8]);
			const double z2 = atof(token[9]);
			const int divx = atoi(token[10]);
			const int divy = atoi(token[11]);
			const int divz = atoi(token[12]);
			planegeometry(&g[0], x1, x1, x1, x1, y1, y2, y2, y1, z1, z1, z2, z2, divy, divz);  // -X
			planegeometry(&g[1], x2, x2, x2, x2, y1, y2, y2, y1, z1, z1, z2, z2, divy, divz);  // +X
			planegeometry(&g[2], x1, x1, x2, x2, y1, y1, y1, y1, z1, z2, z2, z1, divz, divx);  // -Y
			planegeometry(&g[3], x1, x1, x2, x2, y2, y2, y2, y2, z1, z2, z2, z1, divz, divx);  // +Y
			planegeometry(&g[4], x1, x2, x2, x1, y1, y1, y2, y2, z1, z1, z1, z1, divx, divy);  // -Z
			planegeometry(&g[5], x1, x2, x2, x1, y1, y1, y2, y2, z2, z2, z2, z2, divx, divy);  // +Z
			ngeom = 6;
		}
	}

	return ngeom;
}
