#include "BuildSSM.h"


BuildSSM::BuildSSM(const std::string& dir_path) {
    std::cout << "directory: " << dir_path << "\n";
    add_files_to_vec(dir_path, "json", metadata_files);
    add_files_to_vec(dir_path, "obj", meshes_files);


}

BuildSSM::BuildSSM() {

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

    // TODO: check if greater than or less than previous error, i.e make it a while loop, do something ba2a fo this
    for (int k = 0; k < 2; k++) {
        // 4. align all models to the reference model
        for (int i = 0; i < models.size(); i++) {
            Alignment::RotationAlignment(models[i], ref); // aligns models[i] in-place(pass by reference)
        }
        // 5. compute mean shape of ALL the aligned+reference shapes
        Model referenceMeanModel = createMeanModel();
        // 6. calculate procrustes distance between mean shape and referece
        error = Alignment::procrustesDistance(ref, referenceMeanModel);
        // 7. procrustes distance is above a certain threshold, set 
        std::cout << "error: " << error << "\n";
        // 8. the reference to mean shape and go to step 4
        ref = referenceMeanModel;
    }
 
    // update the actual reference model
    this->referenceModel = &ref;

}

void BuildSSM::buildCovarianceMatrix() {
    
}

void BuildSSM::createPCA() {
    //pcaModel = new PCA(models);
    pcaModel = new PCA(deformationFields);
    pcaModel->computeEig();

    Eigen::VectorXf coefficients(9);
    coefficients.setZero();
    std::cout << "coefficients.size(): " << coefficients.size() << "\n";
    std::cout << "coefficients[0]: " << coefficients[0] << "\n";
    std::cout << "coefficients[1]: " << coefficients[1] << "\n";
    coefficients[0] = 1.0f;
    std::cout << "coefficients[0]: " << coefficients[0] << "\n";

    Eigen::VectorXf projection = pcaModel->getProjection(coefficients);
    std::cout << "projection.size(): " << projection.size() << "\n";
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
    std::cout << "m.points.size(): " << m.points.size() << "\n";
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

    std::cout << "projection.sum(): " << projection.sum() << "\n";
    Model meanModel = createMeanModel();

    Eigen::VectorXf meanVector = MeshToVectorXf(meanModel.mesh);

    Eigen::VectorXf sample = meanVector + projection;

    Mesh sampledMesh = vectorXfToMesh(sample);
    sampledMesh.textureCoords = models[0].mesh.textureCoords;
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
        Point<glm::vec3> point = m.getPointAtIndex(index);

        // save in newLmsJson
        newLmsJson[k]["id"] = id;
        newLmsJson[k]["coordinates"] = {point.position.x, point.position.y, point.position.z};
        k++;
    }
    std::ofstream writeJson(json_path);
    writeJson << std::setw(4) << newLmsJson << std::endl;
    
}

