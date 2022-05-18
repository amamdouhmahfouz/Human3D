#include "Alignment.h"

void Alignment::RotationAlignment(Model &from, Model to) {

    

    glm::vec3 fromVec = from.mesh.getPointAtIndex(0).position;
    glm::vec3 toVec = to.mesh.getPointAtIndex(0).position;

    

    glm::vec3 v = glm::cross(toVec, fromVec);
    
    float angle = acos(glm::dot(toVec, fromVec) / (glm::length(toVec) * glm::length(fromVec)));
    std::cout << "angle(degrees): " << glm::degrees(angle) << "\n";
    std::cout << "angle(radians): " << angle << "\n";
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


    std::cout << "\nrotmat:\n";
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            std::cout << rotmat[i][j] << " ";
        }
        std::cout << "\n";
    }


    // rotate all the points in the "to" mesh by the calculated rotation matrix
    for (int i = 0; i < from.mesh.points.size(); i++) {
        from.mesh.points[i].position = rotmat * glm::vec4(from.mesh.points[i].position, 1.0f);
    }


}