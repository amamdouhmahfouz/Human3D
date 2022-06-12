#include "BuildSSM.h"


BuildSSM::BuildSSM() {

}

BuildSSM::BuildSSM(const std::string& dir_path) {
    std::cout << "directory: " << dir_path << "\n";
    add_files_to_vec(dir_path, "json", metadata_files);
    add_files_to_vec(dir_path, "obj", meshes_files);
}

BuildSSM::BuildSSM(const std::string& dir_path, const std::string& reference_obj) {
    std::cout << "directory: " << dir_path << "\n";
    add_files_to_vec(dir_path, "obj", meshes_files);

    // read in the reference mesh to get the faces and texture coordinates
    ObjLoader::readSmplUVRef(reference_obj, referenceObj.points, referenceObj.triangleCells, referenceObj.textureCoords);
}

void BuildSSM::add_files_to_vec(const std::string& dir, const std::string& delim, std::vector<std::string>& v) {
    for (const auto& file: std::filesystem::directory_iterator(dir)) {
        std::string filename = file.path();
        std::size_t pos = filename.find(delim);
        if (pos >= 0 && pos < filename.size()) {
            v.push_back(filename);
        }
    }
}

BuildSSM::~BuildSSM() {
 
}
// file_extension example: .json, .obj
std::string BuildSSM::getNameFromFile(const std::string& filepath, const std::string& file_extension) {
    std::size_t start_index = filepath.find_last_of('/')+1;
    std::size_t last_index = filepath.find(file_extension);
    std::string name = filepath.substr(start_index, last_index-start_index);
    return name;
}

std::string BuildSSM::getCorrespondingMetadataFile(const std::string& mesh_file_path) {
    std::string mesh_filename = getNameFromFile(mesh_file_path, ".obj");

    for (const auto& metadata_file: metadata_files ) {
        std::string metadata_filename = getNameFromFile(metadata_file, ".json");
        if (mesh_filename == metadata_filename) {
            return metadata_file;
        }
    }

    return "";
}

void BuildSSM::createModelsFromFiles() {
    std::cout << "meshes_files.size(): " << meshes_files.size() << "\n";
    for (int i = 0; i < meshes_files.size(); i++) {
        Model m(meshes_files[i]);
        models.push_back(m);
        //std::cout << "loaded model #" << i+1 << "\n";
        // search for its corresponding json file 
        std::string metadata_filepath = this->getCorrespondingMetadataFile(meshes_files[i]);
        models[i].setMetadata(metadata_filepath);
    }

    for (int i = 0; i < models.size(); i++) {
        std::cout << models[i].getName() << "   height: " << models[i].getHeight() << "\n";
    }
}

void BuildSSM::createModelsSmpl() {
    std::cout << "number of meshes: " << meshes_files.size() << "\n";
    for (int i = 0; i < meshes_files.size(); i++) {
        Model m(meshes_files[i], referenceObj.triangleCells, referenceObj.textureCoords);
        models.push_back(m);
    }
    std::cout << "[BuildSSM::createModelsSmpl]::done\n";
}

void BuildSSM::createStatsFromMetadata() {
    averageHeight = 0.0;
    averageWeight = 0.0;

    for (const auto& model: models) {
        averageHeight += model.getHeight();
        averageWeight += model.getWeight();
    }

    averageHeight /= models.size();
    averageWeight /= models.size();
}

void BuildSSM::createDeformationFields() {
    
    // assign reference and rest of models
    referenceModel = &models[0];
    // for(int i = 1; i < models.size(); i++) {
    //     restOfModels.push_back(models[i]);
    // }

    // for (int i = 0; i < restOfModels.size(); i++) {
    //     // std::vector<glm::vec3> d;
    //     std::vector<DeformationField> d;
    //     for (int j = 0; j < restOfModels[i].mesh.points.size(); j++) {
    //         //std::cout << "restOfModels[i].mesh.points["<<j << "].index: " << restOfModels[i].mesh.points[j].index << "\n";
    //         //d.push_back(restOfModels[i].mesh.points[j].position - referenceModel->mesh.points[j].position);
    //         DeformationField df;
    //         df.vectorField = restOfModels[i].mesh.points[j].position - referenceModel->mesh.points[j].position;
    //         df.index = restOfModels[i].mesh.points[j].index;
    //         d.push_back(df);
    //     }
    //     deformationFields.push_back(d);
    // }

    for (int i = 1; i < models.size(); i++) {
        std::vector<DeformationField> d;
        for (int j = 0; j < models[i].mesh.points.size(); j++) {
            DeformationField df;
            df.vectorField = models[i].mesh.points[j].position - referenceModel->mesh.points[j].position;
            df.index = models[i].mesh.points[j].index;
            d.push_back(df);
        }
        deformationFields.push_back(d);
    }

}

