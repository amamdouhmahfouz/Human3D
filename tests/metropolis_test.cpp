#include <iostream>
#include "MetropolisHastings.h"


int main() {

    std::cout << "metropolis included successfully\n";

    BuildSSM ssm;
    ssm.loadPCAModel("/Users/abdelrahmanabdelghany/Documents/college/semester10/GP/Human3D/tests/pcaModel_male_T.h5",
        "/Users/abdelrahmanabdelghany/Documents/college/semester10/GP/Human3D/tests/referenceObj.obj");
    ssm.readIdsIndicesLandmarks("/Users/abdelrahmanabdelghany/Documents/college/semester10/GP/Human3D/tests/ids_index.json");

    // observedParams will correspond to this obj: meshTestSampledFromSavedModel.obj
    BodyParameters observedParams;
    observedParams.armSpan = 4.16168;
    observedParams.shoulderWidth = 13.1;
    observedParams.height = 23.9259;

    Eigen::VectorXf coefficients(10);
    coefficients.setZero();
    MetropolisHastings metropolis(&ssm, coefficients, observedParams, 0.05, 0.1, 0.1);

    metropolis.run(10);

    return 0;
}