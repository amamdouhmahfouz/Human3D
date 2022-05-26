#ifndef PCA_H
#define PCA_H

#include "Model.h"
#include <Eigen/SVD>
#include <Eigen/Dense>
#include <cmath>


class PCA {
public:
    PCA(std::vector<Model> models);
    ~PCA();

    Eigen::MatrixXf getData() const;
    // compute eigen vectors and values
    void computeEig();

    Eigen::VectorXf getEigenValues() const;
    Eigen::MatrixXf getTopKEigenVectors(int k) const;
    Eigen::VectorXf getProjection(Eigen::VectorXf vec) const;
    Eigen::VectorXf getMean() const;

private:  
    Eigen::MatrixXf data; // original data
    Eigen::MatrixXf eigenVectors;
    Eigen::VectorXf mean;
    Eigen::VectorXf eigenVector;
    Eigen::VectorXf eigenValues;

    // rows and cols of original data
    unsigned int nrows;
    unsigned int ncols;


};

#endif // PCA_H