Model BuildSSM::createMeanModel() {
    
    Mesh meanMesh;
    float meanHeight = 0.0;
    float meanWeight = 0.0;
    std::vector<Point<glm::vec3>> meanPoints;

    glm::vec3 v(0.0f,0.0f,0.0f);
    // initialize positions of meanPoints with zeros
    for (int i = 0; i < models[0].mesh.points.size(); i++) {
        meanPoints.push_back(Point<glm::vec3>(v, models[0].mesh.points[i].index));
    }

    for (int i = 0; i < models.size(); i++) {
        for (int j = 0; j < models[i].mesh.points.size(); j++) {
            meanPoints[j].position += models[i].mesh.points[j].position;
        }
        meanHeight += models[i].getHeight();
        meanWeight += models[i].getWeight();
    }

    meanHeight /= float(models.size());
    meanWeight /= float(models.size());

    // divide by size to get the mean points
    for (int i = 0; i < meanPoints.size(); i++) {
        meanPoints[i].position /= meanPoints.size();
    }

    meanMesh.setMesh(meanPoints, models[0].mesh.triangleCells);

    Model meanModel(meanMesh, meanWeight, meanHeight);
    return meanModel;
}

void BuildSSM::computeGPA() {

    float error = 0.0;

    // 1. deTranslate all models
    // 2. deScale all models
    for (int i = 0; i < models.size(); i++) {
        models[i].mesh.deTranslate();
        models[i].mesh.deScale();
    }
    // 3. choose arbitray reference model
    Model ref = models[0];

    // // TODO: check if greater than or less than previous error, i.e make it a while loop, do something ba2a fo this
    // for (int k = 0; k < 2; k++) {
    //     // 4. align all models to the reference model
    //     for (int i = 0; i < models.size(); i++) {
    //         Alignment::RotationAlignment(models[i], ref); // aligns models[i] in-place(pass by reference)
    //     }
    //     // 5. compute mean shape of ALL the aligned+reference shapes
    //     Model referenceMeanModel = createMeanModel();
    //     // 6. calculate procrustes distance between mean shape and referece
    //     error = Alignment::procrustesDistance(ref, referenceMeanModel);
    //     // 7. procrustes distance is above a certain threshold, set 
    //     std::cout << "error: " << error << "\n";
    //     // 8. the reference to mean shape and go to step 4
    //     ref = referenceMeanModel;
    // }
 
    // // update the actual reference model
    // this->referenceModel = &ref;

    
    for (int i = 1; i < models.size(); i++) {
        //Model referenceMeanModel = createMeanModel();
        //Alignment::ICP(models[i], referenceMeanModel);
        Alignment::ICP(models[i], models[0]);
    }
    
    //exit(0);
}

void BuildSSM::buildCovarianceMatrix() {
    
}

