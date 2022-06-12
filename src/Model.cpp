#include "Model.h"

Model::Model(const std::string &filename) {
    loadModel(filename);
}

Model::Model(const std::string &filename, std::vector<TriangleCell> faces, std::vector<Point<glm::vec2>> texCoords) {
    ObjLoader::loadBasicObj(filename, mesh.points);
    mesh.triangleCells = faces;
    mesh.textureCoords = texCoords;
    //mesh.computeNormals();
}

Model::Model(Mesh mesh) {
    this->mesh = mesh;
}

Model::Model(Mesh mesh, float weight, float height) {
    this->mesh = mesh;
    this->weight = weight;
    this->height = height;
}

Model::Model(Mesh mesh, nlohmann::json idsIndicesJson) {
    this->mesh = mesh;
    computeLandmarksPositions(idsIndicesJson);
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

void Model::computeLandmarksPositions(nlohmann::json idsIndicesJson) {
    idsPoints.clear();

    for (nlohmann::json::iterator it = idsIndicesJson.begin(); it != idsIndicesJson.end(); ++it) {
        // get id and index from idsIndicesJson
        std::string id = (*it)["id"];
        unsigned int index = (*it)["index"];
        
        Point<glm::vec3> point = mesh.getPointAtIndex(index);
        
        idsPoints.insert(std::make_pair(id, point));
    }

    std::map<std::string, Point<glm::vec3>>::iterator it;
    it = idsPoints.find("nosetip");
    
    //std::cout << "idsPoints[nosetip].position.x: " << it->second.position.x << "\n";
}

float Model::computeModelHeight() {

    std::map<std::string, Point<glm::vec3>>::iterator it;
    
    it = idsPoints.find("head.top");
    glm::vec3 head_top = it->second.position;
    //std::cout << "head_top: (" << head_top.x << ", " << head_top.y << ", " << head_top.z << ")\n";

    it = idsPoints.find("foot.left.bottom.center");
    glm::vec3 foot_bottom = it->second.position;
    //std::cout << "foot_bottom: (" << foot_bottom.x << ", " << foot_bottom.y << ", " << foot_bottom.z << ")\n";

    
    float model_height = sqrt(pow(head_top.x - foot_bottom.x, 2) + pow(head_top.y - foot_bottom.y, 2) + pow(head_top.z - foot_bottom.z, 2));    
    //std::cout << "s: " << s << "\n";
    //float modeHeight = glm::distance(head_top, foot_bottom);
    this->modelHeight = model_height;
    return model_height;
}

float Model::computeArmSpan() {
    std::map<std::string, Point<glm::vec3>>::iterator it;
    
    it = idsPoints.find("fist.right.fist");
    glm::vec3 fist_right = it->second.position;

    it = idsPoints.find("fist.left.fist");
    glm::vec3 fist_left = it->second.position;

    float armSpan = sqrt(pow(fist_right.x - fist_left.x, 2) + pow(fist_right.y - fist_left.y, 2) + pow(fist_right.z - fist_left.z, 2));

    return armSpan;
}

float Model::computeShoulderWidth() {
    std::map<std::string, Point<glm::vec3>>::iterator it;
    
    it = idsPoints.find("shoulder.right.top");
    glm::vec3 shoulder_right = it->second.position;

    it = idsPoints.find("shoulder.left.top");
    glm::vec3 shoulder_left = it->second.position;

    float shoulderWidth = sqrt(pow(shoulder_right.x - shoulder_left.x, 2) + pow(shoulder_right.y - shoulder_left.y, 2) + pow(shoulder_right.z - shoulder_left.z, 2));
    //std::cout << "shoulder: " << glm::distance(shoulder_left, shoulder_right) << "\n";
    return shoulderWidth;
}

BodyParameters Model::computeBodyParameters() {
    BodyParameters params;

    params.armSpan = computeArmSpan();
    //std::cout << "****** params.armSpan: " << params.armSpan << "\n";
    params.shoulderWidth = computeShoulderWidth();
    //std::cout << "****** params.shoulderWidth: " << params.shoulderWidth << "\n";
    params.height = computeModelHeight();
    //std::cout << "****** params.height: " << params.height << "\n";

    return params;
}