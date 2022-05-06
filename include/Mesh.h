#ifndef MESH_H 
#define MESH_H

#include <vector>
#include "Vertex.h"

class Mesh {
private:  

public:  
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
    ~Mesh();
};

#endif //MESH_H

