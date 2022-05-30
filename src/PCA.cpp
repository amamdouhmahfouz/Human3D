#include "PCA.h"


PCA::PCA(std::vector<Model> models) {

    std::vector<std::vector<float>> vertices;

    // read data from models
    for (int i = 0; i < models.size(); i++) {
        std::vector<float> vertices_i;
        for (int j = 0; j < models[i].mesh.points.size(); j++) {
            float x = models[i].mesh.points[j].position.x;
            float y = models[i].mesh.points[j].position.y;
            float z = models[i].mesh.points[j].position.z;
            vertices_i.push_back(x);
            vertices_i.push_back(y);
            vertices_i.push_back(z);
        }

        vertices.push_back(vertices_i);
    }

    this->nrows = vertices.size();
    this->ncols = vertices[0].size();
 

    // row #0: x11,y11,z11,x12,y12,z12,x13........,
    // row #1: x21,y21,z21,....
    // each row is a sample, i.e all vertices from a model
    data = Eigen::MatrixXf(this->nrows, this->ncols);

    // loop on samples
    for (int row = 0; row < this->nrows; row++) {
        // loop on vertices
        for (int col = 0; col < this->ncols; col++) {
            data(row, col) = vertices[row][col];
        }
    }

    std::cout << "data.rows(): " << data.rows() << "\n";
    std::cout << "data.cols(): " << data.cols() << "\n";


}

PCA::PCA(std::vector<std::vector<DeformationField>> defFields) {
    std::vector<std::vector<float>> vertices;

    for (int i = 0; i < defFields.size(); i++) {
        std::vector<float> vertices_i;
        for (int j = 0; j < defFields[i].size(); j++) {
            float x = defFields[i][j].vectorField.x;
            float y = defFields[i][j].vectorField.y;
            float z = defFields[i][j].vectorField.z;
            vertices_i.push_back(x);
            vertices_i.push_back(y);
            vertices_i.push_back(z);
        }

        vertices.push_back(vertices_i);
    }

    this->nrows = vertices.size();
    this->ncols = vertices[0].size();
 

    // row #0: x11,y11,z11,x12,y12,z12,x13........,
    // row #1: x21,y21,z21,....
    // each row is a sample, i.e all vertices from a model
    data = Eigen::MatrixXf(this->nrows, this->ncols);

    // loop on samples
    for (int row = 0; row < this->nrows; row++) {
        // loop on vertices
        for (int col = 0; col < this->ncols; col++) {
            data(row, col) = vertices[row][col];
        }
    }

    std::cout << "data.rows(): " << data.rows() << "\n";
    std::cout << "data.cols(): " << data.cols() << "\n";

}

void PCA::computeEig() {

    // deMean the data
    mean = data.colwise().mean();

    Eigen::VectorXf identity(this->nrows);
    identity.setOnes();
    data -= (identity * mean.transpose());

    // get eigne vectors and eigen values
    Eigen::JacobiSVD<Eigen::MatrixXf> eigSVD(data, Eigen::ComputeThinU | Eigen::ComputeThinV);

    int k = 8;
    eigenValues = eigSVD.singularValues();
    std::cout << "accuracy " << eigenValues.head(k).sum() / eigenValues.sum() * 1.0 << std::endl;
    
    // TODO: apply threshold to get the best PCA vectors (best principal components)
    
    //std::cout << "eigSVD.matrixV().rows(): " << eigSVD.matrixV().rows() << "\n";
    // eigenVectors =  eigSVD.matrixV().block(0, 0, this->ncols, k);
    eigenVectors =  eigSVD.matrixV().block(0, 0, this->ncols, this->nrows);
    
    //std::cout << "eigenVectors: \n";
    //std::cout << eigenVectors << std::endl;

    for (int i = 0; i < eigenValues.size(); i++) {
        std::cout << "eigenValue: " << eigenValues[i] << "\n";
    }

    std::cout << "eigenVector.size(): " << eigenValues.size() << "\n";
    std::cout << "eigenVectors.rows(): " << eigenVectors.rows() << "\n";
    std::cout << "eigenVectors.cols(): " << eigenVectors.cols() << "\n";
}



Eigen::VectorXf PCA::getEigenValues() const {
    return eigenValues;
}

Eigen::MatrixXf PCA::getTopKEigenVectors(int k) const {
    return eigenVectors.block(0, 0, this->ncols, k);
}

Eigen::VectorXf PCA::getProjection(Eigen::VectorXf vec) const {
    // return eigenVectors.transpose() * vec;
    std::cout << "vec.rows(): " << vec.rows() << "\n";
    std::cout << "vec.cols(): " << vec.cols() << "\n";
    std::cout << "eigenVectors.rows(): " << eigenVectors.block(0,0,31425,10).rows() << ", eigenVectors.cols(): " << eigenVectors.block(0,0,31425,10).cols() << "\n";
    return eigenVectors.block(0,0,31425,10) * vec;
}

Eigen::VectorXf PCA::getMean() const {
    return mean;
}

Eigen::MatrixXf PCA::getEigenVectors() const {
    return eigenVectors;
}