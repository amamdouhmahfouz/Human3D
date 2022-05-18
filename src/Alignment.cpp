#include "Alignment.h"

void Alignment::RotationAlignment(Model &from, Model to) {

    glm::vec3 fromVec = from.mesh.getPointAtIndex(0).position;
    glm::vec3 toVec = to.mesh.getPointAtIndex(0).position;

    glm::vec3 v = glm::cross(toVec, fromVec);
    if (v.x == 0.0 && v.y == 0.0 && v.z == 0.0) 
        return;

    // source: stackoverflow
    float angle = acos(glm::dot(toVec, fromVec) / (glm::length(toVec) * glm::length(fromVec)));

    glm::mat4 rotmat = glm::rotate(angle, v);

    // special cases lead to NaN values in the rotation matrix
    if (glm::any(glm::isnan(rotmat * glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)))) {
        if (angle < 0.1f) {
            rotmat = glm::mat4(1.0f);
        }
        else if (angle > 3.1f) {
            // rotate about any perpendicular vector
            rotmat = glm::rotate(angle, glm::cross(fromVec,
                                        glm::vec3(fromVec.y, fromVec.z, fromVec.x)));
        }
        else {
            assert(false);
        }
    }

    // rotate all the points in the "from" mesh by the calculated rotation matrix
    for (int i = 0; i < from.mesh.points.size(); i++) {
        from.mesh.points[i].position = rotmat * glm::vec4(from.mesh.points[i].position, 1.0f);        
    }

    // rotate all normals in "from" mesh
    for (int i = 0; i < from.mesh.normals.size(); i++) {
        from.mesh.normals[i].position = rotmat * glm::vec4(from.mesh.normals[i].position, 1.0f);
    }

}

float Alignment::procrustesDistance(Model m, Model reference) {

    float sumSquareDistance = 0.0f;
    for (int i = 0; i < m.mesh.points.size(); i++) {
        sumSquareDistance += (pow(reference.mesh.points[i].position.x - m.mesh.points[i].position.x, 2) +
            pow(reference.mesh.points[i].position.y - m.mesh.points[i].position.y, 2) +
            pow(reference.mesh.points[i].position.z - m.mesh.points[i].position.z, 2));
    }

    sumSquareDistance = sqrt(sumSquareDistance);
    return sumSquareDistance;
}
