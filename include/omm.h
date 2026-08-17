// omm.h
#ifndef _OMM_H_
#define _OMM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <assert.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#define PROGRAM "OpenMOM"
#define VERSION_MAJOR (4)
#define VERSION_MINOR (2)
#define VERSION_BUILD (0)

#define FN_out      "omm.out"
#define FN_log      "omm.log"
#define FN_element  "element.log"
#define FN_current  "current.log"
#define FN_zmatrix  "zmatrix.log"
#define FN_geom3d_0 "geom3d.htm"
#define FN_ev2d_0   "ev2d.htm"
#define FN_ev3d_0   "ev3d.htm"
#define FN_geom3d_1 "geom.ev3"
#define FN_ev2d_1   "ev.ev2"
#define FN_ev3d_1   "ev.ev3"
#define FN_far0d    "far0d.log"
#define FN_far1d    "far1d.log"
#define FN_far2d    "far2d.log"
#define FN_near1d   "near1d.log"
#define FN_near2d   "near2d.log"

#define PI     (4.0*atan(1.0))
#define DTOR   (PI/180)
#define C      (2.99792458e8)
#define ETA0   (C*4*PI*1e-7)
#define EPS0   (1/(C*ETA0))
#define EPS    (1e-6)
#define EPS2   (EPS*EPS)

#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define NINT(l,d) ((int)((l) / (d) + 0.5))

#if defined(_DOUBLE)
#define real_t double
#else
#define real_t float
#endif

typedef struct {double r, i;} d_complex_t;
typedef struct {float  r, i;} f_complex_t;

#ifdef MAIN
#define EXTERN
#else
#define EXTERN extern
#endif

enum {UNDEF = 0, WIRE, PLANE};
enum {XYZ = 1, CYLINDER, POLAR};
enum {WIRE1 = 1, WIRE2, WIRE3};
enum {XLINE = 11, YLINE, ZLINE};
enum {PLANE1 = 1, PLANE2, PLANE3};
enum {XRECT = 11, YRECT, ZRECT};
enum {BOX1 = 21, BOX2, BOX3};

typedef struct {
	int    shape;               // 1/2
	int    cosys;               // 1/2/3
	double pos[4][3];           // position [vertex1/2/3/4][X/Y/Z]
	int    div[2];              // division [1-2/1-4]
	int    ifeed;               // feed 0/1
	double feed[2];             // amplitude[V], phase[deg]
	int    iload;               // load 0/1=R/2=L/3=C
	double load;                // R/L/C
	int    iradius;             // 0/1
	double radius;              // radius
	double offset[3];           // offset [X/Y/Z]
	char   *name;               // (GUI) name
} geometry_t;                   // geometry

typedef struct {
	double pos[3][3];           // position [center/minus/plus][X/Y/Z]
	double lng[3];              // length [center/minus/plus]
	double tan[3];              // unit vecor [X/Y/Z]
	int    ifeed;               // feed (0/1)
	double feed[2];             // amplitude[V], phase[deg]
	int    iload;               // load 0/1=R/2=L/3=C
	double load;                // R/L/C
	double radius;              // radius
	int    iopen;               // open end (0/1/2/3)
	int    border;              // (plot) border element (0/1)
	int    wire;                // (plot) wire unit (0/1)
} element_t;                    // element

typedef struct {
	double theta, phi;          // incidence angle [deg]
	int    pol;                 // polarization (1/2/3/4/5=V/H/R/H/E)
	double a, r;                // elliptical: angle [deg], axial ratio
} planewave_t;                  // plane wave incidence

typedef struct {
	char   dir;                 // direction : 'X'/'Y'/'Z'/'V'/'H'
	int    div;                 // division of angle 360 deg
	double angle;               // V/H constant angle [deg]
} far1d_t;                      // far field 1d

typedef struct {
	int    divtheta, divphi;    // division in theta and phi
} far2d_t;                      // far field 2d

typedef struct {
	char   cmp[3];              // component : E/Ex/Ey/Ez/H/Hx/Hy/Hz
	double pos[2][3];           // start and end position
	int    div;                 // division
} near1d_t;                     // near field 1d

