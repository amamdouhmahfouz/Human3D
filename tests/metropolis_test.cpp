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
    observedParams.armSpan = 254.472;
    observedParams.shoulderWidth = 45.2493;
    observedParams.height = 248.842;//23.9259;

    Eigen::VectorXf coefficients(10);
    coefficients.setZero();
    MetropolisHastings metropolis(&ssm, coefficients, observedParams, 0.12, 0.2, 1.0);

    metropolis.run(100);

    return 0;
}