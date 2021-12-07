#ifndef __kuFUNC_HH
#define __kuFUNC_HH

#include "math.h"
#include "string.h"
//-----------------------------------------------------------------------------
  //////////////////////////////////////////////
  // Spline interpolation for the given table //
  //////////////////////////////////////////////
class kuSPL{  // spline interpolation from the numerical recipes
    double *x,*y,*y2;
    int n,klo,khi;
public:
    kuSPL(int na,double v[],double w[]); // given v[na],w[na]
    double splint(double t);           // given t; spline interpolated value
    ~kuSPL(){delete [] x;delete [] y;delete [] y2;};
};
//-----------------------------------------------------------------------------
  //////////////////////////////////////////////
  // Linear interpolation for the given table //
  //////////////////////////////////////////////
class kuLIN{
    double *x,*y;
    int n,klo,khi;
public:
    kuLIN(int na,double v[],double w[]); // given v[na],w[na]
    double linint(double t);           // given t; linear interpolated value
    ~kuLIN(){delete [] x;delete [] y;};
};
//-----------------------------------------------------------------------------
  ///////////////////////////////////////////////////
  // Function based on spline/linear interpolation //
  ///////////////////////////////////////////////////
class kuFUNC{
    kuSPL *spline;
    kuLIN *linear;
    double norm;
    int norm_flag;
public:
    kuFUNC(int n,double xt[],double yt[]);
    void set_norm(double norm_x){norm=norm_x;norm_flag=1;};
    double get_norm(){return norm;};
    void reset_norm(){norm_flag=0;};
    double splint(double p);
    double linint(double p);
    ~kuFUNC(){delete spline;delete linear;};
};

//-----------------------------------------------------------------------------
  ///////////////////////////////////////////////////////////
  // Integration on kuFUNC with two different methods      //
  //  faster unstable Romberg                              //
  //  and slower stable Simpson                            //
  ///////////////////////////////////////////////////////////
class kuINTEG{
    double s;
    double trapzd(kuFUNC *func,int method,double a,double b,int n);
    void polint(double xa[],double ya[],int n,double x,double *y,double *dy);
public:
  // integrate kuFUNC on [a,b]  0 linear,  1 spline
    double romberg(kuFUNC *func,int method,double a,double b);
    double simpson(kuFUNC *func,int method,double a,double b);
};

#endif
