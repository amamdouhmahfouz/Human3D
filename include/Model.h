#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>
#include <fstream>
#include <json/json.hpp>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Vertex.h"
#include "Mesh.h"

class Model {
private: 
    std::string directory;
    std::string name;
    std::string gender;
    float height;
    float weight;

    std::string getNameFromFile(const std::string& filepath, const std::string& file_extension);
public:
    std::vector<Mesh> meshes;

    Model(const std::string &filename);
    ~Model();

    void loadModel(std::string const &filename);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    void setMetadata(const std::string& filename);

    float getHeight() const;
    float getWeight() const;
    std::string getGender() const;
    std::string getName() const;
};



#endif // MODEL_H