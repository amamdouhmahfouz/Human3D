#include "ObjLoader.h"

void ObjLoader::loadObj(std::string obj_path, std::vector<Point<glm::vec3>>* points, std::vector<unsigned int>* pointIds, std::vector<TriangleCell>* faces, std::vector<Point<glm::vec3>>* normals) {
    
    std::vector<std::string*> _positions;
    std::vector<Point<glm::vec3>*> _points;
    std::vector<TriangleCell*> _faces;
    std::vector<Point<glm::vec3>*> _normals;

    std::fstream file_in(obj_path);
    if (!file_in.is_open()) {
        std::cout << "[ObjLoader::loadModel] cannot load obj file\n";
        return;
    }
    
    char buffer[256];
    while (!file_in.eof()) {
        file_in.getline(buffer, 256);
        _positions.push_back(new std::string(buffer));
    }

    unsigned int vertexIndex = 0;
    unsigned int normalIndex = 0;
    unsigned int triangleIndex = 0;
    for (int i = 0; i < _positions.size(); i++) {
        if (_positions[i]->c_str()[0] == '#') {
            continue;
        } else if (_positions[i]->c_str()[0] == 'v' && _positions[i]->c_str()[1] == ' ') {
            glm::vec3 pos;
            sscanf(_positions[i]->c_str(), "v %f %f %f", &pos.x, &pos.y, &pos.z);

            _points.push_back(new Point(pos, vertexIndex));
            vertexIndex += 1;
        } else if (_positions[i]->c_str()[0] == 'v' && _positions[i]->c_str()[1] == 'n') {
            glm::vec3 normal;
            sscanf(_positions[i]->c_str(), "vn %f %f %f", &normal.x, &normal.y, &normal.z);

            _normals.push_back(new Point(normal, vertexIndex));
            normalIndex += 1;
        } else if (_positions[i]->c_str()[0] == 'f') {
            unsigned int v1,v2,v3, n1, vt1,vt2,vt3;

            sscanf(_positions[i]->c_str(), "f %d//%d//%d %d//%d//%d %d//%d//%d", 
                   &v1,&vt1,&n1, &v2,&vt2,&n1, &v3,&vt3,&n1);
            _faces.push_back(new TriangleCell(v1,v2,v3,n1,triangleIndex));
            triangleIndex += 1;
        }
        
    }

    // cleanup
    for (int i = 0; i < _positions.size(); i++) 
        delete _positions[i];
    for (int i = 0; i < _points.size(); i++) 
        delete _points[i];
    for (int i = 0; i < _normals.size(); i++) 
        delete _normals[i];
    for (int i = 0; i < _faces.size(); i++) 
        delete _faces[i];

}