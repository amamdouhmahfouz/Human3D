#ifndef BUILDSSM_H
#define BUILDSSM_H

#include <vector>
#include <string>
#include <iostream>
#include <filesystem>
#include <glm/glm.hpp>
#include "Model.h"
#include "DeformationField.h"
#include "Alignment.h"
#include "PCA.h"
#include <json/json.hpp>
#include <fstream>
#include "H5Cpp.h"
using namespace H5;


class BuildSSM {
private:
    // metadate files include height, weight, gender of each mesh
    std::vector<std::string> metadata_files;
    std::vector<std::string> meshes_files;

    Model* referenceModel;
    std::vector<Model> restOfModels; // does not include the referenceMesh

    nlohmann::json idsIndicesJson;

    float averageHeight;
    float averageWeight;

    Eigen::MatrixXf pcaBasis;
    Eigen::VectorXf eigenValues;
    Mesh meanMesh;
    Mesh referenceMesh;

    void add_files_to_vec(const std::string& dir, const std::string& delim, std::vector<std::string>& v);
    std::string getCorrespondingMetadataFile(const std::string& mesh_file_path);
    std::string getNameFromFile(const std::string& filepath, const std::string& file_extension);
public:
    std::vector<Model> models;
    PCA* pcaModel;
    

    // std::vector<std::vector<glm::vec3> > deformationFields;
    std::vector<std::vector<DeformationField> > deformationFields;
    void createModelsFromFiles();
    // directory contains: 01.obj, 01.json, 02.obj, 02.json, ...
    BuildSSM(const std::string& dir_path);
    BuildSSM();
    ~BuildSSM();

    // average height and weight
    void createStatsFromMetadata();
    void createDeformationFields();
    
    Model createMeanModel();
    void computeGPA();
    void buildCovarianceMatrix();

    // subtracts the mean model out of all models
    void deMeanModels();

    void createPCA();

    Mesh vectorXfToMesh(Eigen::VectorXf vec);
    Mesh vectorXfToMesh_w_reference(Eigen::VectorXf vec);
    Eigen::VectorXf MeshToVectorXf(Mesh m);

    Mesh sampleSSM(Eigen::VectorXf coefficients);

    void readIdsIndicesLandmarks(const std::string& json_path);
    void saveLandmarks(std::string json_path, Mesh m);
    void savePCAModel(const std::string& model_path);
    void loadPCAModel(const std::string& model_path, const std::string& reference_obj_path);
    Mesh instance(Eigen::VectorXf coefficients); // used when loading a model
};


#endif // BUILDSSM_H