void BuildSSM::createPCA() {

    pcaModel = new PCA(deformationFields);
    //pcaModel = new PCA(models);
    pcaModel->computeEig();


// std::vector<std::vector<float>> vertices;

//     for (int i = 0; i < deformationFields.size(); i++) {
//         std::vector<float> vertices_i;
//         for (int j = 0; j < deformationFields[i].size(); j++) {
//             float x = deformationFields[i][j].vectorField.x;
//             float y = deformationFields[i][j].vectorField.y;
//             float z = deformationFields[i][j].vectorField.z;
//             vertices_i.push_back(x);
//             vertices_i.push_back(y);
//             vertices_i.push_back(z);
//         }

//         vertices.push_back(vertices_i);
//     }

//     int _nrows = vertices.size();
//     int _ncols = vertices[0].size();
 

//     // row #0: x11,y11,z11,x12,y12,z12,x13........,
//     // row #1: x21,y21,z21,....
//     // each row is a sample, i.e all vertices from a model
//     Eigen::MatrixXf _data = Eigen::MatrixXf(_nrows, _ncols);

//     // loop on samples
//     for (int row = 0; row < _nrows; row++) {
//         // loop on vertices
//         for (int col = 0; col < _ncols; col++) {
//             _data(row, col) = vertices[row][col];
//         }
//     }

//     std::cout << "creating empty matrices\n";
//     Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> input_matrix;
// 	Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> centered_matrix;
// 	Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> mean_matrix;
// 	Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> covariance_matrix;




//     std::cout << "centering data\n";
//     centered_matrix = _data.rowwise() - _data.colwise().mean();
//     mean_matrix = _data - centered_matrix;

 
//     //
//     // Compute covariance matrix
//     std::cout << "centered_matrix.rows(): " << centered_matrix.rows() << "\n";
//     std::cout << "centered_matrix.cols(): " << centered_matrix.cols() << "\n";

//     std::cout << "computing cov matrix\n";
//     auto t_start = std::chrono::high_resolution_clock::now();
//     //covariance_matrix = (centered_matrix.adjoint() * centered_matrix) / (float)(input_matrix.rows() - 1);
//     covariance_matrix = (centered_matrix.transpose() * centered_matrix) / (float)(_data.rows() - 1);
//     auto t_end = std::chrono::high_resolution_clock::now();

//     double elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end-t_start).count();
//     std::cout << "elapsed_time_ms: " << elapsed_time_ms << "\n";
//     exit(0);

//     Eigen::LDLT<Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>> cholesky(100);
//     std::cout << "building eigen with new method: \n";
//     Eigen::SelfAdjointEigenSolver<Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic>> eigen_solver(covariance_matrix);
// 	Eigen::Matrix<float, 1, Eigen::Dynamic> eigen_values = eigen_solver.eigenvalues();
// 	Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> eigen_vectors = eigen_solver.eigenvectors();



//     std::cout << "eigen_vectors.rows(): " << eigen_vectors.rows() << "\n";
//     std::cout << "eigen_vectors.cols(): " << eigen_vectors.cols() << "\n";
//     std::cout << "eigen_values.rows(): " << eigen_values.rows() << "\n";
//     std::cout << "eigen_values.cols(): " << eigen_values.cols() << "\n";
//     int x;
//     std::cin >> x;
   
}




Mesh BuildSSM::vectorXfToMesh(Eigen::VectorXf vec) {

    Mesh newMesh;
    std::vector<Point<glm::vec3>> points;
    // triangle cells vertices are the same across all models
    std::vector<TriangleCell> triangleCells = models[0].mesh.triangleCells;

    unsigned int pointIndex = 0;
    for (int i = 0; i < vec.size(); i+=3) {
        float x = vec[i];
        float y = vec[i+1];
        float z = vec[i+2];
        Point<glm::vec3> point(glm::vec3(x,y,z), pointIndex);
        pointIndex++;
        
        points.push_back(point);
    }

    newMesh.setMesh(points, triangleCells);
    newMesh.textureCoords = models[0].mesh.textureCoords;
    // newMesh.computeNormals();
    return newMesh;
}

Eigen::VectorXf BuildSSM::MeshToVectorXf(Mesh m) {
    //std::cout << "m.points.size(): " << m.points.size() << "\n";
    Eigen::VectorXf vec(3*m.points.size());

    int k = 0;
    for (int i = 0; i < m.points.size(); i++) {
        vec[k] = m.points[i].position.x;
        vec[k+1] = m.points[i].position.y;
        vec[k+2] = m.points[i].position.z;
        k += 3;
    }

    return vec;
}

Mesh BuildSSM::sampleSSM(Eigen::VectorXf coefficients) {
    // multiply the coefficients by the eigen vectors matrix
    Eigen::VectorXf projection = pcaModel->getProjection(coefficients);

    //std::cout << "projection.sum(): " << projection.sum() << "\n";
    Model meanModel = createMeanModel();

    Eigen::VectorXf meanVector = MeshToVectorXf(meanModel.mesh);

    Eigen::VectorXf sample = meanVector + projection;

    Mesh sampledMesh = vectorXfToMesh(sample);
    sampledMesh.textureCoords = models[0].mesh.textureCoords;
    sampledMesh.pointIds = models[0].mesh.pointIds;
    return sampledMesh;

}

