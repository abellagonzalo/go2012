/*! \file tracking.hpp
 \brief The Object and Feature Tracking
 */

/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009, Willow Garage Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#ifndef __OPENCV_TRACKING_HPP__
#define __OPENCV_TRACKING_HPP__

#include <opencv/cv.h>

using namespace cv;

/*
standard Kalman filter (in G. Welch' and G. Bishop's notation):

  x(k)=A*x(k-1)+B*u(k)+w(k)  p(w)~N(0,Q)
  z(k)=H*x(k)+v(k),   p(v)~N(0,R)
*/

typedef struct bicaKalman
{
    int MP;                     /* number of measurement vector dimensions */
    int DP;                     /* number of state vector dimensions */
    int CP;                     /* number of control vector dimensions */

    /* backward compatibility fields */
#if 1
    float* PosterState;         /* =state_pre->data.fl */
    float* PriorState;          /* =state_post->data.fl */
    float* DynamMatr;           /* =transition_matrix->data.fl */
    float* MeasurementMatr;     /* =measurement_matrix->data.fl */
    float* MNCovariance;        /* =measurement_noise_cov->data.fl */
    float* PNCovariance;        /* =process_noise_cov->data.fl */
    float* KalmGainMatr;        /* =gain->data.fl */
    float* PriorErrorCovariance;/* =error_cov_pre->data.fl */
    float* PosterErrorCovariance;/* =error_cov_post->data.fl */
    float* Temp1;               /* temp1->data.fl */
    float* Temp2;               /* temp2->data.fl */
#endif

    CvMat* state_pre;           /* predicted state (x'(k)):
                                    x(k)=A*x(k-1)+B*u(k) */
    CvMat* state_post;          /* corrected state (x(k)):
                                    x(k)=x'(k)+K(k)*(z(k)-H*x'(k)) */
    CvMat* transition_matrix;   /* state transition matrix (A) */
    CvMat* control_matrix;      /* control matrix (B)
                                   (it is not used if there is no control)*/
    CvMat* measurement_matrix;  /* measurement matrix (H) */
    CvMat* process_noise_cov;   /* process noise covariance matrix (Q) */
    CvMat* measurement_noise_cov; /* measurement noise covariance matrix (R) */
    CvMat* error_cov_pre;       /* priori error estimate covariance matrix (P'(k)):
                                    P'(k)=A*P(k-1)*At + Q)*/
    CvMat* gain;                /* Kalman gain matrix (K(k)):
                                    K(k)=P'(k)*Ht*inv(H*P'(k)*Ht+R)*/
    CvMat* error_cov_post;      /* posteriori error estimate covariance matrix (P(k)):
                                    P(k)=(I-K(k)*H)*P'(k) */
    CvMat* temp1;               /* temporary matrices */
    CvMat* temp2;
    CvMat* temp3;
    CvMat* temp4;
    CvMat* temp5;
} bicaKalman;

/* Creates Kalman filter and sets A, B, Q, R and state to some initial values */
bicaKalman* bicaCreateKalman( int dynam_params, int measure_params,
                                 int control_params = 0);

/* Releases Kalman filter state */
void  bicaReleaseKalman( bicaKalman** kalman);

/* Updates Kalman filter by time (predicts future state of the system) */
const CvMat*  bicaKalmanPredict( bicaKalman* kalman,
                                      const CvMat* control = NULL);

/* Updates Kalman filter by measurement
   (corrects state of the system and internal matrices) */
const CvMat*  bicaKalmanCorrect( bicaKalman* kalman, const CvMat* measurement );

#define bicaKalmanUpdateByTime  bicaKalmanPredict
#define bicaKalmanUpdateByMeasurement bicaKalmanCorrect
    

/*!
 Kalman filter.
 
 The class implements standard Kalman filter \url{http://en.wikipedia.org/wiki/Kalman_filter}.
 However, you can modify KalmanFilter::transitionMatrix, KalmanFilter::controlMatrix and
 KalmanFilter::measurementMatrix to get the extended Kalman filter functionality.
*/
class bicaKalmanFilter
{
public:
    //! the default constructor
    bicaKalmanFilter();
    //! the full constructor taking the dimensionality of the state, of the measurement and of the control vector
    bicaKalmanFilter(int dynamParams, int measureParams, int controlParams=0, int type=CV_32F);
    //! re-initializes Kalman filter. The previous content is destroyed.
    void init(int dynamParams, int measureParams, int controlParams=0, int type=CV_32F);

    //! computes predicted state
    const Mat& predict(const Mat& control=Mat());
    //! updates the predicted state from the measurement
    const Mat& correct(const Mat& measurement);

    Mat statePre;           //!< predicted state (x'(k)): x(k)=A*x(k-1)+B*u(k)
    Mat statePost;          //!< corrected state (x(k)): x(k)=x'(k)+K(k)*(z(k)-H*x'(k))
    Mat transitionMatrix;   //!< state transition matrix (A)
    Mat controlMatrix;      //!< control matrix (B) (not used if there is no control)
    Mat measurementMatrix;  //!< measurement matrix (H)
    Mat processNoiseCov;    //!< process noise covariance matrix (Q)
    Mat measurementNoiseCov;//!< measurement noise covariance matrix (R)
    Mat errorCovPre;        //!< priori error estimate covariance matrix (P'(k)): P'(k)=A*P(k-1)*At + Q)*/
    Mat gain;               //!< Kalman gain matrix (K(k)): K(k)=P'(k)*Ht*inv(H*P'(k)*Ht+R)
    Mat errorCovPost;       //!< posteriori error estimate covariance matrix (P(k)): P(k)=(I-K(k)*H)*P'(k)

    // temporary matrices
    Mat temp1;
    Mat temp2;
    Mat temp3;
    Mat temp4;
    Mat temp5;
};

#endif
