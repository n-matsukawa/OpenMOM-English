#ifndef _OMM_DATALIB_H_
#define _OMM_DATALIB_H_
extern void omm_init(void);
extern void omm_title(const char []);
extern void omm_geometry_wire(int, double, double, double, double, double, double, int);
extern void omm_geometry_xline(double, double, double, double, int);
extern void omm_geometry_yline(double, double, double, double, int);
extern void omm_geometry_zline(double, double, double, double, int);
extern void omm_geometry_triangle(double, double, double, double, double, double, double, double, double);
extern void omm_geometry_plane(int, double, double, double, double, double, double, double, double, double, double, double, double, int, int);
extern void omm_geometry_xrect(double, double, double, double, double, int, int);
extern void omm_geometry_yrect(double, double, double, double, double, int, int);
extern void omm_geometry_zrect(double, double, double, double, double, int, int);
extern void omm_geometry_box(int, double, double, double, double, double, double, int, int, int);
extern void omm_geometry_disc(double, double, double, double, int);
extern void omm_geometry_bell(double, double, double, double, double, int, int);
extern void omm_feed(double, double);
extern void omm_load(int, double);
extern void omm_radius(double);
extern void omm_offset(double, double, double);
extern void omm_name(const char []);
extern void omm_planewave(double, double, int);
extern void omm_planewave_ellipse(double, double, double, double);
extern void omm_ground(void);
extern void omm_z0(double);
extern void omm_radiusall(int, double);
extern void omm_geom3dnode(double, int, int);
extern void omm_frequency(double, double, int);
extern void omm_matchingloss(void);
extern void omm_plotcurrent(int, int);
extern void omm_plotsmith(void);
extern void omm_plotzin(int, double, double, int);
extern void omm_plotyin(int, double, double, int);
extern void omm_plotref(int, double, double, int);
extern void omm_plotfar0d(double, double, int, double, double, int);
extern void omm_freqdiv(int);
extern void omm_plotfar1d(char, int, double);
extern void omm_far1dstyle(int);
extern void omm_far1dcomponent(int, int, int);
extern void omm_far1ddb(int);
extern void omm_far1dnorm(int);
extern void omm_far1dabs(int);
extern void omm_far1dscale(double, double, int);
extern void omm_plotfar2d(int, int);
extern void omm_far2dcomponent(int, int, int, int, int, int, int);
extern void omm_far2ddb(int);
extern void omm_far2dscale(double, double);
extern void omm_far2dobj(double);
extern void omm_plotnear1d(const char [], double, double, double, double, double, double, int);
extern void omm_near1ddb(int);
extern void omm_near1dscale(double, double, int);
extern void omm_near1dnoinc(int);
extern void omm_plotnear2d(const char [], char, double, double, double, double, double, int, int);
extern void omm_near2ddim(int, int);
extern void omm_near2ddb(int);
extern void omm_near2dscale(double, double);
extern void omm_near2dnoinc(int);
extern void omm_near2dcontour(int);
extern void omm_near2dobj(int);
extern void omm_near2dframe(int);
extern void omm_outdata(const char []);

// nearest integer
#define NINT(l, d) (int)((fabs(l) / fabs(d)) + 0.5)
#endif  // _OMM_DATALIB_H_