Mesh BuildSSM::sampleSSMSmpl(Eigen::VectorXf coefficients) {
    // multiply the coefficients by the eigen vectors matrix
    Eigen::VectorXf projection = pcaModel->getProjection(coefficients);

    //std::cout << "projection.sum(): " << projection.sum() << "\n";
    Model meanModel = createMeanModel();

    Eigen::VectorXf meanVector = MeshToVectorXf(meanModel.mesh);

    Eigen::VectorXf sample = meanVector + projection;

    Mesh sampledMesh = vectorXfToMesh(sample);
    sampledMesh.textureCoords = referenceObj.textureCoords;
    sampledMesh.triangleCells = referenceObj.triangleCells;
    sampledMesh.pointIds = models[0].mesh.pointIds;
    return sampledMesh;

}


void BuildSSM::readIdsIndicesLandmarks(const std::string& json_path) {
    std::ifstream json_file(json_path);
    // read as json file
    json_file >> this->idsIndicesJson;
    json_file.close();

    std::cout << "idsIndicesJson.size(): " << idsIndicesJson.size() << "\n";
}

void BuildSSM::saveLandmarks(std::string json_path, Mesh m) {
    
    nlohmann::json newLmsJson;
    int k = 0;
    for (nlohmann::json::iterator it = idsIndicesJson.begin(); it != idsIndicesJson.end(); ++it) {
        // get id and index from idsIndicesJson
        std::string id = (*it)["id"];
        unsigned int index = (*it)["index"];
        //std::cout << "index: " << index << "\n";
        Point<glm::vec3> point = m.getPointAtIndex(index);

        // save in newLmsJson
        newLmsJson[k]["id"] = id;
        newLmsJson[k]["coordinates"] = {point.position.x, point.position.y, point.position.z};
        k++;
    }
    std::ofstream writeJson(json_path);
    writeJson << std::setw(4) << newLmsJson << std::endl;
    std::cout << "saved landmarks\n";
}

