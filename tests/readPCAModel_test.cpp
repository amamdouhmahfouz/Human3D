#include <iostream>
#include <string> 
#include "BuildSSM.h"

// #include "H5Cpp.h"
// using namespace H5;

// const H5std_string FILE_NAME("/Users/abdelrahmanabdelghany/Documents/college/semester10/GP/Human3D/tests/pcaModel.h5");
// const H5std_string DATASET_NAME("/model/mean");
// const H5std_string DATASET_NAME2("/model/pcaBasis");
// const int          DIM0 = 31425; // dataset dimensions
// const int          DIM1 = 9;


int main(int argc, char *argv[]) {

    BuildSSM ssm;
    ssm.loadPCAModel("/Users/abdelrahmanabdelghany/Documents/college/semester10/GP/Human3D/tests/pcaModel_male_T.h5",
        "/Users/abdelrahmanabdelghany/Documents/college/semester10/GP/Human3D/tests/referenceObj.obj");

    Eigen::VectorXf coefficients(10);
    coefficients.setZero();
    coefficients[atoi(argv[1])] = std::stof(argv[2]);
    std::cout << "argv[1]: " << std::stof(argv[1]) << "\n";
    Mesh sampledMesh =  ssm.instance(coefficients); 

    sampledMesh.scale(1000.0);
    ObjLoader::saveObj("/Users/abdelrahmanabdelghany/Documents/college/semester10/GP/Human3D/tests/meshTestSampledFromSavedModel.obj",
     sampledMesh.points, sampledMesh.pointIds, sampledMesh.triangleCells,
     sampledMesh.normals, sampledMesh.textureCoords);

    ssm.readIdsIndicesLandmarks("/Users/abdelrahmanabdelghany/Documents/college/semester10/GP/Human3D/tests/ids_index.json");
    ssm.saveLandmarks("/Users/abdelrahmanabdelghany/Documents/college/semester10/GP/Human3D/tests/meshTestSampledFromSavedModelLandmarks.json",sampledMesh);
    

    std::cout << "sampling pca model done.\n";
    return 0;
}