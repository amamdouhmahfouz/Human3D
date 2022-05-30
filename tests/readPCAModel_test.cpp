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
    ssm.loadPCAModel("/Users/abdelrahmanabdelghany/Documents/college/semester10/GP/Human3D/tests/pcaModel.h5",
        "/Users/abdelrahmanabdelghany/Documents/college/semester10/GP/Human3D/tests/referenceObj.obj");

    Eigen::VectorXf coefficients(9);
    coefficients.setZero();
    coefficients[atoi(argv[1])] = std::stof(argv[2]);
    std::cout << "argv[1]: " << std::stof(argv[1]) << "\n";
    Mesh sampledMesh =  ssm.instance(coefficients); 

    ObjLoader::saveObj("/Users/abdelrahmanabdelghany/Documents/college/semester10/GP/Human3D/tests/meshTestSampledFromSavedModel.obj",
     sampledMesh.points, sampledMesh.pointIds, sampledMesh.triangleCells,
     sampledMesh.normals, sampledMesh.textureCoords);


    // float data[DIM0];
    // float pcaBasis[DIM0][DIM1];

    // // Try block to detect exceptions raised by any of the calls inside it
    // try {
    //     // Turn off the auto-printing when failure occurs so that we can
    //     // handle the errors appropriately
    //     Exception::dontPrint();

    //     // Open an existing file and dataset.
    //     H5File  file(FILE_NAME, H5F_ACC_RDWR);
    //     DataSet dataset = file.openDataSet(DATASET_NAME);
    
    //     // Write the data to the dataset using default memory space, file
    //     // space, and transfer properties.
    //     dataset.read(data, PredType::NATIVE_FLOAT);

    //     for (int i = 0; i < DIM0; i++) {
    //         std::cout << data[i] << "\n";
    //     }

    //     dataset = file.openDataSet(DATASET_NAME2);
    //     dataset.read(pcaBasis, PredType::NATIVE_FLOAT);
    //     for (int i = 0; i < DIM0; i++) {
    //         std::cout << "row#: " << i << "\n";
    //         for (int j = 0; j < DIM1; j++) {
    //             std::cout << pcaBasis[i][j] << "  ";
    //         }
    //         std::cout << "\n";
    //     }

    // } // end of try block

    // // catch failure caused by the H5File operations
    // catch (FileIException error) {
    //     error.printErrorStack();
    //     return -1;
    // }

    // // catch failure caused by the DataSet operations
    // catch (DataSetIException error) {
    //     error.printErrorStack();
    //     return -1;
    // }



    std::cout << "reading pca model done.\n";
    return 0;
}