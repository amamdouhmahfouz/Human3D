#include <iostream>
#include "MetropolisHastings.h"

/**
 * @brief fits the pca shape model built on input body proportions of 2D human image
 * 
 * @usage ./fitting ../models/pcaModel_male_630_smplx_v2_T.h5 ../data/referenceObj.obj ../data/ids_index_v2.json ../data/inputParamsjson ../output/finalMesh7.obj ../output/finalLms7.json
 * 
 * @param argv[1] - pca shape model h5 file path
 * @param argv[2] - reference mesh obj file path
 * @param argv[3] - ids indices json file path
 * @param argv[4] - input json file with proportions
 * @param argv[5] - output fitted mesh obj file path
 * @param argv[6] - output landmarks json file path
 * 
 */
int main(int argc, char *argv[]) {

    std::cout << "[Fitting]::starting fitting...\n";

    BuildSSM ssm;

    ssm.loadPCAModel(std::string(argv[1]),
        std::string(argv[2]));
    std::cout << "[Fitting]::loaded model successfully\n";

    ssm.readIdsIndicesLandmarks(std::string(argv[3]));
    std::cout << "[Fitting]::reading ids_index json successfully\n";

    BodyParameters observedParams;

    // define the ratios in the image
    // observedParams.height = 492.0;
    // observedParams.armSpanRatio = 497.0/492.0; // 1.01
    // observedParams.shoulderWidthRatio = 101.0/492.0; // 0.205
    // observedParams.thighWidthRatio = 41.0/492.0; // 0.08333
    // observedParams.stomachWidthRatio = 75.0/492.0; // 0.152439  
    // observedParams.chestWidthRatio = 97.0/492.0; // 0.19715
    // observedParams.legHeightRatio = 240.0/492.0; // 0.4878
    // //observedParams.neckLengthRatio = 26.0/492.0; // 0.046747
    // observedParams.headWidthRatio = 44.0/492.0; // 0.08943
    //observedParams.headIntoLengthRatio = 66.0/492.0;


    // observedParams.height = 868.0;
    // observedParams.armSpanRatio = 758.0/868.0; // 0.87327
    // //observedParams.shoulderWidthRatio = 101.0/868.0; // 0.205
    // observedParams.thighRightWidthRatio = 83.0/868.0; // 0.0956
    // observedParams.stomachWidthRatio = 173.0/868.0; // 0.199308  
    // observedParams.chestWidthRatio = 185.0/868.0; // 0.21313
    // observedParams.legHeightRatio = 456.0/868.0; // 0.525345
    // observedParams.headWidthRatio = 82.0/868.0; // 0.0.09447

    observedParams.height = 492.0;
    observedParams.armSpanRatio = 497.0/492.0; // 0.87327
    //observedParams.shoulderWidthRatio = 101.0/868.0; // 0.205
    observedParams.thighRightWidthRatio = 41.0/492.0; // 0.0956
    observedParams.stomachWidthRatio = 75.0/492.0; // 0.199308  
    observedParams.chestWidthRatio = 97.0/492.0; // 0.21313
    observedParams.legHeightRatio = 240.0/492.0; // 0.525345
    observedParams.headWidthRatio = 44.0/492.0; // 0.0.09447
    observedParams.neckLengthRatio = 0.030;


    Eigen::VectorXf coefficients(7);
    coefficients.setZero();
    //MetropolisHastings metropolis(&ssm, coefficients, observedParams, 0.34, 0.2, 1.0);
    //MetropolisHastings metropolis(&ssm, coefficients, observedParams, 0.165, 1.0, 1.0); //u(0,1)
    MetropolisHastings metropolis(&ssm, std::string(argv[4]), coefficients, 0.066f, 0.01f, 1.0f);
    Model fittedModel = metropolis.run(7000);

    fittedModel.saveMesh(std::string(argv[5]));
    fittedModel.saveLandmarks(ssm.getIdsIndicesJson(), std::string(argv[6]));


    std::cout << "[Fitting]::done fitting\n";
    return 0;
}