void BuildSSM::savePCAModel(const std::string& model_path) {
    
    std::cout << "saving pca model\n";
    const H5std_string FILE_NAME(model_path);
    const H5std_string DATASET_NAME1("/model/mean");
    const H5std_string DATASET_NAME2("/model/noiseVariance");
    const H5std_string DATASET_NAME3("/model/pcaBasis");
    const H5std_string DATASET_NAME4("/model/pcaVariance");

    //const H5std_string DATASET_NAME5("/objStructure/verticesCells"); // vertices indices cells
    //const H5std_string DATASET_NAME6("/objStructure/textureCells"); // texture coordinates indices cells
    //const H5std_string DATASET_NAME7("/objStructure/textureCoordinates");

    Model mean = createMeanModel();
    std::cout << "created mean model\n";
    Eigen::VectorXf meanVector = MeshToVectorXf(mean.mesh);
    std::cout << "created mean vector\n";
    Eigen::MatrixXf eigenVectors = pcaModel->getEigenVectors();
    std::cout << "created eigenVectors\n";
    Eigen::VectorXf eigenValues = pcaModel->getEigenValues();
    std::cout << "created eigenValues\n";

    //const int DIM_MEAN = 3*10475;
    const int DIM_MEAN = 3*6890;
    const int DIM_NOISE_VARIANCE = 1;
    //const int rowsPCA = 31425;
    const int rowsPCA = models[0].mesh.points.size()*3;
    //const int colsPCA = 79;
    //const int colsPCA = 299;
    const int colsPCA = models.size() - 1;

    // TODO: ********** define the following arrays dynamically (i.e using new)
    std::cout << "created mean_data before\n";
    float *_mean_data = new float[DIM_MEAN];
    //float mean_data[DIM_MEAN];
    std::cout << "created mean_data\n";
    float *_noiseVariance = new float[DIM_NOISE_VARIANCE];
    //float noiseVariance[DIM_NOISE_VARIANCE];
    //float **_pcaBasis = new float*[rowsPCA];
    //for (int i = 0; i < rowsPCA; i++) _pcaBasis[i] = new float[colsPCA];
    float *_pcaBasis = new float[rowsPCA*colsPCA];
    //float pcaBasis[rowsPCA][colsPCA];
    std::cout << "created pcaBasis\n";
    float *_pcaVariance = new float[colsPCA];
    //float pcaVariance[colsPCA];
    std::cout << "created pcaVariance\n";
    

    try {
        // Turn off the auto-printing when failure occurs so that we can
        // handle the errors appropriately
        Exception::dontPrint();

        // 1. Create a new file using the default property lists.
        H5File file(FILE_NAME, H5F_ACC_TRUNC);

        // 2. Create Groups
        Group groupModel(file.createGroup("/model"));

        // 3. Create Data
        for (int i = 0; i < DIM_MEAN; i++){
            _mean_data[i] = meanVector[i];
        }
        _noiseVariance[0] = 0.0;
        std::cout << "mean_data saved\n";
        for (int i = 0; i < rowsPCA; i++) {
            for (int j = 0; j < colsPCA; j++) {
                _pcaBasis[i*colsPCA + j] = eigenVectors(i,j);
            }
        }
        std::cout << "pcaBasis saved\n";
        for (int i = 0; i < colsPCA; i++) {
            _pcaVariance[i] = eigenValues[i];
            //std::cout << "pcaVariance[i]: " << pcaVariance[i] << "\n"; 
        }

        // 4. Create dataspace for mean
        hsize_t dimsMean[1]; // dataset dimensions
        dimsMean[0]              = DIM_MEAN;
        DataSpace *dataspace = new DataSpace(1, dimsMean);
        // 5. Create the dataset in group
        DataSet *dataset = new DataSet(file.createDataSet(DATASET_NAME1, PredType::NATIVE_FLOAT, *dataspace));
        // Write the data to the dataset using default memory space, file
        // space, and transfer properties.
        dataset->write(_mean_data, PredType::NATIVE_FLOAT);
        // Close the current dataset and data space.
        delete dataset;
        delete dataspace;

        // noiseVariance
        hsize_t dimsNoiseVariance[1];
        dimsNoiseVariance[0]   = 1;
        dataspace = new DataSpace(1, dimsNoiseVariance);
        dataset = new DataSet(groupModel.createDataSet(DATASET_NAME2, PredType::NATIVE_FLOAT, *dataspace));
        dataset->write(_noiseVariance, PredType::NATIVE_FLOAT);
        delete dataset;
        delete dataspace;


    // pcaVariance
        hsize_t dimsPCAVariance[1];
        dimsPCAVariance[0]   = colsPCA;
        dataspace = new DataSpace(1, dimsPCAVariance);
        dataset = new DataSet(groupModel.createDataSet(DATASET_NAME4, PredType::NATIVE_FLOAT, *dataspace));
        dataset->write(_pcaVariance, PredType::NATIVE_FLOAT);
        delete dataset;
        delete dataspace;


        // pcaBasis
        hsize_t dimsPCABasis[2];
        dimsPCABasis[0]   = rowsPCA;
        dimsPCABasis[1]   = colsPCA;
        dataspace = new DataSpace(2, dimsPCABasis);
        dataset = new DataSet(groupModel.createDataSet(DATASET_NAME3, PredType::NATIVE_FLOAT, *dataspace));
        dataset->write(_pcaBasis, PredType::NATIVE_FLOAT);
        delete dataset;
        delete dataspace;

        // // pcaVariance
        // hsize_t dimsPCAVariance[1];
        // dimsPCAVariance[0]   = colsPCA;
        // dataspace = new DataSpace(1, dimsPCAVariance);
        // dataset = new DataSet(groupModel.createDataSet(DATASET_NAME4, PredType::NATIVE_FLOAT, *dataspace));
        // dataset->write(pcaVariance, PredType::NATIVE_FLOAT);
        // delete dataset;
        // delete dataspace;

        delete [] _mean_data;
        delete [] _noiseVariance;
        delete [] _pcaVariance;
        //for (int i = 0; i < rowsPCA; i++) delete[] _pcaBasis[i];
        delete[] _pcaBasis;

        groupModel.close();

    } // end of try block

    // catch failure caused by the H5File operations
    catch (FileIException error) {
        error.printErrorStack();
        return;
    }

    // catch failure caused by the DataSet operations
    catch (DataSetIException error) {
        error.printErrorStack();
        return;
    }

    // catch failure caused by the DataSpace operations
    catch (DataSpaceIException error) {
        error.printErrorStack();
        return;
    }

}

