#include "ObjLoader.h"


int main() {

    std::cout << "[ObjLoader] imported successfully\n";
    std::string obj_path = "/Users/abdelrahmanabdelghany/Documents/college/semester10/GP/Human3D/tests/cube2.obj";
    std::vector<Point<glm::vec3>> points;
    std::vector<unsigned int> pointIds; 
    std::vector<TriangleCell> faces;
    std::vector<Point<glm::vec3>> normals;

    ObjLoader::loadObj(obj_path, &points, &pointIds, &faces, &normals);
    return 0;
}