void BuildSSM::savePCAModel(const std::string& model_path) {
    
    const H5std_string FILE_NAME(model_path);
    const H5std_string DATASET_NAME1("/model/mean");
    const H5std_string DATASET_NAME2("/model/noiseVariance");
    const H5std_string DATASET_NAME3("/model/pcaBasis");
    const H5std_string DATASET_NAME4("/model/pcaVariance");

    //const H5std_string DATASET_NAME5("/objStructure/verticesCells"); // vertices indices cells
    //const H5std_string DATASET_NAME6("/objStructure/textureCells"); // texture coordinates indices cells
    //const H5std_string DATASET_NAME7("/objStructure/textureCoordinates");

    Model mean = createMeanModel();
    Eigen::VectorXf meanVector = MeshToVectorXf(mean.mesh);
    Eigen::MatrixXf eigenVectors = pcaModel->getEigenVectors();
    Eigen::VectorXf eigenValues = pcaModel->getEigenValues();

    const int DIM_MEAN = 3*10475;
    const int DIM_NOISE_VARIANCE = 1;
    const int rowsPCA = 31425;
    const int colsPCA = 9;

    float mean_data[DIM_MEAN];
    float noiseVariance[DIM_NOISE_VARIANCE];
    float pcaBasis[rowsPCA][colsPCA];
    float pcaVariance[colsPCA];
    

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
            mean_data[i] = meanVector[i];
        }
        noiseVariance[0] = 0.0;

        for (int i = 0; i < rowsPCA; i++) {
            for (int j = 0; j < colsPCA; j++) {
                pcaBasis[i][j] = eigenVectors(i,j);
            }
        }

        for (int i = 0; i < colsPCA; i++) {
            pcaVariance[i] = eigenValues[i];
        }

        // 4. Create dataspace for mean
        hsize_t dimsMean[1]; // dataset dimensions
        dimsMean[0]              = DIM_MEAN;
        DataSpace *dataspace = new DataSpace(1, dimsMean);
        // 5. Create the dataset in group
        DataSet *dataset = new DataSet(file.createDataSet(DATASET_NAME1, PredType::NATIVE_FLOAT, *dataspace));
        // Write the data to the dataset using default memory space, file
        // space, and transfer properties.
        dataset->write(mean_data, PredType::NATIVE_FLOAT);
        // Close the current dataset and data space.
        delete dataset;
        delete dataspace;

        // noiseVariance
        hsize_t dimsNoiseVariance[1];
        dimsNoiseVariance[0]   = 1;
        dataspace = new DataSpace(1, dimsNoiseVariance);
        dataset = new DataSet(groupModel.createDataSet(DATASET_NAME2, PredType::NATIVE_FLOAT, *dataspace));
        dataset->write(noiseVariance, PredType::NATIVE_FLOAT);
        delete dataset;
        delete dataspace;

        // pcaBasis
        hsize_t dimsPCABasis[2];
        dimsPCABasis[0]   = rowsPCA;
        dimsPCABasis[1]   = colsPCA;
        dataspace = new DataSpace(2, dimsPCABasis);
        dataset = new DataSet(groupModel.createDataSet(DATASET_NAME3, PredType::NATIVE_FLOAT, *dataspace));
        dataset->write(pcaBasis, PredType::NATIVE_FLOAT);
        delete dataset;
        delete dataspace;

        // pcaVariance
        hsize_t dimsPCAVariance[1];
        dimsPCAVariance[0]   = colsPCA;
        dataspace = new DataSpace(1, dimsPCAVariance);
        dataset = new DataSet(groupModel.createDataSet(DATASET_NAME4, PredType::NATIVE_FLOAT, *dataspace));
        dataset->write(pcaVariance, PredType::NATIVE_FLOAT);
        delete dataset;
        delete dataspace;


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
    ObjLoader::loadObj(reference_obj_path, referenceMesh.points, referenceMesh.pointIds, referenceMesh.triangleCells, referenceMesh.normals, referenceMesh.textureCoords);


    // 2. read and save mean mesh
    // 3. read and save eigen vectors
    // 4. read and save eigen values
    

    const H5std_string FILE_NAME(model_path);
    const H5std_string MEAN_DATASET_NAME("/model/mean");
    const H5std_string PCA_BASIS_DATASET_NAME("/model/pcaBasis");
    const int          DIM0 = 31425; // dataset dimensions
    const int          DIM1 = 9;

    Eigen::VectorXf meanVector(DIM0);

    float mean_data[DIM0];
    float eigenVectors[DIM0][DIM1];
    pcaBasis = Eigen::MatrixXf(DIM0, DIM1);

    try {
        // Turn off the auto-printing when failure occurs so that we can
        // handle the errors appropriately
        Exception::dontPrint();

        // Open an existing file and dataset.
        H5File  file(FILE_NAME, H5F_ACC_RDWR);
        DataSet dataset = file.openDataSet(MEAN_DATASET_NAME);
    
        // Write the data to the dataset using default memory space, file
        // space, and transfer properties.
        dataset.read(mean_data, PredType::NATIVE_FLOAT);

        // read mean data
        for (int i = 0; i < DIM0; i++) {
            meanVector[i] = mean_data[i];
        }
        meanMesh = vectorXfToMesh_w_reference(meanVector);

        dataset = file.openDataSet(PCA_BASIS_DATASET_NAME);
        dataset.read(eigenVectors, PredType::NATIVE_FLOAT);

        for (int i = 0; i < DIM0; i++) {
            for (int j = 0; j < DIM1; j++) {
                pcaBasis(i,j) = eigenVectors[i][j];
            }
        }

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
    Eigen::VectorXf projection = this->pcaBasis * coefficients;

    std::cout << "projection.sum(): " << projection.sum() << "\n";
    
    Eigen::VectorXf meanVector = MeshToVectorXf(meanMesh);
    std::cout << "meanVector done\n";
    std::cout << "meanVector.size(): " << meanVector.size() << ", " << "projection.size(): " << projection.size() << "\n";
    Eigen::VectorXf sample = meanVector + projection;
    std::cout << "sample done\n";
    Mesh sampledMesh = vectorXfToMesh_w_reference(sample);
    sampledMesh.textureCoords = referenceMesh.textureCoords;
    sampledMesh.pointIds = referenceMesh.pointIds;
    sampledMesh.computeNormals();
    return sampledMesh;
}