void BuildSSM::loadPCAModel(const std::string& model_path, const std::string& reference_obj_path) {

    // 1. read and save reference mesh from reference_obj_path
    std::vector<TriangleCell> cells;
    ObjLoader::loadObj(reference_obj_path, referenceMesh.points, referenceMesh.pointIds, referenceMesh.triangleCells, referenceMesh.normals, referenceMesh.textureCoords);

    // for (auto face : referenceMesh.triangleCells) {
    //     std::cout << "load face.indexVertex1: " << face.indexVertex1 << ", face.indexVertex2: " << face.indexVertex2 << ", face.indexVertex3: " << face.indexVertex3 << "\n";
    // }
    // std::cout << "finished printing\n";
    // 2. read and save mean mesh
    // 3. read and save eigen vectors
    // 4. read and save eigen values
    

    const H5std_string FILE_NAME(model_path);
    const H5std_string MEAN_DATASET_NAME("/model/mean");
    const H5std_string PCA_BASIS_DATASET_NAME("/model/pcaBasis");
    //const int          DIM0 = 31425; // dataset dimensions
    const int          DIM0 = referenceMesh.points.size()*3; // dataset dimensions
    //const int          DIM1 = 79;
    //const int          DIM1 = 299;
    const int          DIM1 = 629;
    const int RANK = 2;

    Eigen::VectorXf meanVector(DIM0);

    float *_mean_data = new float[DIM0];
    //std::cout << "allocated _mean_data\n";
    //float **_eigenVectors = new float*[DIM0];
    //for (int i = 0; i < DIM0; i++) _eigenVectors[i] = new float[DIM1];
    float *_eigenVectors = new float[DIM0*DIM1];
    //std::cout << "allocated _eigenVectors\n";
    pcaBasis = Eigen::MatrixXf(DIM0, DIM1);
    //std::cout << "allocated pcaBasis\n";

    try {
        // Turn off the auto-printing when failure occurs so that we can
        // handle the errors appropriately
        Exception::dontPrint();

        // Open an existing file and dataset.
        H5File  file(FILE_NAME, H5F_ACC_RDONLY);
        DataSet dataset = file.openDataSet(MEAN_DATASET_NAME);
    

        // Write the data to the dataset using default memory space, file
        // space, and transfer properties.
        dataset.read(_mean_data, PredType::NATIVE_FLOAT);

        // read mean data
        for (int i = 0; i < DIM0; i++) {
            meanVector[i] = _mean_data[i];
        }
        //std::cout << "read meanVector\n";
        meanMesh = vectorXfToMesh_w_reference(meanVector);
        //std::cout << "meanMesh in loadPCA\n";




        dataset = file.openDataSet(PCA_BASIS_DATASET_NAME);
        //std::cout << "opened dataset for pcaBasis\n";
        
    


        dataset.read(_eigenVectors, PredType::NATIVE_FLOAT);
        //std::cout << "read dataset _eigenVectors\n";

        for (int i = 0; i < DIM0; i++) {
            for (int j = 0; j < DIM1; j++) {
                //pcaBasis(i,j) = _eigenVectors[i][j];
                pcaBasis(i,j) = _eigenVectors[i*DIM1 + j];
                //std::cout << "pcaBases("<<i<<","<<j<<"): " <<pcaBasis(i,j) << "\n";
            }
        }
        //std::cout << "read pcaBasis loadPca\n";


        delete [] _mean_data;
        //for (int i = 0; i < DIM0; i++) delete[] _eigenVectors[i];
        delete[] _eigenVectors;

    } // end of try block

    // catch failure caused by the H5File operations
    catch (FileIException error) {
        error.printErrorStack();
        return;
    }

    // catch failure caused by the DataSet operations
    catch (DataSetIException error) {
        error.printErrorStack();
        return;
    }

    std::cout << "pcaModel loaded...\n";
}


