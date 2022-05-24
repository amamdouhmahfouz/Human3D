#include <glm/glm.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include "Point.h"
#include "TriangleCell.h"


class ObjLoader {
public:  
    static void loadObj(std::string obj_path, std::vector<Point<glm::vec3>>& points, std::vector<unsigned int>& pointIds, std::vector<TriangleCell>& faces, std::vector<Point<glm::vec3>>& normals, std::vector<Point<glm::vec2>>& textureCoords);
    static void saveObj(std::string obj_path, std::vector<Point<glm::vec3>> points, std::vector<unsigned int> pointIds, std::vector<TriangleCell> faces, std::vector<Point<glm::vec3>> normals, std::vector<Point<glm::vec2>> textureCoords);

    template <typename T>
    static std::string convertToString(const T& num) { 
        std::ostringstream ss; 
        ss << num; 
        return ss.str(); 
    } 

private:
    static int x;
};