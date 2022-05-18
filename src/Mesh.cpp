#include "Mesh.h"

Mesh::Mesh() {
  
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) {
    this->vertices = vertices;
    this->indices = indices;
}

Mesh::Mesh(std::vector<Point<glm::vec3>> points) {
    this->points = points;
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<glm::vec3> faces) {
    this->vertices = vertices;
    this->indices = indices;
    this->faces = faces;
}

Mesh::~Mesh() {

}

void Mesh::setMesh(std::vector<Point<glm::vec3>> points, std::vector<glm::vec3> faces) {
    this->points = points;
    this->faces = faces;
}

void Mesh::setMesh(std::vector<Point<glm::vec3>> points) {
    this->points = points;
}


// glm::vec3 Mesh::getClosestPoint(glm::vec3 point) {

// }

glm::vec3 Mesh::getCenterOfMass() const {
    glm::vec3 center(0.0f,0.0f,0.0f);
    
    for (const auto& point : points) {
        center += point.position;
    }
    
    center /= float(points.size());
    return center;
}

float Mesh::getScale() const {
    float scale = 0.0;
    for (int i = 0; i < points.size(); i++) {
        scale += (points[i].position.x*points[i].position.x +points[i].position.y*points[i].position.y + points[i].position.z*points[i].position.z);
    }
    scale = sqrt(scale/float(points.size()));
    return scale;
}

void Mesh::deTranslate() {
    glm::vec3 center = getCenterOfMass();

    for (int i = 0; i < points.size(); i++) {
        points[i].position -= center;
    }
}

void Mesh::deScale() {
    float scale = getScale();
    
    for (int i = 0; i < points.size(); i++) {
        points[i].position /= scale;
    }
}

Point<glm::vec3> Mesh::getPointAtIndex(unsigned int pointIndex) {
    for (int i = 0; i < points.size(); i++) 
        if (points[i].index == pointIndex)
            return points[i];
}