void BuildSSM::loadPCAModelSmpl(const std::string& model_path, const std::string& reference_obj_path) {

    // 1. read and save reference mesh from reference_obj_path
    std::vector<TriangleCell> cells;
    ObjLoader::readSmplUVRef(reference_obj_path, referenceMesh.points,  referenceMesh.triangleCells, referenceMesh.textureCoords);

    // for (auto face : referenceMesh.triangleCells) {
    //     std::cout << "load face.indexVertex1: " << face.indexVertex1 << ", face.indexVertex2: " << face.indexVertex2 << ", face.indexVertex3: " << face.indexVertex3 << "\n";
    // }
    // std::cout << "finished printing\n";
    // 2. read and save mean mesh
    // 3. read and save eigen vectors
    // 4. read and save eigen values
    

    const H5std_string FILE_NAME(model_path);
    const H5std_string MEAN_DATASET_NAME("/model/mean");
    const H5std_string PCA_BASIS_DATASET_NAME("/model/pcaBasis");
    //const int          DIM0 = 31425; // dataset dimensions
    const int          DIM0 = referenceMesh.points.size()*3; // dataset dimensions
    //const int          DIM1 = 79;
    //const int          DIM1 = 299;
    const int          DIM1 = 629;
    const int RANK = 2;

    std::cout << "DIM0: " << DIM0 << "\n";

    Eigen::VectorXf meanVector(DIM0);

    float *_mean_data = new float[DIM0];
    //std::cout << "allocated _mean_data\n";
    //float **_eigenVectors = new float*[DIM0];
    //for (int i = 0; i < DIM0; i++) _eigenVectors[i] = new float[DIM1];
    float *_eigenVectors = new float[DIM0*DIM1];
    //std::cout << "allocated _eigenVectors\n";
    pcaBasis = Eigen::MatrixXf(DIM0, DIM1);
    //std::cout << "allocated pcaBasis\n";

    try {
        // Turn off the auto-printing when failure occurs so that we can
        // handle the errors appropriately
        Exception::dontPrint();

        // Open an existing file and dataset.
        H5File  file(FILE_NAME, H5F_ACC_RDONLY);
        DataSet dataset = file.openDataSet(MEAN_DATASET_NAME);
    

        // Write the data to the dataset using default memory space, file
        // space, and transfer properties.
        dataset.read(_mean_data, PredType::NATIVE_FLOAT);

        // read mean data
        for (int i = 0; i < DIM0; i++) {
            meanVector[i] = _mean_data[i];
        }
        //std::cout << "read meanVector\n";
        meanMesh = vectorXfToMesh_w_reference(meanVector);
        //std::cout << "meanMesh in loadPCA\n";




        dataset = file.openDataSet(PCA_BASIS_DATASET_NAME);
        //std::cout << "opened dataset for pcaBasis\n";
        
    


        dataset.read(_eigenVectors, PredType::NATIVE_FLOAT);
        //std::cout << "read dataset _eigenVectors\n";

        for (int i = 0; i < DIM0; i++) {
            for (int j = 0; j < DIM1; j++) {
                //pcaBasis(i,j) = _eigenVectors[i][j];
                pcaBasis(i,j) = _eigenVectors[i*DIM1 + j];
                //std::cout << "pcaBases("<<i<<","<<j<<"): " <<pcaBasis(i,j) << "\n";
            }
        }
        //std::cout << "read pcaBasis loadPca\n";


        delete [] _mean_data;
        //for (int i = 0; i < DIM0; i++) delete[] _eigenVectors[i];
        delete[] _eigenVectors;

    } // end of try block

    // catch failure caused by the H5File operations
    catch (FileIException error) {
        error.printErrorStack();
        return;
    }

    // catch failure caused by the DataSet operations
    catch (DataSetIException error) {
        error.printErrorStack();
        return;
    }

    std::cout << "pcaModel loaded...\n";
}

