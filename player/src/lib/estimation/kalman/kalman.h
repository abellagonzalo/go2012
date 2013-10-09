#include <algorithm>
#include <boost/bind.hpp>
#include "classic_matrices.h"

namespace kalman {

  namespace algorithm {

    using namespace kalman::matrices;

    typedef std::pair<MatrixCM, MatrixCM> MatrixCMPair;

    double zero4_init [4] = { 0., 0., 0., 0.};
    double zero6_init [6] = { 0., 0., 0., 0., 0., 0.};
    MatrixCM zero4(4, 1, zero4_init);
    MatrixCM zero6(6, 1, zero6_init);
    MatrixCM iden4(4);

    MatrixCMPair pair_generator() {
        double x [4] = { 0, 0, 0, 0 };
        double P [16] = {1000, 0, 0, 0,
                         0, 1000, 0, 0,
                         0, 0, 1000, 0,
                         0, 0, 0, 1000 };
        return std::make_pair( MatrixCM(4, 1, x), MatrixCM(4, 4, P) );
    }

    MatrixCMPair predict(const MatrixCMPair& pair, const MatrixCM& u, const MatrixCM& w) {
      using namespace matrices;
      const MatrixCM& x = pair.first;
      const MatrixCM& P = pair.second;
      return std::make_pair( f(x, u, zero6),
        A(x, u, w) * P * A(x, u, w).transpose() + W(x, u, w) * Q(w) * W(x, u, w).transpose());      
    }

    template <typename InputIterator>
    void predict_collection(InputIterator first, InputIterator last, const MatrixCM& u, const MatrixCM& w) {
      std::transform(first, last, first, boost::bind(predict, _1, u, w));
    }

    template <typename T>
    MatrixCM diff(const T& t1, const T& t2) {
      return t1 - t2;
    }

    double mahalanobis(const MatrixCM& x1, const MatrixCM& x2, MatrixCM cov) {
      return sqrt( (diff(x1, x2).transpose() * cov.inverse() * diff(x1, x2)).determinant() );
    }

    MatrixCM create_mahalanobis_mat(const MatrixCM& m) {
      double data [2] = { m.e(0,0), m.e(1,0) };
      return MatrixCM(2, 1, data);
    }

    MatrixCM create_mahalanobis_cov(const MatrixCM& m) {
      double data [4] = { m.e(0,0), m.e(0,1), m.e(1,0), m.e(1,1) };
      return MatrixCM(2, 2, data);
    }

    bool min_mahalanobis_pred(const algorithm::MatrixCMPair& p1, const algorithm::MatrixCMPair& p2, MatrixCM z) {
      return mahalanobis(create_mahalanobis_mat(p1.first), create_mahalanobis_mat(z), create_mahalanobis_cov(p1.second))
           < mahalanobis(create_mahalanobis_mat(p2.first), create_mahalanobis_mat(z), create_mahalanobis_cov(p2.second));
    }

    MatrixCMPair update(const MatrixCMPair& pair, const MatrixCM& z, const MatrixCM& v) {
      using namespace matrices;
      const MatrixCM& x = pair.first;
      const MatrixCM& P = pair.second;
      MatrixCM aux = H(x, v) * P * H(x, v).transpose() + V(x, v) * R(v) * V(x, v).transpose();
      MatrixCM gain = P * H(x, v).transpose() * aux.inverse();
      return std::make_pair( x + gain * ( z - h(x, zero4) ),
                             ( iden4 - gain * H(x,v) ) * P);
    }

    template <typename InputIterator>
    void update_collection(InputIterator first, InputIterator last, const MatrixCM& z, const MatrixCM& v) {
      if (first == last) return;
      InputIterator min_element = std::min_element(first, last, boost::bind(min_mahalanobis_pred, _1, _2, z));
      *min_element = update(*min_element, z, v);
    }

  } // algorithm

} // kalman
