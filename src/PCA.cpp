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
    //Eigen::JacobiSVD<Eigen::MatrixXf> eigSVD(data, Eigen::ComputeThinV);

    int k = 2;
    eigenValues = eigSVD.singularValues();
    std::cout << "accuracy " << eigenValues.head(k).sum() / eigenValues.sum() * 1.0 << std::endl;
    
    // TODO: apply threshold to get the best PCA vectors (best principal components)
    
    //std::cout << "eigSVD.matrixV().rows(): " << eigSVD.matrixV().rows() << "\n";
    // eigenVectors =  eigSVD.matrixV().block(0, 0, this->ncols, k);
    eigenVectors =  eigSVD.matrixV().block(0, 0, this->ncols, this->nrows);
    

    for (int i = 0; i < eigenValues.size(); i++) {
        std::cout << "eigenValue: " << eigenValues[i] << "\n";
    }

    std::cout << "eigenVector.size(): " << eigenValues.size() << "\n";
    std::cout << "eigenVectors.rows(): " << eigenVectors.rows() << "\n";
    std::cout << "eigenVectors.cols(): " << eigenVectors.cols() << "\n";

    std::cout << "eigSVD.matrixV().rows(): " << eigSVD.matrixV().rows() << "\n";
    std::cout << "eigSVD.matrixV().cols(): " << eigSVD.matrixV().cols() << "\n";
    std::cout << "eigSVD.matrixU().rows(): " << eigSVD.matrixU().rows() << "\n";
    std::cout << "eigSVD.matrixU().cols(): " << eigSVD.matrixU().cols() << "\n";
    std::cout << "eigSVD.singularValues().asDiagonal().rows(): " << eigSVD.singularValues().asDiagonal().rows() << "\n";
    std::cout << "eigSVD.singularValues().asDiagonal().cols(): " << eigSVD.singularValues().asDiagonal().cols() << "\n";


    // std::cout << "mean.rows(): " << mean.rows() << "\n";
    // std::cout << "mean.cols(): " << mean.cols() << "\n";

    // Eigen::VectorXf mean2 = data.rowwise().mean();
    // std::cout << "mean2.rows(): " << mean2.rows() << "\n";
    // std::cout << "mean2.cols(): " << mean2.cols() << "\n"; 

    // std::cout << "------------------ using eigen solver ---------------------\n";
    // Eigen::EigenSolver<Eigen::MatrixXf> solver;
    // solver.compute(data, true);
    // std::cout << "The eigenvalues of data are: " << solver.eigenvalues() << endl;
  
    // std::cout << "------------------ finished eigen solver ---------------------\n";

    std::cout << "------------------ using svdNew ---------------------\n";
    Eigen::MatrixXf covariance = (data * data.transpose()) * ((float)1.0f/(float)(data.rows()-1));
    Eigen::JacobiSVD<Eigen::MatrixXf> svdNew(covariance, Eigen::ComputeThinV);
    Eigen::VectorXf singularValues = svdNew.singularValues();
    Eigen::MatrixXf matV = svdNew.matrixV();
    Eigen::VectorXf singSqrt = singularValues.array().sqrt();
    Eigen::VectorXf singSqrtInv = Eigen::VectorXf::Zero(singSqrt.rows());

    unsigned numComponentsAboveTolerance =
          ((singularValues.array() - 0.0 - 1e-6) > 0).count();
    std::cout << "numComponentsAboveTolerance: " << numComponentsAboveTolerance << "\n";
    for (int i = 0; i < singSqrt.size(); i++) {
        if (singSqrt(i) >= 1e-6) {
            singSqrtInv(i) = 1.0f / singSqrt(i);
        }
        //std::cout << "singSqrt("<<i<<"): " << singSqrt(i) << "\n";
    }
    Eigen::MatrixXf pcaBasis = data.transpose() * matV * singSqrtInv.asDiagonal();
    pcaBasis /= sqrt(data.rows() - 1.0);
    eigenVectors = pcaBasis;
    eigenValues = singularValues;
    std::cout << "pcaBasis.rows(): " << pcaBasis.rows() << "\n";
    std::cout << "pcaBasis.cols(): " << pcaBasis.cols() << "\n";
    for (int i = 0; i < 15; i++) {
        std::cout << "pcaVariance #"<<i<<": " << singularValues[i] << "\n";
    }
    std::cout << "accuracy first 10 columns: " << eigenValues.head(10).sum() / eigenValues.sum() * 1.0 << std::endl;
    std::cout << "accuracy first 20 columns: " << eigenValues.head(20).sum() / eigenValues.sum() * 1.0 << std::endl;
    std::cout << "accuracy first 30 columns: " << eigenValues.head(30).sum() / eigenValues.sum() * 1.0 << std::endl;
    std::cout << "accuracy first 50 columns: " << eigenValues.head(50).sum() / eigenValues.sum() * 1.0 << std::endl;
    std::cout << "accuracy first 100 columns: " << eigenValues.head(100).sum() / eigenValues.sum() * 1.0 << std::endl;
    std::cout << "accuracy first 150 columns: " << eigenValues.head(150).sum() / eigenValues.sum() * 1.0 << std::endl;
    std::cout << "accuracy first 200 columns: " << eigenValues.head(200).sum() / eigenValues.sum() * 1.0 << std::endl;
    std::cout << "accuracy first 250 columns: " << eigenValues.head(250).sum() / eigenValues.sum() * 1.0 << std::endl;
    std::cout << "accuracy first 300 columns: " << eigenValues.head(300).sum() / eigenValues.sum() * 1.0 << std::endl;
    //std::cout << "Press enter to continue:\n";
    //int ii;
    //std::cin >> ii;
    std::cout << "------------------ finished svdNew ---------------------\n";

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
    ////std::cout << "eigenVectors.rows(): " << eigenVectors.block(0,0,31425,299).rows() << ", eigenVectors.cols(): " << eigenVectors.block(0,0,31425,299).cols() << "\n";
    std::cout << "eigenVectors.rows(): " << eigenVectors.block(0,0,31425,7).rows() << ", eigenVectors.cols(): " << eigenVectors.block(0,0,31425,7).cols() << "\n";
    //std::cout << "eigenVectors.rows(): " << eigenVectors.block(0,0,6890*3,10).rows() << ", eigenVectors.cols(): " << eigenVectors.block(0,0,6890*3,10).cols() << "\n";
    ////return eigenVectors.block(0,0,31425,299) * vec;
    return eigenVectors.block(0,0,31425,7) * vec;
    //return eigenVectors.block(0,0,6890*3,10) * vec;
}

Eigen::VectorXf PCA::getMean() const {
    return mean;
}

Eigen::MatrixXf PCA::getEigenVectors() const {
    return eigenVectors;
}