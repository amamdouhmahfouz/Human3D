#ifndef ALIGNMENT_H
#define ALIGNMENT_H

#include "Model.h"
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

class Alignment {
public: 
    // aligns the "from" mesh to the "to" mesh, (NOTE: only by rotations for now)
    static void RotationAlignment(Model &from, Model to);
};

#endif // ALIGNMENT_H