#include <iostream>
#include "MetropolisHastings.h"


int main() {

    std::cout << "metropolis included successfully\n";

    BuildSSM ssm;
    ssm.loadPCAModel("/Users/abdelrahmanabdelghany/Documents/college/semester10/GP/Human3D/tests/pcaModel_male_630_smpl_T.h5",
        "/Users/abdelrahmanabdelghany/Documents/college/semester10/GP/Human3D/data/smpl/smpl_uv.obj");
    ssm.readIdsIndicesLandmarks("/Users/abdelrahmanabdelghany/Documents/college/semester10/GP/Human3D/data/smpl/ids_index_smpl.json");

    // observedParams will correspond to this obj: meshTestSampledFromSavedModel.obj
    BodyParameters observedParams;
    // observedParams.armSpan = 254.472;
    // observedParams.shoulderWidth = 45.2493;
    // observedParams.height = 248.842;//23.9259;

    // define the ratios in the image
    observedParams.height = 492.0;
    observedParams.armSpanRatio = 497.0/492.0; // 1.01
    observedParams.shoulderWidthRatio = 101.0/492.0; // 0.205
    observedParams.thighWidthRatio = 41.0/492.0; // 0.08333
    observedParams.stomachWidthRatio = 75.0/492.0; // 0.152439  
    observedParams.chestWidthRatio = 97.0/492.0; // 0.19715
    observedParams.legHeightRatio = 240.0/492.0; // 0.4878

    // float actualHeight = 168.0;
    // observedParams.height = 492.0 / actualHeight;
    // observedParams.armSpanRatio = 497.0 / actualHeight; // 1.01
    // observedParams.shoulderWidthRatio = 101.0 / actualHeight; // 0.205
    // observedParams.thighWidthRatio = 41.0 / actualHeight; // 0.08333
    // observedParams.stomachWidthRatio = 75.0 / actualHeight; // 0.152439  
    // observedParams.chestWidthRatio = 97.0 / actualHeight; // 0.19715
    // observedParams.legHeightRatio = 240.0 / actualHeight; // 0.4878

    Eigen::VectorXf coefficients(10);
    coefficients.setZero();
    MetropolisHastings metropolis(&ssm, coefficients, observedParams, 0.34, 0.1, 1.0);

    metropolis.run(5000);

    return 0;
}