#include "Vertex.h"
#include "Model.h"
#include <iostream> 


std::ostream& operator<<(std::ostream& os, const Vertex& vertex)
{
    os << "position: (" << vertex.position.x<< ", "<<vertex.position.y<<", "<<vertex.position.z<<")\n";
    
    return os;
}

void printMesh(Mesh m) {
    for (int i = 0; i < m.vertices.size(); i++) {
        //std::cout << "vertices #" << i << ": " << m.vertices[i].position.x << "\n";
        std::cout << m.indices[i] << "\n";
    }
    std::cout << "size: " << m.indices.size() << "\n";
}

int main() {

    std::cout << "[Model] included successfully\n";

    Model ourModel("/Users/abdelrahmanabdelghany/Documents/college/semester10/GP/Human3D/tests/01.obj");
    std::cout << "number of meshes: " << ourModel.meshes.size() << "\n";
    std::cout << "[Model] loaded .obj file successfully\n";

    printMesh(ourModel.meshes[0]);

    return 0;
}
