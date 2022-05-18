#ifndef ALIGNMENT_H
#define ALIGNMENT_H

#include "Model.h"
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <cmath>

class Alignment {
public: 
    // aligns the "from" mesh to the "to" mesh, (NOTE: only by rotations for now)
    static void RotationAlignment(Model &from, Model to);
    static float procrustesDistance(Model m, Model reference);
};

#endif // ALIGNMENT_H