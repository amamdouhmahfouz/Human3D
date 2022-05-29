#include <iostream>
#include "BuildSSM.h"
//#include <glm/ext.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
void printVec(glm::vec3 v) {
    std::cout << "vec: (" << v.x << ", " << v.y << ", " << v.z << ")\n";
}

float angleBetween(
 glm::vec3 a,
 glm::vec3 b,
 glm::vec3 origin
){
 glm::vec3 da=glm::normalize(a-origin);
 glm::vec3 db=glm::normalize(b-origin);
 return glm::degrees(glm::acos(glm::dot(da, db)));
}

int main(int argc, char *argv[]) {

    std::cout << "[BuildSSM] included successfully\n";

    std::string male_dir = "/Users/abdelrahmanabdelghany/Documents/college/semester10/GP/Human3D/data/male";
    BuildSSM ssm(male_dir);
    std::size_t p = male_dir.find_last_of('/');
    std::cout << "p: " << p << "\n";
    
    ssm.createModelsFromFiles();
    std::cout << "[BuildSSM] created models from files\n";

    ssm.createDeformationFields();
    std::cout << "ssm.deformationFields.size(): " << ssm.deformationFields.size() << "\n";
    for (int i = 0; i < ssm.deformationFields.size(); i++)
        std::cout << "ssm.deformationFields[i].size(): " << ssm.deformationFields[i].size() << "\n";

    Model meanModel = ssm.createMeanModel();
    std::cout << "mean height: " << meanModel.getHeight() << ", mean weight: " << meanModel.getWeight() << "\n";
    
    std::vector<int> v1{1,2,3};
    std::vector<int> v2 = v1;
    v2[0] = 900;
    std::cout << "v1[0]: " << v1[0] << ", v2[0]: " << v2[0] << "\n";

    glm::vec3 c1 = ssm.models[0].mesh.getCenterOfMass();
    std::cout << "center of mass before deTranslate: ";
    printVec(c1);
    ssm.models[0].mesh.deTranslate();
    std::cout << "center of mass after deTranslate(should be almost zeros): ";
    printVec(ssm.models[0].mesh.getCenterOfMass());

    float s1 = ssm.models[0].mesh.getScale();
    std::cout << "scale before deScale: " << s1 << "\n";
    ssm.models[0].mesh.deScale();
    std::cout << "scale after deScale(should be 1): " << ssm.models[0].mesh.getScale() << "\n";
    // for (int i = 0; i < meanModel.mesh.points.size(); i++) {
    //     std::cout << "pointId: " << meanModel.mesh.points[i].index << "\n";
    // }
    
    std::cout << "angle between: " << angleBetween(glm::vec3(1,0,0),glm::vec3(0,1,0), glm::vec3(0,0,0)) << "\n";

    // const glm::vec3 a = glm::vec3(1.0f,1.0f,1.0f);
    // const glm::vec3 b = glm::vec3(0.3033688f,1.0,-1.3812919f); // in my case (1, 0, 0)
    // glm::vec3 v = glm::cross(b, a);
    // float angle = acos(glm::dot(b, a) / (glm::length(b) * glm::length(a)));
    // glm::mat4 rotmat = glm::rotate(angle, v);


    glm::vec3 from = glm::vec3(-1.0f,-1.0f,1.0f);
    //glm::vec3 to = glm::vec3(0.3033688f,1.0,-1.3812919f);
    //glm::vec3 to = glm::vec3(0.303368837f,1.0f,-1.38129191286f);
    glm::vec3 to = glm::vec3(1.38129191286f,-1.0f,0.30336883733f);
    

    glm::vec3 v = glm::cross(to, from);
    //glm::mat4 m = glm::mat4( 1.0 );
    float angle = acos(glm::dot(to, from) / (glm::length(to) * glm::length(from)));
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
            rotmat = glm::rotate(angle, glm::cross(from,
                                        glm::vec3(from.y, from.z, from.x)));
        }
        else {
            assert(false);
        }
    }

    std::cout << "angle between from and to: " << angleBetween(from, to, glm::vec3(0,0,0)) << "\n";

    std::cout << "\nrotmat:\n";
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            std::cout << rotmat[i][j] << " ";
        }
        std::cout << "\n";
    }

    glm::mat4 transform = glm::eulerAngleYXZ(-2.14f, 0.0f, 0.0f);
    
    
    std::cout << "\ntransform:\n";
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            std::cout << transform[i][j] << " ";
        }
        std::cout << "\n";
    }

    glm::vec3 aa  = rotmat * glm::vec4(from, 1.0f);
    std::cout << "aa.position: (" << aa.x << ", " << aa.y << ", " << aa.z << ")\n"; 
    
    //Mesh sampledMesh = ssm.createMeanModel().mesh;//ssm.sampleSSM(coefficients);


    ssm.computeGPA();

    ssm.models[8].mesh.computeNormals();
    // test saving a mesh after computing generalized procrustes alignment
    ObjLoader::saveObj("/Users/abdelrahmanabdelghany/Documents/college/semester10/GP/Human3D/tests/meshTest1.obj",
     ssm.models[8].mesh.points, ssm.models[8].mesh.pointIds, ssm.models[8].mesh.triangleCells,
     ssm.models[8].mesh.normals, ssm.models[8].mesh.textureCoords);
    

    ssm.createPCA();

    Eigen::VectorXf coefficients(9);
    coefficients.setZero();
    coefficients[atoi(argv[1])] = std::stof(argv[2]);
    std::cout << "argv[1]: " << std::stof(argv[1]) << "\n";
    Mesh sampledMesh =  ssm.sampleSSM(coefficients); //ssm.createMeanModel().mesh ;//
    sampledMesh.computeNormals();



    ObjLoader::saveObj("/Users/abdelrahmanabdelghany/Documents/college/semester10/GP/Human3D/tests/meshTestSampled.obj",
     sampledMesh.points, sampledMesh.pointIds, sampledMesh.triangleCells,
     sampledMesh.normals, sampledMesh.textureCoords);
    // std::string test_str = "/aa/bb/cc.json";
    // std::string delim = ".json";
    // std::size_t pos = test_str.find(delim);
    // std::cout << "pos: " << pos << "\n";
    // if (pos >= 0 && pos < test_str.size()) {
    //     std::string s = test_str.substr(pos);
    //     std::cout << "s: " << s << "\n";
    //     std::cout << "splitted: " << s << std::endl;
    // } else {
    //     std::cout << "nothing found\n";
    // }

    return 0;
}