typedef struct {
	char   cmp[3];              // component : E/Ex/Ey/Ez/H/Hx/Hy/Hz
	char   dir;                 // direction : X/Y/Z
	double pos0;                // constant [m]
	double pos1[2];             // position-1 [m]
	double pos2[2];             // position-2 [m]
	int    div1;                // division-1
	int    div2;                // division-2
} near2d_t;                     // near field 2d

typedef struct {
	int    db;                  // db ? (0/1)
	int    user;                // 0/1 : auto/user
	double min, max;            // min, max
	int    div;                 // division
} scale_t;                      // scale

// === solver ===

EXTERN char        Title[256];         // title
EXTERN int         NGeometry;          // number of geometries
EXTERN geometry_t  *Geometry;          // geometries
EXTERN int         NElement;           // number of elements
EXTERN element_t   *Element;           // elements
EXTERN int         NFrequency;         // number of frequencies
EXTERN double      *Frequency;         // frequencies
EXTERN double      Z0;                 // feed impedance [ohm]
EXTERN int         IGround;            // ground : 0/1/2/3=None/X/Y/Z

EXTERN real_t      **Am_r, **Am_i;     // impedance matrix (aligned)
EXTERN real_t      **Wv_r, **Wv_i;     // work array (aligned)

EXTERN int         LElement;           // array length
EXTERN d_complex_t *Wv, *Dv;           // work array
EXTERN d_complex_t *Bv;                // RHS vector
EXTERN d_complex_t *Cv;                // current distribution

EXTERN int         NFeed;              // number of feeds
EXTERN d_complex_t *Zin;               // input impedance [ohm]

EXTERN int         IPlanewave;         // plane wave incidence (0/1)
EXTERN planewave_t Planewave;          // plane wave

EXTERN double      cGeom3d;            // size of node
EXTERN int         pGeom3d[2];         // plot node of wire/plane unit

EXTERN int         Plot3dGeom;         // plot geometry (3D)

// === post ===

EXTERN int         HTML;               // 0:ev, 1:HTML

EXTERN int         NGline;             // number of geometry lines
EXTERN double      (*Gline)[2][3];     // geometry lines

EXTERN int         MatchingLoss;       // include matching loss (0/1)

EXTERN int         Pcurrent[2];        // current distribution (0/1)

EXTERN int         IFreq[4];           // frequency char.s (0/1)
EXTERN scale_t     FreqScale[4];       // scale
EXTERN int         Freqdiv;            // x-axis division

EXTERN int         IFar0d;             // frequency char of far0d (0/1)
EXTERN double      Far0d[2];           // theta, phi [deg]
EXTERN scale_t     Far0dScale;         // scale

EXTERN int         NFar1d;             // number of far1d
EXTERN far1d_t     *Far1d;             // parameters
EXTERN int         Far1dStyle;         // style (0/1)
EXTERN int         Far1dComp[3];       // component (0/1)
EXTERN int         Far1dNorm;          // normalize (0/1)
EXTERN int         Far1dAbs;           // plot abs (0/1)
EXTERN scale_t     Far1dScale;         // scale

EXTERN int         NFar2d;             // number of far2d
EXTERN far2d_t     Far2d;              // parameters
EXTERN int         Far2dComp[7];       // component (0/1)
EXTERN scale_t     Far2dScale;         // scale
EXTERN double      Far2dObj;           // size of objects

EXTERN int         NNear1d;            // number of near1d
EXTERN near1d_t    *Near1d;            // parameters
EXTERN scale_t     Near1dScale;        // scale
EXTERN int         Near1dNoinc;        // exclude plane wave incidence (0/1)

EXTERN int         NNear2d;            // number of near2d
EXTERN near2d_t    *Near2d;            // parameters
EXTERN int         Near2dDim[2];       // 0/1 : 2D, 3D
EXTERN scale_t     Near2dScale;        // scale
EXTERN int         Near2dContour;      // contour (0/1/2/3)
EXTERN int         Near2dObj;          // object (0/1)
EXTERN int         Near2dNoinc;        // exclude plane wave incidence (0/1)
EXTERN int         Near2dFrame;        // frames of animation

EXTERN double      Width2d, Height2d;  // window size (2D)
EXTERN double      Fontsize2d;         // font size (2D)

#ifdef __cplusplus
}
#endif

#endif  // _OMM_H_
