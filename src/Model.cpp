#include "Model.h"

Model::Model(const std::string &filename) {
    loadModel(filename);
}

Model::Model(Mesh mesh) {
    this->mesh = mesh;
}

Model::Model(Mesh mesh, float weight, float height) {
    this->mesh = mesh;
    this->weight = weight;
    this->height = height;
}

void Model::loadModel(const std::string& filename) {
    ObjLoader::loadObj(filename, mesh.points, mesh.pointIds, mesh.triangleCells, mesh.normals, mesh.textureCoords);
}

Model::~Model() {
    
}

// file_extension example: .json, .obj
std::string Model::getNameFromFile(const std::string& filepath, const std::string& file_extension) {
    std::size_t start_index = filepath.find_last_of('/')+1;
    std::size_t last_index = filepath.find(file_extension);
    std::string name = filepath.substr(start_index, last_index-start_index);
    return name;
}

void Model::setMetadata(const std::string& filename) {
    std::ifstream json_file(filename);
    nlohmann::json json;
    json_file >> json;
    json_file.close();

    this->gender = json["gender"];
    this->height = json["height"];
    this->weight = json["weight"];

    std::string modelName = getNameFromFile(filename, ".json");
    this->name = modelName;
}

std::string Model::getGender() const {
    return this->gender;
}

float Model::getHeight() const {
    return this->height;
}

float Model::getWeight() const {
    return this->weight;
}

std::string Model::getName() const {
    return this->name;
}

glm::vec3 Model::getCenterOfMass() const {
    return mesh.getCenterOfMass();
}

void Model::setMetadata(float height, float weight) {
    this->height = height;
    this->weight = weight;
}