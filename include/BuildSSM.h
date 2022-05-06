#ifndef BUILDSSM_H
#define BUILDSSM_H

#include <vector>
#include <string>
#include <iostream>
#include <filesystem>
#include <glm/glm.hpp>
#include "Model.h"

class BuildSSM {
private:
    // metadate files include height, weight, gender of each mesh
    std::vector<std::string> metadata_files;
    std::vector<std::string> meshes_files;

    std::vector<Model> models;
    Model* referenceModel;
    std::vector<Model> restOfModels; // does not include the referenceMesh

    std::vector<std::vector<glm::vec3> > deformationFields;

    float averageHeight;
    float averageWeight;

    void add_files_to_vec(const std::string& dir, const std::string& delim, std::vector<std::string>& v);
    std::string getCorrespondingMetadataFile(const std::string& mesh_file_path);
    std::string getNameFromFile(const std::string& filepath, const std::string& file_extension);
public:
    void createModelsFromFiles();
    // directory contains: 01.obj, 01.json, 02.obj, 02.json, ...
    BuildSSM(const std::string& dir_path);
    ~BuildSSM();

    // average height and weight
    void createStatsFromMetadata();
    void createDeformationFields();
};


#endif // BUILDSSM_H
