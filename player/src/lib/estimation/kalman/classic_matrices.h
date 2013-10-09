#include "Matrix.h"
#include <cmath>

namespace kalman {

  namespace matrices {

    /*****
    * f1 *
    *****/
    double f1(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);

    double df1_dxx(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);
    double df1_dxy(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);
    double df1_dxvx(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);
    double df1_dxvy(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);

    double df1_dwx(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);
    double df1_dwy(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);
    double df1_dwo(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);
    double df1_dwt(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);
    double df1_dwj(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);
    double df1_dwk(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);

    /*****
    * f2 *
    *****/
    double f2(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);

    double df2_dxx(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);
    double df2_dxy(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);
    double df2_dxvx(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);
    double df2_dxvy(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);

    double df2_dwx(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);
    double df2_dwy(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);
    double df2_dwo(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);
    double df2_dwt(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);
    double df2_dwj(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);
    double df2_dwk(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);

    /*****
    * f3 *
    *****/
    double f3(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);

    double df3_dxx(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);
    double df3_dxy(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);
    double df3_dxvx(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);
    double df3_dxvy(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);
    
    double df3_dwx(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);
    double df3_dwy(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);
    double df3_dwo(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);
    double df3_dwt(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);
    double df3_dwj(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);
    double df3_dwk(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);

    /*****
    * f4 *
    *****/
    double f4(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);

    double df4_dxx(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);
    double df4_dxy(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);
    double df4_dxvx(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);
    double df4_dxvy(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);
    
    double df4_dwx(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);
    double df4_dwy(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);
    double df4_dwo(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);
    double df4_dwt(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);
    double df4_dwj(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);
    double df4_dwk(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);

    /****
    * f *
    ****/
    MatrixCM f(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);

    /****
    * A *
    ****/
    MatrixCM A(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);

    /****
    * W *
    ****/
    MatrixCM W(const MatrixCM& x, const MatrixCM& u, const MatrixCM& w);

    /****
    * Q *
    ****/
    MatrixCM Q(const MatrixCM& w);

    /****
    * h *
    ****/
    MatrixCM h(const MatrixCM& x, const MatrixCM& v);

    /****
    * H *
    ****/    
    MatrixCM H(const MatrixCM& x, const MatrixCM& v);

    /****
    * V *
    ****/
    MatrixCM V(const MatrixCM& x, const MatrixCM& v);

    /****
    * R *
    ****/
    MatrixCM R(const MatrixCM& v);

  }

}
