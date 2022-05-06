#include "BuildSSM.h"


BuildSSM::BuildSSM(const std::string& dir_path) {
    std::cout << "directory: " << dir_path << "\n";
    add_files_to_vec(dir_path, "json", metadata_files);
    add_files_to_vec(dir_path, "stl", meshes_files);


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
    std::string mesh_filename = getNameFromFile(mesh_file_path, ".stl");

    for (const auto& metadata_file: metadata_files ) {
        std::string metadata_filename = getNameFromFile(metadata_file, ".json");
        if (mesh_filename == metadata_filename) {
            return metadata_file;
        }
    }

    return "";
}

void BuildSSM::createModelsFromFiles() {
    for (int i = 0; i < meshes_files.size(); i++) {
        Model m(meshes_files[i]);
        models.push_back(m);

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
    for(int i = 1; i < models.size(); i++) {
        restOfModels.push_back(models[i]);
    }

    for (int i = 0; i < restOfModels.size(); i++) {
        std::vector<glm::vec3> d;
        for (int j = 0; j < restOfModels[i].meshes[0].vertices.size(); j++) {
            d.push_back(restOfModels[i].meshes[0].vertices[j].position - referenceModel->meshes[0].vertices[j].position);
        }
        deformationFields.push_back(d);
    }

}