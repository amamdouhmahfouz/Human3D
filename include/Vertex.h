#ifndef VERTEX_H
#define VERTEX_H

#include <glm/glm.hpp>
#include <iostream>

using namespace std;

class Vertex {
public: 
    glm::vec3 position;         // v
    glm::vec3 normal;           // vn
    glm::vec2 texture_coords;   // vt
    glm::vec3 tangent;
    glm::vec3 bitangent;
};



#endif // VERTEX_H