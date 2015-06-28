#ifndef DATA_DATA_SET_H
#define DATA_DATA_SET_H

#include <iostream>
#include <vector>
#include "basedef.h"
#include "data/data_point.h"

class data_set {
  /**
   * Collection of all data points.
   *
   * @param xpMat    pointer to bigmat if using bigmatrix
   * @param big      whether using bigmatrix or not
   * @param Xx       design matrix if not using bigmatrix
   * @param Yy       response values
   * @param n_passes number of passes for data
   */
private:
  Rcpp::XPtr<BigMatrix> xpMat_;
  std::vector<unsigned> idxmap_; // index to data point for each iteration
public:
  mat X;
  mat Y;
  bool big;
  unsigned n_samples;
  unsigned n_features;

  data_set(const SEXP& xpMat, bool big, const mat& Xx, const mat& Yy,
    unsigned n_passes) : xpMat_(xpMat), big(big), Y(Yy) {
    if (!big) {
      X = Xx;
      n_samples = X.n_rows;
      n_features = X.n_cols;
    } else {
      n_samples = xpMat_->nrow();
      n_features = xpMat_->ncol();
    }
    idxmap_ = std::vector<unsigned>(n_samples*n_passes);
    // std::srand(unsigned(std::time(0)));
    std::srand(0);
    for (unsigned i = 0; i < n_passes; ++i) {
      for (unsigned j = 0; j < n_samples; ++j) {
        idxmap_[i * n_samples + j] = j;
      }
      std::random_shuffle(idxmap_.begin() + i * n_samples,
                          idxmap_.begin() + (i + 1) * n_samples);
    }
  }

  data_point get_data_point(unsigned t) const {
    /* Return the @t th data point */
    t = t - 1;
    mat xt;
    if (!big) {
      xt = mat(X.row(idxmap_[t]));
    } else {
      MatrixAccessor<double> matacess(*xpMat_);
      xt = mat(1, n_features);
      for (unsigned i=0; i < n_features; ++i) {
        xt(0, i) = matacess[i][idxmap_[t]];
      }
    }
    double yt = Y(idxmap_[t]);
    return data_point(xt, yt);
  }

  friend std::ostream& operator<<(std::ostream& os, const data_set& data) {
    os << "  Data set:\n"
       << "    X has " << data.n_features << " features\n"
       << "    Total of " << data.n_samples << " data points" << std::endl;
    return os;
  }
};

#endif
