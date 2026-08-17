#ifndef _OMM_PROTOTYPE_H_
#define _OMM_PROTOTYPE_H_

#ifdef __cplusplus
extern "C" {
#endif

extern void   calcFar1d(const double [], d_complex_t ***, d_complex_t ***);
extern void   calcFar2d(const double [], d_complex_t ***, d_complex_t ***);
extern void   calcNear1d(d_complex_t ****, d_complex_t ****);
extern void   calcNear2d(d_complex_t *****, d_complex_t *****);
extern void   cdot(int, int, const real_t *, const real_t *, const real_t *, const real_t *, double *, double *);
extern void   cholesky(int, int, int);
extern double cputime(void);
extern void   currentlog(void);
extern void   elementlog(void);
extern void   farComponent(d_complex_t, d_complex_t, double []);
extern double farfactor(int);
extern void   farfield(int, double, double, double, d_complex_t *, d_complex_t *);
extern int    input_data(FILE *);
extern void   logNear1d(d_complex_t ****, d_complex_t ****);
extern void   logNear2d(d_complex_t *****, d_complex_t *****);
extern void   memalloc(int, int, int);
extern void   memfree(int, int, int);
extern void   monitor1(FILE *, const char []);
extern void   monitor2(FILE *, int, int);
extern void   monitor3(FILE *);
extern void   monitor4(FILE *);
extern void   monitor5(FILE *, const double []);
extern void   nearfield(const double [], int, double, int, d_complex_t [], d_complex_t []);
extern void   outputFar0d(void);
extern void   outputFar1d(void);
extern void   outputFar2d(void);
extern void   outputNear1d(void);
extern void   outputNear2d(void);
extern void   planewave(const double [], d_complex_t [], d_complex_t [], double, double, int, double, double, double);
extern void   plot2d3dNear2d(d_complex_t *****, d_complex_t *****);
extern void   plot2dCurrent(void);
extern void   plot2dFar0d0(int, double *[7], int, double, double, int, int, double, double, const char [], const char [], const char [], double, double, double);
extern void   plot2dFar1d0(int, double (*)[7], int [], char, double, int, int, int, int, double, double, int, const char [], const char [], double, double, double, double);
extern void   plot2dFchar(int, const double [], scale_t, int, const double [], const char [], const char [], const char [], int, int, int);
extern void   plot2dFreq(void);
extern void   plot2dNear1d(d_complex_t ****, d_complex_t ****);
extern void   plot2dNear1d0(const char [], int, double *[3], double *[3], const double [], int, int, double, double, int, const char [], double, double [2][3], double, double, double);
extern void   plot2dNear2d0(int, int, double **, const double [], const double [], int, int, int, double, double, int, const char [], double, char, double, const char [], int, double (*)[2][3], double, double, double);
extern void   plot2dRef(int, int, const d_complex_t [], const double [], scale_t, int, const double [], const char [], double, double, double);
extern void   plot2dSmith(int, int, const d_complex_t [], const double [], const double [], const char [], double, double, double);
extern void   plot2dYin(int, int, const d_complex_t [], scale_t, int, const double [], const char [], double, double, double);
extern void   plot2dZin(int, int, const d_complex_t [], scale_t, int, const double [], const char [], double, double, double);
extern void   plot3dCurrent(void);
extern void   plot3dFar2d(d_complex_t ***, d_complex_t ***);
extern void   plot3dFar2d0(int, int, double **, int, int, double, double, int, double (*)[2][3], double, int, char **);
extern void   plot3dGeom(void);
extern void   plot3dNear2d0(int, int, double **, const double [], const double [], int, int, int, double, double, int, const char [], double, char, double, const char [], int, double (*)[2][3]);
extern void   posNear2d(near2d_t, int, int, double []);
extern int    post_data(FILE *);
extern void   post(void);
extern void   readout(FILE *);
extern void   rhsvector(int);
extern void   solve(int, int, int, int, int, double []);
extern int    tokenize(char *, const char [], char *[], size_t);
extern void   wiregrid(void);
extern void   writeout(FILE *);
extern void   zdot(int, const double *, const double *, const double *, const double *, double *, double *);
extern void   zfeed(void);
extern void   zmatrix(int);

#ifdef __cplusplus
}
#endif

#endif
