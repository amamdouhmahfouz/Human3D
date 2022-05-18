#include "BuildSSM.h"


BuildSSM::BuildSSM(const std::string& dir_path) {
    std::cout << "directory: " << dir_path << "\n";
    add_files_to_vec(dir_path, "json", metadata_files);
    add_files_to_vec(dir_path, "obj", meshes_files);


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
    for(int i = 1; i < models.size(); i++) {
        restOfModels.push_back(models[i]);
    }

    for (int i = 0; i < restOfModels.size(); i++) {
        // std::vector<glm::vec3> d;
        std::vector<DeformationField> d;
        for (int j = 0; j < restOfModels[i].mesh.points.size(); j++) {
            //std::cout << "restOfModels[i].mesh.points["<<j << "].index: " << restOfModels[i].mesh.points[j].index << "\n";
            //d.push_back(restOfModels[i].mesh.points[j].position - referenceModel->mesh.points[j].position);
            DeformationField df;
            df.vectorField = restOfModels[i].mesh.points[j].position - referenceModel->mesh.points[j].position;
            df.index = restOfModels[i].mesh.points[j].index;
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
            meanPoints[i].position += models[i].mesh.points[j].position;
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

    meanMesh.setMesh(meanPoints);

    Model meanModel(meanMesh, meanWeight, meanHeight);
    return meanModel;
}

void BuildSSM::computeGPA(Model meanModel) {

    // 1. deTranslate all models
    // 2. deScale all models
    for (int i = 0; i < models.size(); i++) {
        models[i].mesh.deTranslate();
        models[i].mesh.deScale();
    }
    // 3. choose arbitray reference model
    Model referenceMeanModel = models[0];
    // 4. align all models to the reference model
    for (int i = 0; i < models.size(); i++) {
        Alignment::RotationAlignment(models[i],referenceMeanModel);
    }

    // 5. compute mean shape of ALL the aligned+reference shapes
    // 6. calculate procrustes distance between mean shape and referece
    // 7. if the procrustes distance is above a certain threshold, set 
    //    the reference to mean shape and go to step 4

    // 1. get reference mean model
    referenceMeanModel = createMeanModel();
    // 2. allign all models to the reference mean model
    // 3. calculate procrustes distance
    // 4. loop again if error is greater than a threshold

}