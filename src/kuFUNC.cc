#include <iostream>
#include "kuFUNC.hh"
using namespace std;

kuFUNC::kuFUNC(int n,double xt[],double yt[])
{
    norm=0.0;
    norm_flag=0;
    spline=new kuSPL(n,xt,yt);
    linear=new kuLIN(n,xt,yt);
}
double kuFUNC::splint(double p)
{
    double spec=spline->splint(p);
    if(norm_flag==1)spec*=norm;
    return spec;
}
double kuFUNC::linint(double p)
{
    double spec=linear->linint(p);
    if(norm_flag==1)spec*=norm;
    return spec;
}

  ///////////////////////////
  // linear interporation  //
  ///////////////////////////

kuLIN::kuLIN(int na,double v[],double w[])
{
    n=na;
    x=new double[n];
    y=new double[n];
    memcpy(x,v,sizeof(double)*n);
    memcpy(y,w,sizeof(double)*n);
    klo=0;khi=1;
}
double kuLIN::linint(double t)
{
    if(t<x[klo])klo=0;
    if(t>x[khi])khi=n-1;
    if(t<x[klo] || t>x[khi])return 0.0;
    while(khi-klo>1){
       int k=(khi+klo)/2;
       if(x[k]>t)khi=k;else klo=k;
    }
    double h=x[khi]-x[klo];
    double a=(x[khi]-t)/h;
    double b=(t-x[klo])/h;
    return a*y[klo]+b*y[khi];
}

  ///////////////////////////
  // spline interporation  //
  ///////////////////////////

kuSPL::kuSPL(int na,double v[],double w[])
{
    n=na;
    x=new double[n];
    y=new double[n];
    y2=new double[n];
    double *u=new double[n];
    memcpy(x,v,sizeof(double)*n);
    memcpy(y,w,sizeof(double)*n);
    y2[0]=u[0]=0.;
    for(int i=1;i<n-1;i++){
       double sig=(x[i]-x[i-1])/(x[i+1]-x[i-1]);
       double p=sig*y2[i-1]+2.0;
       y2[i]=(sig-1.0)/p;
       u[i]=(y[i+1]-y[i])/(x[i+1]-x[i])-(y[i]-y[i-1])/(x[i]-x[i-1]);
       u[i]=(6.0*u[i]/(x[i+1]-x[i-1])-sig*u[i-1])/p;
    }
    y2[n-1]=0.0;
    for(int k=n-2;k>=0;k--)y2[k]=y2[k]*y2[k+1]+u[k];
    delete [] u;
    klo=0;khi=1;
}
double kuSPL::splint(double t)
{
    if(t<x[klo])klo=0;
    if(t>x[khi])khi=n-1;
    if(t<x[klo] || t>x[khi])return 0.0;
    while(khi-klo>1){
       int k=(khi+klo)/2;
       if(x[k]>t)khi=k;else klo=k;
    }
    double h=x[khi]-x[klo];
    double a=(x[khi]-t)/h;
    double b=(t-x[klo])/h;
    return (a*y[klo]+b*y[khi]+((a*a*a-a)*y2[klo]+(b*b*b-b)*y2[khi])*h*h/6.0);
}

  ///////////////////////////////////
  // Simpson & Romberg integration //
  ///////////////////////////////////

#define JMAX_INTEG 20
#define JMAXP_INTEG JMAX_INTEG+1
#define EPS_INTEG 1.0e-12
#define K_INTEG 5

double kuINTEG::trapzd(kuFUNC *func,int method,double a,double b,int n)
{
    int it,j;
    double sum;
    if(n==1){
       if(method==1){
          return (s=0.5*(b-a)*(func->splint(a)+func->splint(b)));
       } else {
          return (s=0.5*(b-a)*(func->linint(a)+func->linint(b)));
       }
    } else {
       for(it=1,j=1;j<n-1;j++)it *= 2;
       double tnm=double(it);
       double del=(b-a)/tnm;
       double x=a+0.5*del;
       if(method==1){
          for(sum=0.0,j=1;j<=it;j++,x+=del)sum+=func->splint(x);
       } else {
          for(sum=0.0,j=1;j<=it;j++,x+=del)sum+=func->linint(x);
       }
       s=0.5*(s+(b-a)*sum/tnm);
       return s;
    }
}
void kuINTEG::polint(double xa[],double ya[],int n,
                          double x,double *y,double *dy)
{
    double dif=fabs(x-xa[1]);
    double *c=new double[n+1];
    double *d=new double[n+1];
    double dift,ho,hp,w,den;
    int ns;
    for(int i=1;i<=n;i++){
        if((dift=fabs(x-xa[i]))<dif){
           ns=i;
           dif=dift;
        }
        c[i]=ya[i];
        d[i]=ya[i];
    }
    *y=ya[ns--];
    for(int m=1;m<n;m++){
       for(int i=1;i<=n-m;i++){
          ho=xa[i]-x;
          hp=xa[i+m]-x;
          w=c[i+1]-d[i];
          den=ho-hp;
          den=w/den;
          d[i]=hp*den;
          c[i]=ho*den;
       }
       *y+=(*dy=(2*ns<(n-m)?c[ns+1]:d[ns--]));
    }
    delete [] d;
    delete [] c;
}
double kuINTEG::romberg(kuFUNC *func,int method,double a,double b)
{ // Romberg integral, very qiuck but not so stable
    double t[JMAXP_INTEG+1],h[JMAXP_INTEG+1];
    double ss,dss;
    h[1]=1.0;
    for(int j=1;j<=JMAX_INTEG;j++){
       t[j]=trapzd(func,method,a,b,j);
       if(j>=K_INTEG){
           polint(&h[j-K_INTEG],&t[j-K_INTEG],K_INTEG,0.0,&ss,&dss);
           if(fabs(dss)<EPS_INTEG*fabs(ss))return ss; 
       }
       t[j+1]=t[j];
       h[j+1]=0.25*h[j];
    }
    return ss;
}
double kuINTEG::simpson(kuFUNC *func,int method,double a,double b)
{  // Simpson integral, slow but stable
    double t,st,ost,os;
    ost = os = -1.0e30;
    for(int j=1;j<=JMAX_INTEG;j++){
       st=trapzd(func,method,a,b,j);
       t=(4.0*st-ost)/3.0;
       if(fabs(t-os) < EPS_INTEG*fabs(os))return t;
       os=t;
       ost=st;
    }
    return t;
}
