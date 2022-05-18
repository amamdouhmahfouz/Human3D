#include <glm/glm.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include "Point.h"
#include "TriangleCell.h"


class ObjLoader {
public:  
    static void loadObj(std::string obj_path, std::vector<Point<glm::vec3>>& points, std::vector<unsigned int>& pointIds, std::vector<TriangleCell>& faces, std::vector<Point<glm::vec3>>& normals);
    
    static void saveObj(std::string obj_path, std::vector<Point<glm::vec3>>& points, std::vector<unsigned int>& pointIds, std::vector<TriangleCell>& faces, std::vector<Point<glm::vec3>>& normals);
};