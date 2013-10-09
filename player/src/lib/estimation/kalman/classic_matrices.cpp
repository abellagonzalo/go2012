#include "classic_matrices.h"

namespace kalman {

  namespace matrices {

    /*****
    * f1 *
    *****/

    double f1(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return x.e(0) * cos( u.e(2) + w.e(2) )
           + x.e(2) * ( u.e(3) + w.e(3) ) * cos( u.e(2) + w.e(2) )
           + x.e(1) * sin( u.e(2) + w.e(2) )
           - ( u.e(0)+w.e(0) )
           + ( u.e(4) + w.e(4) );
    }

    double df1_dxx(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return cos( u.e(2) + w.e(2) );
    }

    double df1_dxy(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return sin( u.e(2) + w.e(2) );
    }

    double df1_dxvx(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return ( u.e(3) + w.e(3) ) * cos( u.e(2) + w.e(2) );
    }

    double df1_dxvy(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return 0.0;
    }

    double df1_dwx(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return -1;
    }

    double df1_dwy(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return 0.0;
    }

    double df1_dwo(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return -x.e(0) * sin( u.e(2) + w.e(2) ) 
            - x.e(2) * ( u.e(3) + w.e(3) ) * sin( u.e(2) + w.e(2) ) 
            + x.e(1) * cos( u.e(2) + w.e(2) );
    }

    double df1_dwt(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return x.e(2) * cos( u.e(2) + w.e(2) );
    }

    double df1_dwj(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return 1.0;
    }

    double df1_dwk(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return 0.0;
    }

    /*****
    * f2 *
    *****/

    double f2(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return x.e(1) * cos( u.e(2) + w.e(2) )
           + x.e(3) * ( u.e(3) + w.e(3) ) * cos( u.e(2) + w.e(2) )
           - x.e(0) * sin( u.e(2) + w.e(2) )
           - ( u.e(1)+w.e(1) )
           + ( u.e(4) + w.e(4) );
    }

    double df2_dxx(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return -sin( u.e(2) + w.e(2) );
    }

    double df2_dxy(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return cos( u.e(2) + w.e(2) );
    }

    double df2_dxvx(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return 0.0;
    }

    double df2_dxvy(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return ( u.e(3) + w.e(3) ) * cos( u.e(2) + w.e(2) );
    }

    double df2_dwx(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return 0.0;
    }

    double df2_dwy(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return -1.;
    }

    double df2_dwo(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return -x.e(1) * sin( u.e(2) + w.e(2) ) 
            - x.e(3) * ( u.e(3) + w.e(3) ) * sin( u.e(2) + w.e(2) ) 
            - x.e(0) * cos( u.e(2) + w.e(2) );
    }

    double df2_dwt(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return x.e(3) * cos( u.e(2) + w.e(2) );
    }

    double df2_dwj(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return 1.0;
    }

    double df2_dwk(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return 0.0;
    }

    /*****
    * f3 *
    *****/

    double f3(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return x.e(2) * cos( u.e(2) + w.e(2) ) * ( u.e(5) + w.e(5) )
           + x.e(3) * sin( u.e(2) + w.e(2) ) * ( u.e(5) + w.e(5) );
    }

    double df3_dxx(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return 0.0;
    }

    double df3_dxy(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return 0.0;
    }

    double df3_dxvx(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return cos( u.e(2) + w.e(2) ) * ( u.e(5) + w.e(5) );
    }

    double df3_dxvy(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return sin( u.e(2) + w.e(2) ) * ( u.e(5) + w.e(5) );
    }

    double df3_dwx(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return 0.0;
    }

    double df3_dwy(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return 0.0;
    }

    double df3_dwo(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return -x.e(2) * sin( u.e(2) + w.e(2) ) * ( u.e(5) + w.e(5) ) 
            + x.e(3) * cos( u.e(2) + w.e(2) ) * ( u.e(5) + w.e(5) );
    }

    double df3_dwt(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return 0.0;
    }

    double df3_dwj(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return 0.0;
    }

    double df3_dwk(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return x.e(2) * cos( u.e(2) + w.e(2) ) 
           + x.e(3) * sin( u.e(2) + w.e(2) );
    }

    /*****
    * f4 *
    *****/

