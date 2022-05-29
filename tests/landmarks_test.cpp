#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <json/json.hpp>
#include <typeinfo>
#include "ObjLoader.h"
#include <map>
#include <utility>


unsigned int getNearestIndex(std::vector<Point<glm::vec3>> points, glm::vec3 coordinate) {
    float error = 9999999.0f;
    unsigned int index = -1;
    for (int i = 0; i < points.size(); i++) {
        glm::vec3 point = points[i].position;
        float e = glm::distance(point, coordinate);
        if (e < error) {
            error = e;
            //std::cout << "error in loop: " << error << "\n";
            index = i;
        }
    }
    return index;
}


int main() {

    std::string filename = "/Users/abdelrahmanabdelghany/Documents/college/semester10/GP/Human3D/tests/landmarks_v1.json";
    std::ifstream json_file(filename);

    nlohmann::json json;
    json_file >> json;
    json_file.close();

    // reference mesh
    std::string obj_file = "/Users/abdelrahmanabdelghany/Documents/college/semester10/GP/Human3D/tests/referenceObj.obj";
    std::vector<Point<glm::vec3>> points;
    std::vector<unsigned int> pointIds; 
    std::vector<TriangleCell> faces;
    std::vector<Point<glm::vec3>> normals;
    std::vector<Point<glm::vec2>> texCoords;

    ObjLoader::loadObj(obj_file, points, pointIds, faces, normals, texCoords);

    int k = 0;
    nlohmann::json ids_index_json;
    //std::map<std::string, unsigned int> ids_index;
    std::vector<std::string> ids;
    // iterate the array
    for (nlohmann::json::iterator it = json.begin(); it != json.end(); ++it) {
        std::cout << *it << '\n';
        //std::cout << (*it)["id"] << "\n";
        float x = (*it)["coordinates"][0];
        float y = (*it)["coordinates"][1];
        float z = (*it)["coordinates"][2];

        glm::vec3 coordinate = glm::vec3(x, y, z);
        unsigned int index = getNearestIndex(points, coordinate);
        std::cout << "index: " << index << "\n";
        ids_index_json[k]["id"] = (*it)["id"];
        ids_index_json[k]["index"] = index;
        k++;
    }

    std::ofstream writeJson("/Users/abdelrahmanabdelghany/Documents/college/semester10/GP/Human3D/tests/ids_index.json");
    writeJson << std::setw(4) << ids_index_json << std::endl;

    std::cout << "json.size(): " << json.size() << "\n"; 
    std::cout << "id: " << json[1]["id"] << "\n";
    float x = json[1]["coordinates"][0];
    float y = json[1]["coordinates"][1];
    float z = json[1]["coordinates"][2];
    std::cout << "coordinates: " <<  x + 100.0 << "\n";

    glm::vec3 nosetip = glm::vec3(x,y,z);

    float error = 999999.0f;
    unsigned int index = -1;
    for (int i = 0; i < points.size(); i++) {
        glm::vec3 point = points[i].position;
        float e = glm::distance(point, nosetip);
        if (e < error) {
            error = e;
            std::cout << "error in loop: " << error << "\n";
            index = i;
        }
    }

    std::cout << "error: " << error << "\n";
    std::cout << "index: " << index << "\n";
    std::cout << "point: (" << points[index].position.x << ", " << points[index].position.y << ", " << points[index].position.z << ")\n";
    std::cout << "done landmarks test.\n";
    return 0;
}