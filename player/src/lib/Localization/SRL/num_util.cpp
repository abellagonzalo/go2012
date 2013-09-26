/*========================================================================
    This was taken from util.cc originally, and renamed into:
    num_util.cpp

    Somchaya Liemhetcharat, 2009
  ========================================================================*/

#include <math.h>

#include "num_util.h"

float gaussian_pdf(float mu, float sigma_sq, float x) {
  return exp((x - mu)*(x - mu)/(-2.0*sigma_sq)) / sqrt(M_2PI*sigma_sq);
}

float gaussian_log_pdf(float mu, float sigma_sq, float x) {
  return (x - mu)*(x - mu)/(-2.0*sigma_sq) - log(sqrt(M_2PI*sigma_sq));
}