    double f4(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return x.e(3) * cos( u.e(2) + w.e(2) ) * ( u.e(5) + w.e(5) )
           - x.e(2) * sin( u.e(2) + w.e(2) ) * ( u.e(5) + w.e(5) );
    }

    double df4_dxx(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return 0.0;
    }

    double df4_dxy(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return 0.0;
    }

    double df4_dxvx(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return -sin( u.e(2) + w.e(2) ) * ( u.e(5) + w.e(5) );
    }

    double df4_dxvy(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return cos( u.e(2) + w.e(2) ) * ( u.e(5) + w.e(5) );
    }

    double df4_dwx(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return 0.0;
    }

    double df4_dwy(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return 0.0;
    }

    double df4_dwo(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return -x.e(3) * sin( u.e(2) + w.e(2) ) * ( u.e(5) + w.e(5) ) 
            - x.e(2) * cos( u.e(2) + w.e(2) ) * ( u.e(5) + w.e(5) );
    }

    double df4_dwt(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return 0.0;
    }

    double df4_dwj(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return 0.0;
    }

    double df4_dwk(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      return x.e(3) * cos( u.e(2) + w.e(2) ) 
           - x.e(2) * sin( u.e(2) + w.e(2) );
    }

    MatrixCM f(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      double values [4] = { f1(x, u, w), f2(x, u, w), f3(x, u, w), f4(x, u, w) };
      return MatrixCM(4,1,values);
    }

    MatrixCM A(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      double values [16] = { df1_dxx(x, u, w), df1_dxy(x, u, w), df1_dxvx(x, u, w), df1_dxvy(x, u, w),
                            df2_dxx(x, u, w), df2_dxy(x, u, w), df2_dxvx(x, u, w), df2_dxvy(x, u, w),
                            df3_dxx(x, u, w), df3_dxy(x, u, w), df3_dxvx(x, u, w), df3_dxvy(x, u, w),
                            df4_dxx(x, u, w), df4_dxy(x, u, w), df4_dxvx(x, u, w), df4_dxvy(x, u, w) };
      return MatrixCM(4,4,values);
    }

    MatrixCM W(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w) {
      double values [24] = { df1_dwx(x, u, w), df1_dwy(x, u, w), df1_dwo(x, u, w), df1_dwt(x, u, w), df1_dwj(x, u, w), df1_dwk(x, u, w),
                            df2_dwx(x, u, w), df2_dwy(x, u, w), df2_dwo(x, u, w), df2_dwt(x, u, w), df2_dwj(x, u, w), df2_dwk(x, u, w),
                            df3_dwx(x, u, w), df3_dwy(x, u, w), df3_dwo(x, u, w), df3_dwt(x, u, w), df3_dwj(x, u, w), df3_dwk(x, u, w),
                            df4_dwx(x, u, w), df4_dwy(x, u, w), df4_dwo(x, u, w), df4_dwt(x, u, w), df4_dwj(x, u, w), df4_dwk(x, u, w) }; 
      return MatrixCM(4,6,values);
    }

    MatrixCM Q(const MatrixCM& w) {
      double values [36] = { w.e(0), 0, 0, 0, 0, 0,
                            0, w.e(1), 0, 0, 0, 0,
                            0, 0, w.e(2), 0, 0, 0,
                            0, 0, 0, w.e(3), 0, 0,
                            0, 0, 0, 0, w.e(4), 0,
                            0, 0, 0, 0, 0, w.e(5) };
      return MatrixCM(6,6,values);
    }

    MatrixCM h(const MatrixCM& x, const MatrixCM& v) {
      double values [2] = { x.e(0) + v.e(0), 
                           x.e(1) + v.e(1) };
      return MatrixCM(2,1,values);
    }

    MatrixCM H(const MatrixCM& x, const MatrixCM& v) {
      double values [8] = { 1., 0., 0., 0.,  
                           0., 1., 0., 0. };
      return MatrixCM(2,4,values);
    }

    MatrixCM V(const MatrixCM& x, const MatrixCM& v) {
      return MatrixCM(2);
    }

    MatrixCM R(const MatrixCM& v) {
      double values [4] = { v.e(0), 0, 
                           0, v.e(1) };
      return MatrixCM(2,2,values);
    }

  }

}