Mesh BuildSSM::vectorXfToMesh_w_reference(Eigen::VectorXf vec) {
    Mesh newMesh;
    std::vector<Point<glm::vec3>> points;
    // triangle cells vertices are the same across all models
    std::vector<TriangleCell> triangleCells = referenceMesh.triangleCells;

    // for (auto face : referenceMesh.triangleCells) {
    //     std::cout << "vec face.indexVertex1: " << face.indexVertex1 << ", face.indexVertex2: " << face.indexVertex2 << ", face.indexVertex3: " << face.indexVertex3 << "\n";
    // }

    unsigned int pointIndex = 0;
    for (int i = 0; i < vec.size(); i+=3) {
        float x = vec[i];
        float y = vec[i+1];
        float z = vec[i+2];
        Point<glm::vec3> point(glm::vec3(x,y,z), pointIndex);
        pointIndex++;
        
        points.push_back(point);
    }

    newMesh.setMesh(points, triangleCells);
    newMesh.textureCoords = referenceMesh.textureCoords;
    // newMesh.computeNormals();
    return newMesh;
}

Mesh BuildSSM::instance(Eigen::VectorXf coefficients) {
    //Eigen::VectorXf projection = this->pcaBasis.block(0,0,31425,10) * coefficients;
    Eigen::VectorXf projection = this->pcaBasis.block(0,0,6890*3,10) * coefficients;
    std::cout << "projection.size(): " << projection.size() << "\n";
    //Eigen::VectorXf projection = this->pcaBasis.block(0,0,31425,299) * coefficients;

    // for (int i = 0; i < this->pcaBasis.block(0,0,31425,10).rows(); i++) {
    //     for(int j = 0; j < this->pcaBasis.block(0,0,31425,10).cols(); j++) {
    //         std::cout << this->pcaBasis.block(0,0,31425,10)(i,j) << " ";
    //     }
    //     std::cout << "\n";
    // }

    // for (auto face : referenceMesh.triangleCells) {
    //     std::cout << "face.indexVertex1: " << face.indexVertex1 << ", face.indexVertex2: " << face.indexVertex2 << ", face.indexVertex3: " << face.indexVertex3 << "\n";
    // }

    //std::cout << "projection.sum(): " << projection.sum() << "\n";
    
    std::cout << "before mean vector\n";
    Eigen::VectorXf meanVector = MeshToVectorXf(meanMesh);
    std::cout << "after meanVector\n";
    std::cout << "meanVector.size(): " << meanVector.size() << "\n";
    //std::cout << "meanVector done\n";
    //std::cout << "meanVector.size(): " << meanVector.size() << ", " << "projection.size(): " << projection.size() << "\n";
    Eigen::VectorXf sample = meanVector + projection;
    //std::cout << "sample done\n";
    Mesh sampledMesh = vectorXfToMesh_w_reference(sample);
    sampledMesh.textureCoords = referenceMesh.textureCoords;
    sampledMesh.pointIds = referenceMesh.pointIds;
    
    // added for smpl
    sampledMesh.triangleCells = referenceMesh.triangleCells;
    // end added 

    std::cout << "before computeNormals\n";
    sampledMesh.computeNormals();
    std::cout << "after computeNormals\n";
    return sampledMesh;
}



Mesh BuildSSM::instanceNoNormals(Eigen::VectorXf coefficients) {
    ////Eigen::VectorXf projection = this->pcaBasis.block(0,0,31425,299) * coefficients;
    //Eigen::VectorXf projection = this->pcaBasis.block(0,0,31425,10) * coefficients;
    Eigen::VectorXf projection = this->pcaBasis.block(0,0,6890*3,10) * coefficients;


    Eigen::VectorXf meanVector = MeshToVectorXf(meanMesh);
    //std::cout << "meanVector done\n";
    //std::cout << "meanVector.size(): " << meanVector.size() << ", " << "projection.size(): " << projection.size() << "\n";
    Eigen::VectorXf sample = meanVector + projection;
    //std::cout << "sample done\n";
    Mesh sampledMesh = vectorXfToMesh_w_reference(sample);
    sampledMesh.textureCoords = referenceMesh.textureCoords;
    sampledMesh.pointIds = referenceMesh.pointIds;
    //sampledMesh.computeNormals();
    return sampledMesh;
}

nlohmann::json BuildSSM::getIdsIndicesJson() const {
    return idsIndicesJson;
}