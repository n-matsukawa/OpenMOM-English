/*
omm_datalib.c
OpenMOM datalib
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#define PROGRAM "OpenMOM 4 2"

int _MAXGEOMETRY;
int _MAXFAR1D;
int _MAXNEAR1D;
int _MAXNEAR2D;

typedef struct {
	int    type;              // 1=wire, 2=plane
	int    shape;             // wire:1/2/3/11/12/13, plane:1/2/3/11/12/13/21/22/23
	double pos[12];           // position
	int    div[3];            // division
	int    ifeed;             // feed 0/1
	double feed[2];           // amplitude[V], phase[deg]
	int    iload;             // load : 0/1/2/3=no/R/L/C
	double load;              // R/L/C
	int    iradius;           // radius 0/1
	double radius;            // radius
	double offset[3];         // X/Y/Z offset
	char   *name;             // (GUI) name
} geometry_t;                 // geometry

typedef struct {
	int    on;                // 0/1:Y/N
	double theta;             // theta [deg]
	double phi;               // phi [deg]
	int    pol;               // polarization : 1/2/3/4/5=V/H/R/L/E
	double a, r;              // E only : angle [deg], axial ratio
} planewave_t;                // plane wave incidence

typedef struct {
	char   dir;               // direction : X/Y/Z/V/H
	int    div;               // division of angle 360 deg
	double angle;             // V/H constant angle [deg]
} far1d_t;                    // far1d

typedef struct {
	char   cmp[3];            // component : E/Ex/Ey/Ez/H/Hx/Hy/Hz
	double pos[6];            // start and end position
	int    div;               // division
} near1d_t;                   // near1d

typedef struct {
	char   cmp[3];            // component : E/Ex/Ey/Ez/H/Hx/Hy/Hz
	char   dir;               // direction : X/Y/Z
	double pos[5];            // position [m]
	int    div[2];            // division
} near2d_t;                   // near2d

typedef struct {
	int    db;                // 0/1 : [V/m]/[dB]
	int    user;              // 0/1 : auto/user
	double min, max;          // min, max
	int    div;               // division
} scale_t;                    // scale

// solver

char        _title[BUFSIZ];
int         _geometrys;
geometry_t *_geometry;
planewave_t _planewave;
double      _fstart, _fend;
int         _fdiv;
int         _ground;
int         _iradiusall;
double      _radiusall;
double      _z0;
double      _geom3dnode_r;
int         _geom3dnode_i0, _geom3dnode_i1;

// post

int         _matchingloss;
int         _plotcurrent[2];
int         _plotsmith;
int         _plotzin;
int         _plotyin;
int         _plotref;
int         _freqdiv;
scale_t     _zinscale;
scale_t     _yinscale;
scale_t     _refscale;
int         _plotfar0d;
double      _plotfar0d_angle[2];
scale_t     _far0dscale;
int         _plotfar1ds;
far1d_t    *_plotfar1d;
int         _far1dstyle;
int         _far1dcomponent[3];
int         _far1dnorm;
int         _far1dabs;
scale_t     _far1dscale;
int         _plotfar2d;
int         _plotfar2d_div[2];
int         _far2dcomponent[7];
scale_t     _far2dscale;
double      _far2dobj;
int         _plotnear1ds;
near1d_t   *_plotnear1d;
scale_t     _near1dscale;
int         _near1dnoinc;
int         _plotnear2ds;
near2d_t   *_plotnear2d;
int         _near2ddim[2];
scale_t     _near2dscale;
int         _near2dnoinc;
int         _near2dcontour;
int         _near2dobj;
int         _near2dframe;


static void init_geometry_option(geometry_t *g)
{
	g->ifeed = 0;
	g->iload = 0;
	g->iradius = 0;
	g->offset[0] = g->offset[1] = g->offset[2] = 0;
	g->name = NULL;
}

// (0) initialize

void omm_init(void)
{
	_MAXGEOMETRY = 100000;
	_MAXFAR1D    = 1000;
	_MAXNEAR1D   = 1000;
	_MAXNEAR2D   = 1000;

	_geometrys = _plotfar1ds = _plotnear1ds = _plotnear2ds = 0;

	strcpy(_title, "");
	_fstart        = 3e9;
	_fend          = 3e9;
	_fdiv          = 0;
	_ground        = 0;
	_iradiusall    = 0;
	_radiusall     = 0.2;
	_z0            = 50;
	_geom3dnode_r  = 0.05;
	_geom3dnode_i0 = 1;
	_geom3dnode_i1 = 0;
	_planewave.on  = 0;

	_matchingloss =
	_plotcurrent[0] =
	_plotcurrent[1] =
	_plotsmith =
	_plotzin =
	_plotyin =
	_plotref =
	_plotfar0d =
	_plotfar1ds =
	_plotfar2d =
	_plotnear1ds =
	_plotnear2ds = 0;
	_freqdiv = 10;
	_far1dstyle = 0;
	_far1dcomponent[0] = 1; _far1dcomponent[1] = _far1dcomponent[2] = 0;
	_far1dnorm = 0;
	_far1dabs = 1;
	_far1dscale.user = 0;
	_far1dscale.db = 1;
	_far1dscale.min = _far1dscale.max = 0;
	_far1dscale.div = 1;
	_far2dcomponent[0] = 1; _far2dcomponent[1] = _far2dcomponent[2] = _far2dcomponent[3] = _far2dcomponent[4] = _far2dcomponent[5] = _far2dcomponent[6] = 0;
	_far2dscale.user = 0;
	_far2dscale.db = 1;
	_far2dscale.min = _far2dscale.max = 0;
	_far2dobj = 0.5;
	_near1dscale.user = 0;
	_near1dscale.db = 0;
	_near1dscale.min = _near1dscale.max = 0;
	_near1dscale.div = 10;
	_near1dnoinc = 0;
	_near2ddim[0] = _near2ddim[1] = 1;
	_near2dscale.user = 0;
	_near2dscale.db = 0;
	_near2dscale.min = _near2dscale.max = 0;
	_near2dnoinc = 0;
	_near2dcontour = 0;
	_near2dobj = 1;
	_near2dframe = 0;

	_geometry   = (geometry_t *)malloc(_MAXGEOMETRY * sizeof(geometry_t));
	_plotfar1d  = (far1d_t *)   malloc(_MAXFAR1D    * sizeof(far1d_t));
	_plotnear1d = (near1d_t *)  malloc(_MAXNEAR1D   * sizeof(near1d_t));
	_plotnear2d = (near2d_t *)  malloc(_MAXNEAR2D   * sizeof(near2d_t));
}

// (1) solver

void omm_title(const char title[])
{
	strcpy(_title, title);
}


void omm_geometry_wire(int cosys, double x1, double x2, double y1, double y2, double z1, double z2, int div)
{
	if (_geometrys >= _MAXGEOMETRY) {fprintf(stderr, "*** too many geometry.s > %d\n", _MAXGEOMETRY); exit(1);}
	if ((cosys < 1) || (cosys > 3)) {fprintf(stderr, "*** invalid coordinate at geometry #%d\n", _geometrys + 1); exit(1);}
	if (div <= 0) {fprintf(stderr, "*** invalid division at geometry #%d\n", _geometrys + 1); exit(1);}
	_geometry[_geometrys].type  = 1;
	_geometry[_geometrys].shape = cosys;
	_geometry[_geometrys].pos[0] = x1;
	_geometry[_geometrys].pos[1] = x2;
	_geometry[_geometrys].pos[2] = y1;
	_geometry[_geometrys].pos[3] = y2;
	_geometry[_geometrys].pos[4] = z1;
	_geometry[_geometrys].pos[5] = z2;
	_geometry[_geometrys].div[0] = div;
	init_geometry_option(&_geometry[_geometrys]);
	_geometrys++;
}


void omm_geometry_xline(double x1, double x2, double y, double z, int div)
{
	omm_geometry_wire(1, x1, x2, y, y, z, z, div);
}


void omm_geometry_yline(double y1, double y2, double z, double x, int div)
{
	omm_geometry_wire(1, x, x, y1, y2, z, z, div);
}


void omm_geometry_zline(double z1, double z2, double x, double y, int div)
{
	omm_geometry_wire(1, x, x, y, y, z1, z2, div);
}


void omm_geometry_triangle(double x1, double x2, double x3, double y1, double y2, double y3, double z1, double z2, double z3)
{
	omm_geometry_wire(1, x1, x2, y1, y2, z1, z2, 1);
	omm_geometry_wire(1, x2, x3, y2, y3, z2, z3, 1);
	omm_geometry_wire(1, x3, x1, y3, y1, z3, z1, 1);
}


void omm_geometry_plane(int cosys, double x1, double x2, double x3, double x4, double y1, double y2, double y3, double y4, double z1, double z2, double z3, double z4, int div1, int div2)
{
	if (_geometrys >= _MAXGEOMETRY) {fprintf(stderr, "*** too many geometry.s > %d\n", _MAXGEOMETRY); exit(1);}
	if ((cosys < 1) || (cosys > 3)) {fprintf(stderr, "*** invalid coordinate at geometry #%d\n", _geometrys + 1); exit(1);}
	if ((div1 <= 0) || (div2 <= 0)) {fprintf(stderr, "*** invalid division at geometry #%d\n", _geometrys + 1); exit(1);}
	_geometry[_geometrys].type  = 2;
	_geometry[_geometrys].shape = cosys;
	_geometry[_geometrys].pos[0]  = x1;
	_geometry[_geometrys].pos[1]  = x2;
	_geometry[_geometrys].pos[2]  = x3;
	_geometry[_geometrys].pos[3]  = x4;
	_geometry[_geometrys].pos[4]  = y1;
	_geometry[_geometrys].pos[5]  = y2;
	_geometry[_geometrys].pos[6]  = y3;
	_geometry[_geometrys].pos[7]  = y4;
	_geometry[_geometrys].pos[8]  = z1;
	_geometry[_geometrys].pos[9]  = z2;
	_geometry[_geometrys].pos[10] = z3;
	_geometry[_geometrys].pos[11] = z4;
	_geometry[_geometrys].div[0]  = div1;
	_geometry[_geometrys].div[1]  = div2;
	init_geometry_option(&_geometry[_geometrys]);
	_geometrys++;
}


void omm_geometry_xrect(double x, double y1, double y2, double z1, double z2, int div1, int div2)
{
	omm_geometry_plane(1, x, x, x, x, y1, y2, y2, y1, z1, z1, z2, z2, div1, div2);
}


void omm_geometry_yrect(double y, double z1, double z2, double x1, double x2, int div1, int div2)
{
	omm_geometry_plane(1, x1, x1, x2, x2, y, y, y, y, z1, z2, z2, z1, div1, div2);
}


void omm_geometry_zrect(double z, double x1, double x2, double y1, double y2, int div1, int div2)
{
	omm_geometry_plane(1, x1, x2, x2, x1, y1, y1, y2, y2, z, z, z, z, div1, div2);
}


void omm_geometry_box(int cosys, double x1, double x2, double y1, double y2, double z1, double z2, int div1, int div2, int div3)
{
	if (_geometrys >= _MAXGEOMETRY) {fprintf(stderr, "*** too many geometry.s > %d\n", _MAXGEOMETRY); exit(1);}
	if ((cosys < 1) || (cosys > 3)) {fprintf(stderr, "*** invalid coordinate at geometry #%d\n", _geometrys + 1); exit(1);}
	if ((div1 <= 0) || (div2 <= 0) || (div3 <= 0)) {fprintf(stderr, "*** invalid division at geometry #%d\n", _geometrys + 1); exit(1);}
	_geometry[_geometrys].type  = 2;
	_geometry[_geometrys].shape = 20 + cosys;
	_geometry[_geometrys].pos[0]  = x1;
	_geometry[_geometrys].pos[1]  = x2;
	_geometry[_geometrys].pos[2]  = y1;
	_geometry[_geometrys].pos[3]  = y2;
	_geometry[_geometrys].pos[4]  = z1;
	_geometry[_geometrys].pos[5]  = z2;
	_geometry[_geometrys].div[0] = div1;
	_geometry[_geometrys].div[1] = div2;
	_geometry[_geometrys].div[2] = div3;
	init_geometry_option(&_geometry[_geometrys]);
	_geometrys++;
}


void omm_feed(double v, double a)
{
	if (!_geometrys) {fprintf(stderr, "%s\n", "*** no geometry for feed"); exit(1);}
	if (_geometry[_geometrys - 1].type != 1) {fprintf(stderr, "*** feed on plane geometry #%d\n", _geometrys); exit(1);}
	_geometry[_geometrys - 1].ifeed = 1;
	_geometry[_geometrys - 1].feed[0] = v;
	_geometry[_geometrys - 1].feed[1] = a;
}


void omm_load(int iload, double load)
{
	if (!_geometrys) {fprintf(stderr, "%s\n", "*** no geometry for load"); exit(1);}
	if (_geometry[_geometrys - 1].type != 1) {fprintf(stderr, "*** load on plane geometry #%d\n", _geometrys); exit(1);}
	if ((iload < 1) || (iload > 3)) {fprintf(stderr, "*** invalid load at geometry #%d\n", _geometrys); exit(1);}
	_geometry[_geometrys - 1].iload = iload;
	_geometry[_geometrys - 1].load = load;
}


void omm_radius(double radius)
{
	if (!_geometrys) {fprintf(stderr, "%s\n", "*** no geometry for radius"); exit(1);}
	_geometry[_geometrys - 1].iradius = 1;
	_geometry[_geometrys - 1].radius = radius;
}


void omm_offset(double x, double y, double z)
{
	if (!_geometrys) {fprintf(stderr, "%s\n", "*** no geometry for offset"); exit(1);}
	_geometry[_geometrys - 1].offset[0] = x;
	_geometry[_geometrys - 1].offset[1] = y;
	_geometry[_geometrys - 1].offset[2] = z;
}


void omm_name(const char name[])
{
	if (!_geometrys) {fprintf(stderr, "%s\n", "*** no geometry for name"); exit(1);}
	size_t len = strlen(name);
	_geometry[_geometrys - 1].name = (char *)malloc((len + 1) * sizeof(char));
	strcpy(_geometry[_geometrys - 1].name, name);
}


void omm_planewave(double theta, double phi, int pol)
{
	_planewave.on = 1;
	_planewave.theta = theta;
	_planewave.phi = phi;
	_planewave.pol = pol;
}


void omm_planewave_ellipse(double theta, double phi, double a, double r)
{
	_planewave.on = 1;
	_planewave.theta = theta;
	_planewave.phi = phi;
	_planewave.pol = 5;
	_planewave.a = a;
	_planewave.r = r;
}


void omm_ground(void)
{
	_ground = 1;
}


void omm_frequency(double fstart, double fend, int div)
{
	_fstart = fstart;
	_fend   = fend;
	_fdiv   = div;
}


void omm_z0(double z0)
{
	_z0 = z0;
}


void omm_radiusall(int i, double r)
{
	_iradiusall = i;
	_radiusall = r;
}


void omm_geom3dnode(double r, int i0, int i1)
{
	_geom3dnode_r  = r;
	_geom3dnode_i0 = i0;
	_geom3dnode_i1 = i1;
}


void omm_geometry_disc(double x0, double y0, double z0, double r, int rdiv)
{
	const double pi = 4 * atan(1);

	omm_geometry_wire(2, r, r, 0, 360, 0, 0, 4 * rdiv);
	omm_offset(x0, y0, z0);

	omm_geometry_xline(x0 - r, x0 + r, y0, z0, 2 * rdiv);
	omm_geometry_yline(y0 - r, y0 + r, z0, x0, 2 * rdiv);

	for (int i = 1; i < rdiv; i++) {
		for (int j = 0; j < i; j++) {
			const double r1 = r * (i + 0) / rdiv;
			const double r2 = r1;
			const double r3 = r * (i + 1) / rdiv;
			const double a1 = (pi / 2) * (j + 0) / i;
			const double a2 = (pi / 2) * (j + 1) / i;
			const double a3 = (pi / 2) * (j + 1) / (i + 1);
			const double x1 = r1 * cos(a1);
			const double x2 = r2 * cos(a2);
			const double x3 = r3 * cos(a3);
			const double y1 = r1 * sin(a1);
			const double y2 = r2 * sin(a2);
			const double y3 = r3 * sin(a3);
			omm_geometry_triangle(x0 + x1, x0 + x2, x0 + x3, y0 + y1, y0 + y2, y0 + y3, z0, z0, z0);
			omm_geometry_triangle(x0 - x1, x0 - x2, x0 - x3, y0 + y1, y0 + y2, y0 + y3, z0, z0, z0);
			omm_geometry_triangle(x0 - x1, x0 - x2, x0 - x3, y0 - y1, y0 - y2, y0 - y3, z0, z0, z0);
			omm_geometry_triangle(x0 + x1, x0 + x2, x0 + x3, y0 - y1, y0 - y2, y0 - y3, z0, z0, z0);
		}
	}
}


void omm_geometry_bell(double x0, double y0, double za, double zb, double rb, int zdiv, int adiv)
{
	if ((za == zb) || (rb == 0) || (zdiv < 1) || (adiv < 1)) return;

	const double pi = 4 * atan(1);

	for (int iz = 1; iz <= zdiv; iz++) {
		const double z = za + (zb - za) * iz / zdiv;
		const double r = rb * (1 - (z - zb) * (z - zb) / (za - zb) / (za - zb));
		omm_geometry_wire(2, r, r, 0, 360, z, z, adiv);
		omm_offset(x0, y0, 0);
	}

	for (int ia = 0; ia < adiv; ia++) {
		const double a = (2 * pi) * ia / adiv;
		for (int iz = 0; iz < zdiv; iz++) {
			const double z1 = za + (zb - za) * (iz + 0) / zdiv;
			const double z2 = za + (zb - za) * (iz + 1) / zdiv;
			const double r1 = rb * (1 - (z1 - zb) * (z1 - zb) / (za - zb) / (za - zb));
			const double r2 = rb * (1 - (z2 - zb) * (z2 - zb) / (za - zb) / (za - zb));
			const double x1 = x0 + r1 * cos(a);
			const double x2 = x0 + r2 * cos(a);
			const double y1 = y0 + r1 * sin(a);
			const double y2 = y0 + r2 * sin(a);
			omm_geometry_wire(1, x1, x2, y1, y2, z1, z2, 1);
		}
	}
}

// (2) post

void omm_matchingloss(void)
{
	_matchingloss = 1;
}


void omm_plotcurrent(int i0, int i1)
{
	_plotcurrent[0] = i0;
	_plotcurrent[1] = i1;
}


void omm_plotsmith(void)
{
	_plotsmith = 1;
}


void omm_plotzin(int scale, double min, double max, int div)
{
	if      (scale == 1) {
		_plotzin = 1;
		_zinscale.user = 0;
	}
	else if (scale == 2) {
		_plotzin = 1;
		_zinscale.user = 1;
		_zinscale.min = min;
		_zinscale.max = max;
		_zinscale.div = div;
	}
}


void omm_plotyin(int scale, double min, double max, int div)
{
	if      (scale == 1) {
		_plotyin = 1;
		_yinscale.user = 0;
	}
	else if (scale == 2) {
		_plotyin = 1;
		_yinscale.user = 1;
		_yinscale.min = min;
		_yinscale.max = max;
		_yinscale.div = div;
	}
}


void omm_plotref(int scale, double min, double max, int div)
{
	if      (scale == 1) {
		_plotref = 1;
		_refscale.user = 0;
	}
	else if (scale == 2) {
		_plotref = 1;
		_refscale.user = 1;
		_refscale.min = min;
		_refscale.max = max;
		_refscale.div = div;
	}
}


void omm_plotfar0d(double theta, double phi, int scale, double min, double max, int div)
{
	if      (scale == 1) {
		_plotfar0d = 1;
		_plotfar0d_angle[0] = theta;
		_plotfar0d_angle[1] = phi;
		_far0dscale.user = 0;
	}
	else if (scale == 2) {
		_plotfar0d = 1;
		_plotfar0d_angle[0] = theta;
		_plotfar0d_angle[1] = phi;
		_far0dscale.user = 1;
		_far0dscale.min = min;
		_far0dscale.max = max;
		_far0dscale.div = div;
	}
}


void omm_freqdiv(int freqdiv)
{
	_freqdiv = freqdiv;
}


void omm_plotfar1d(char dir, int div, double angle)
{
	if (_plotfar1ds >= _MAXFAR1D) {fprintf(stderr, "*** too many plotfar1d.s > %d\n", _MAXFAR1D); exit(1);}
	_plotfar1d[_plotfar1ds].dir   = dir;
	_plotfar1d[_plotfar1ds].div   = div;
	_plotfar1d[_plotfar1ds].angle = angle;
	_plotfar1ds++;
}


void omm_far1dstyle(int i0)
{
	_far1dstyle = i0;
}


void omm_far1dcomponent(int i0, int i1, int i2)
{
	_far1dcomponent[0] = i0;
	_far1dcomponent[1] = i1;
	_far1dcomponent[2] = i2;
}


void omm_far1ddb(int i0)
{
	_far1dscale.db = (i0 == 0) ? 0 : 1;
}


void omm_far1dnorm(int i0)
{
	_far1dnorm = i0;
}


void omm_far1dabs(int i0)
{
	_far1dnorm = i0;
}


void omm_far1dscale(double min, double max, int div)
{
	_far1dscale.user = 1;
	_far1dscale.min  = min;
	_far1dscale.max  = max;
	_far1dscale.div  = div;
}


void omm_plotfar2d(int divtheta, int divphi)
{
	_plotfar2d = 1;
	_plotfar2d_div[0] = divtheta;
	_plotfar2d_div[1] = divphi;
}


void omm_far2dcomponent(int i0, int i1, int i2, int i3, int i4, int i5, int i6)
{
	_far2dcomponent[0] = i0;
	_far2dcomponent[1] = i1;
	_far2dcomponent[2] = i2;
	_far2dcomponent[3] = i3;
	_far2dcomponent[4] = i4;
	_far2dcomponent[5] = i5;
	_far2dcomponent[6] = i6;
}


void omm_far2ddb(int i0)
{
	_far2dscale.db = (i0 == 0) ? 0 : 1;
}


void omm_far2dscale(double min, double max)
{
	_far2dscale.user = 1;
	_far2dscale.min  = min;
	_far2dscale.max  = max;
}


void omm_far2dobj(double r0)
{
	_far2dobj = r0;
}


void omm_plotnear1d(const char cmp[], double x1, double x2, double y1, double y2, double z1, double z2, int div)
{
	if (_plotnear1ds >= _MAXNEAR1D) {fprintf(stderr, "*** too many plotnear1d.s > %d\n", _MAXNEAR1D); exit(1);}
	strcpy(_plotnear1d[_plotnear1ds].cmp, cmp);
	_plotnear1d[_plotnear1ds].pos[0] = x1;
	_plotnear1d[_plotnear1ds].pos[1] = x2;
	_plotnear1d[_plotnear1ds].pos[2] = y1;
	_plotnear1d[_plotnear1ds].pos[3] = y2;
	_plotnear1d[_plotnear1ds].pos[4] = z1;
	_plotnear1d[_plotnear1ds].pos[5] = z2;
	_plotnear1d[_plotnear1ds].div    = div;
	_plotnear1ds++;
}


void omm_near1ddb(int i0)
{
	_near1dscale.db = (i0 == 0) ? 0 : 1;
}


void omm_near1dscale(double min, double max, int div)
{
	_near1dscale.user = 1;
	_near1dscale.min  = min;
	_near1dscale.max  = max;
	_near1dscale.div  = div;
}


void omm_near1dnoinc(int i0)
{
	_near1dnoinc = i0;
}


void omm_plotnear2d(const char cmp[], char dir, double p0, double p1, double p2, double p3, double p4, int div0, int div1)
{
	if (_plotnear2ds >= _MAXNEAR2D) {fprintf(stderr, "*** too many plotnear2d.s > %d\n", _MAXNEAR2D); exit(1);}
	strcpy(_plotnear2d[_plotnear2ds].cmp, cmp);
	_plotnear2d[_plotnear2ds].dir    = dir;
	_plotnear2d[_plotnear2ds].pos[0] = p0;
	_plotnear2d[_plotnear2ds].pos[1] = p1;
	_plotnear2d[_plotnear2ds].pos[2] = p2;
	_plotnear2d[_plotnear2ds].pos[3] = p3;
	_plotnear2d[_plotnear2ds].pos[4] = p4;
	_plotnear2d[_plotnear2ds].div[0] = div0;
	_plotnear2d[_plotnear2ds].div[1] = div1;
	_plotnear2ds++;
}


void omm_near2ddim(int i0, int i1)
{
	_near2ddim[0] = i0;
	_near2ddim[1] = i1;
}


void omm_near2ddb(int i0)
{
	_near2dscale.db = (i0 == 0) ? 0 : 1;
}


void omm_near2dscale(double min, double max)
{
	_near2dscale.user = 1;
	_near2dscale.min  = min;
	_near2dscale.max  = max;
}


void omm_near2dnoinc(int i0)
{
	_near2dnoinc = i0;
}


void omm_near2dcontour(int i0)
{
	_near2dcontour = i0;
}


void omm_near2dobj(int i0)
{
	_near2dobj = i0;
}


void omm_near2dframe(int i0)
{
	_near2dframe = i0;
}

// (3) output

void omm_outdata(const char fn[])
{
	const double eps = 1e-6;

	FILE *fp;
	if ((fp = fopen(fn, "w")) == NULL) {fprintf(stderr, "output file open error : %s\n", fn); exit(1);}

	fprintf(fp, "%s\n", PROGRAM);

	if (strcmp(_title, "")) {
		fprintf(fp, "title = %s\n", _title);
	}

	for (int n = 0; n < _geometrys; n++) {
		int type    = _geometry[n].type;
		int shape   = _geometry[n].shape;
		double *pos = _geometry[n].pos;
		int *div    = _geometry[n].div;
		fprintf(fp, "geometry = %d %d ", type, shape);
		if      (type == 1) {
			// wire
			if ((shape == 1) || (shape == 2) || (shape == 3)) {
				fprintf(fp, "%g %g %g %g %g %g %d\n",
					pos[0], pos[1], pos[2], pos[3], pos[4], pos[5], div[0]);
			}
			else if ((shape == 11) || (shape == 12) || (shape == 13)) {
				fprintf(fp, "%g %g %g %g %d\n",
					pos[0], pos[1], pos[2], pos[3], div[0]);
			}
		}
		else if (type == 2) {
			// plane
			if ((shape == 1) || (shape == 2) || (shape == 3)) {
				fprintf(fp, "%g %g %g %g %g %g %g %g %g %g %g %g %d %d\n",
					pos[0], pos[1], pos[2], pos[3], pos[4], pos[5], pos[6], pos[7], pos[8], pos[9], pos[10], pos[11], div[0], div[1]);
			}
			else if ((shape == 11) || (shape == 12) || (shape == 13)) {
				fprintf(fp, "%g %g %g %g %g %d %d\n",
					pos[0], pos[1], pos[2], pos[3], pos[4], div[0], div[1]);
			}
			else if ((shape == 21) || (shape == 22) || (shape == 23)) {
				fprintf(fp, "%g %g %g %g %g %g %d %d %d\n",
					pos[0], pos[1], pos[2], pos[3], pos[4], pos[5], div[0], div[1], div[2]);
			}
		}

		// option
		if (_geometry[n].ifeed) {
			fprintf(fp, "feed = %g %g\n", _geometry[n].feed[0], _geometry[n].feed[1]);
		}
		if (_geometry[n].iload) {
			fprintf(fp, "load = %d %g\n", _geometry[n].iload, _geometry[n].load);
		}
		if (_geometry[n].iradius) {
			fprintf(fp, "radius = %g\n", _geometry[n].radius);
		}
		if ((_geometry[n].offset[0] != 0) || (_geometry[n].offset[1] != 0) || (_geometry[n].offset[2] != 0)) {
			fprintf(fp, "offset = %g %g %g\n", _geometry[n].offset[0], _geometry[n].offset[1], _geometry[n].offset[2]);
		}
		if (_geometry[n].name != NULL) {
			fprintf(fp, "name = %s\n", _geometry[n].name);
		}
	}

	if (_planewave.on) {
		if (_planewave.pol < 5) {
			fprintf(fp, "planewave = %g %g %d\n", _planewave.theta, _planewave.phi, _planewave.pol);
		}
		else {
			fprintf(fp, "planewave = %g %g %d %g %g\n", _planewave.theta, _planewave.phi, _planewave.pol, _planewave.a, _planewave.r);
		}
	}

	if (_ground) {
		fprintf(fp, "ground = %d\n", 1);
	}

	fprintf(fp, "frequency = %g %g %d\n", _fstart, _fend, _fdiv);

	if (_iradiusall) {
		fprintf(fp, "radiusall = %d %g\n", _iradiusall, _radiusall);
	}
	if (_z0 > eps) {
		fprintf(fp, "z0 = %g\n", _z0);
	}
	fprintf(fp, "geom3dnode = %g %d %d\n", _geom3dnode_r, _geom3dnode_i0, _geom3dnode_i1);

	// post

	if (_matchingloss) {
		fprintf(fp, "matchingloss = 1\n");
	}

	if (_plotcurrent[0] || _plotcurrent[1]) {
		fprintf(fp, "plotcurrent = %d %d\n", _plotcurrent[0], _plotcurrent[1]);
	}

	if (_plotsmith) {
		fprintf(fp, "plotsmith = 1\n");
	}

	if (_plotzin) {
		if      (_zinscale.user == 0) {
			fprintf(fp, "plotzin = 1\n");
		}
		else if (_zinscale.user == 1) {
			fprintf(fp, "plotzin = 2 %g %g %d\n", _zinscale.min, _zinscale.max, _zinscale.div);
		}
	}

	if (_plotyin) {
		if      (_yinscale.user == 0) {
			fprintf(fp, "plotyin = 1\n");
		}
		else if (_yinscale.user == 1) {
			fprintf(fp, "plotyin = 2 %g %g %d\n", _yinscale.min, _yinscale.max, _yinscale.div);
		}
	}

	if (_plotref) {
		if      (_refscale.user == 0) {
			fprintf(fp, "plotref = 1\n");
		}
		else if (_refscale.user == 1) {
			fprintf(fp, "plotref = 2 %g %g %d\n", _refscale.min, _refscale.max, _refscale.div);
		}
	}

	if (_plotfar0d) {
		if      (_far0dscale.user == 0) {
			fprintf(fp, "plotfar0d = %g %g 1\n", _plotfar0d_angle[0], _plotfar0d_angle[1]);
		}
		else if (_far0dscale.user == 1) {
			fprintf(fp, "plotfar0d = %g %g 2 %g %g %d\n", _plotfar0d_angle[0], _plotfar0d_angle[1], _far0dscale.min, _far0dscale.max, _far0dscale.div);
		}
	}

	if (_freqdiv != 10) {  // default = 10
		fprintf(fp, "freqdiv = %d\n", _freqdiv);
	}

	if (_plotfar1ds) {
		for (int n = 0; n < _plotfar1ds; n++) {
			fprintf(fp, "plotfar1d = %c %d", _plotfar1d[n].dir, _plotfar1d[n].div);
			if ((_plotfar1d[n].dir == 'V') || (_plotfar1d[n].dir == 'H')) {
				fprintf(fp, " %g", _plotfar1d[n].angle);
			}
			fprintf(fp, "%s", "\n");
		}
		fprintf(fp, "far1dstyle = %d\n", _far1dstyle);
		fprintf(fp, "far1dcomponent = %d %d %d\n", _far1dcomponent[0], _far1dcomponent[1], _far1dcomponent[2]);
		fprintf(fp, "far1ddb = %d\n", _far1dscale.db);
		fprintf(fp, "far1dnorm = %d\n", _far1dnorm);
		fprintf(fp, "far1dabs = %d\n", _far1dabs);
		if (_far1dscale.user) {
			fprintf(fp, "far1dscale = %g %g %d\n", _far1dscale.min, _far1dscale.max, _far1dscale.div);
		}
	}

	if (_plotfar2d) {
		fprintf(fp, "plotfar2d = %d %d\n", _plotfar2d_div[0], _plotfar2d_div[1]);
		fprintf(fp, "far2dcomponent = %d %d %d %d %d %d %d\n", _far2dcomponent[0], _far2dcomponent[1], _far2dcomponent[2], _far2dcomponent[3], _far2dcomponent[4], _far2dcomponent[5], _far2dcomponent[6]);
		fprintf(fp, "far2ddb = %d\n", _far2dscale.db);
		if (_far2dscale.user) {
			fprintf(fp, "far2dscale = %g %g\n", _far2dscale.min, _far2dscale.max);
		}
		fprintf(fp, "far2dobj = %g\n", _far2dobj);
	}

	if (_plotnear1ds) {
		for (int n = 0; n < _plotnear1ds; n++) {
			fprintf(fp, "plotnear1d = %s %g %g %g %g %g %g %d\n", _plotnear1d[n].cmp, _plotnear1d[n].pos[0], _plotnear1d[n].pos[1], _plotnear1d[n].pos[2], _plotnear1d[n].pos[3], _plotnear1d[n].pos[4], _plotnear1d[n].pos[5], _plotnear1d[n].div);
		}
		fprintf(fp, "near1ddb = %d\n", _near1dscale.db);
		if (_near1dscale.user) {
			fprintf(fp, "near1dscale = %g %g %d\n", _near1dscale.min, _near1dscale.max, _near1dscale.div);
		}
	}

	if (_plotnear2ds) {
		fprintf(fp, "near2ddim = %d %d\n", _near2ddim[0], _near2ddim[1]);
		for (int n = 0; n < _plotnear2ds; n++) {
			fprintf(fp, "plotnear2d = %s %c %g %g %g %g %g %d %d\n", _plotnear2d[n].cmp, _plotnear2d[n].dir, _plotnear2d[n].pos[0], _plotnear2d[n].pos[1], _plotnear2d[n].pos[2], _plotnear2d[n].pos[3], _plotnear2d[n].pos[4], _plotnear2d[n].div[0], _plotnear2d[n].div[1]);
		}
		fprintf(fp, "near2ddb = %d\n", _near2dscale.db);
		if (_near2dscale.user) {
			fprintf(fp, "near2dscale = %g %g\n", _near2dscale.min, _near2dscale.max);
		}
		fprintf(fp, "near2dcontour = %d\n", _near2dcontour);
		fprintf(fp, "near2dobj = %d\n", _near2dobj);
		if (_near2dframe) {
			fprintf(fp, "near2dframe = %d\n", _near2dframe);
		}
	}

	fprintf(fp, "end\n");

	fclose(fp);

	printf("output file -> %s\n", fn);
}
