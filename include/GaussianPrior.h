#ifndef GAUSSIANPRIOR_H
#define GAUSSIANPRIOR_H

#include <Eigen/Dense>

class GaussianPrior {
public:
    static float evaluateLogPrior(float mean, float standardDeviation, Eigen::VectorXf coefficients);
};

#endif // GAUSSIANPRIOR_H