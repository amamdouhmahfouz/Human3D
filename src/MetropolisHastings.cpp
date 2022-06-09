#include "MetropolisHastings.h"


MetropolisHastings::MetropolisHastings(BuildSSM* shapeModel, Eigen::VectorXf initCoefficients, BodyParameters observedParams, float proposalStddev, float likelihoodStddev, float priorStddev) {
    ssm = shapeModel;
    shapeCoefficients = initCoefficients;
    observedBodyParams = observedParams;
    proposalDistribution = new GaussianProposal(proposalStddev);
    this->likelihoodStddev = likelihoodStddev;
    this->priorStddev = priorStddev;
}

MetropolisHastings::~MetropolisHastings() {

}

void MetropolisHastings::run(unsigned int iterations) {
    // 1. propose on shape coefficients
    // 2. evaluate posterior
    //      - evaluate likelihood
    //      - evaluate prior
    // 3. instance from ssm with new proposed shape coefficients
    // 4. create Model from instanced mesh
    // 5. get new BodyParameters from the Model 
    // 6. compute posterior of proposed shape coefficients
    // 7. compute transition ratio
    //      - evaluateLogTransitionProbability from GaussianProposal
    // 8.

    // -------------------------------------------------

    float bestAlpha = -99999999.99;
    Eigen::VectorXf bestCoef;
    int countAccepted = 0;
    int countRejected = 0;
    nlohmann::json idsIndicesJson = ssm->getIdsIndicesJson();

    for (int i = 0; i < iterations; i++) {
        std::cout << "......... iteration #" << i << " ............\n";
        // 1.
        Eigen::VectorXf proposalShapeCoeff = proposalDistribution->propose(shapeCoefficients);
        // 2.
        Mesh meshInstance = ssm->instance(shapeCoefficients);
        meshInstance.scale(1000.0);
        Model modelInstance(meshInstance, idsIndicesJson);
        
        BodyParameters calculatedBodyParams = modelInstance.computeBodyParameters();
        
        // std::cout << "calculatedBodyParams: \n";
        // std::cout << "calculatedBodyParams.armSpan; " << calculatedBodyParams.armSpan << "\n";
        // std::cout << "calculatedBodyParams.shoulderWidth; " << calculatedBodyParams.shoulderWidth << "\n";
        // std::cout << "calculatedBodyParams.height; " << calculatedBodyParams.height << "\n";


        float likelihood = GaussianProportionEvaluator::evaluateLogProportions(likelihoodStddev, observedBodyParams, calculatedBodyParams);
        float prior = GaussianPrior::evaluateLogPrior(0.0, priorStddev, shapeCoefficients);
        float posterior = likelihood + prior;
        //std::cout << "posterior: " << posterior << "\n";

        //std::cout << "proposalShapeCoeff: " << proposalShapeCoeff << "\n";
        // 3. 
        Mesh proposedMesh = ssm->instance(proposalShapeCoeff);
        proposedMesh.scale(1000.0);

        // ObjLoader::saveObj("/Users/abdelrahmanabdelghany/Documents/college/semester10/GP/Human3D/tests/meshFromMetropolis.obj",
        //  proposedMesh.points, proposedMesh.pointIds, proposedMesh.triangleCells,
        //  proposedMesh.normals, proposedMesh.textureCoords);
        // 4.
        Model proposedModel(proposedMesh, idsIndicesJson);
        // 5.
        BodyParameters proposedBodyParameters = proposedModel.computeBodyParameters();
        std::cout << "proposedBodyParameters: \n";
        std::cout << "proposedBodyParameters.armSpan; " << proposedBodyParameters.armSpan << "\n";
        std::cout << "proposedBodyParameters.shoulderWidth; " << proposedBodyParameters.shoulderWidth << "\n";
        std::cout << "proposedBodyParameters.height; " << proposedBodyParameters.height << "\n";

        // 6. 
        float likelihoodProposed = GaussianProportionEvaluator::evaluateLogProportions(likelihoodStddev, observedBodyParams, proposedBodyParameters);
        float priorProposed = GaussianPrior::evaluateLogPrior(0.0, priorStddev, proposalShapeCoeff);
        float posteriorProposed = likelihoodProposed + priorProposed;
        // 7.
        float transitionProbRatio = proposalDistribution->evaluateLogTransitionProbability(shapeCoefficients, proposalShapeCoeff);

        ///std::cout << "transitionProbRatio: " << transitionProbRatio << "\n";

        float alpha = posteriorProposed - posterior - transitionProbRatio;
        //std::cout << "alpha: " << alpha << "\n";
        
        srand( (unsigned)time( NULL ) );
        float u = (float) rand()/RAND_MAX;

        
        if (alpha > 0 || u < exp(alpha)) {
            // accept
            std::cout << "accepted\n";
            std::cout << "alpha: " << alpha << "\n";
            countAccepted++;
            shapeCoefficients = proposalShapeCoeff;

            //if (alpha > bestAlpha) {
                bestAlpha = alpha;
                bestCoef = shapeCoefficients;
            //}
        } else {
            std::cout << "rejected\n";
            std::cout << "alpha: " << alpha << "\n";
            countRejected++;
            shapeCoefficients = proposalShapeCoeff;
            bestCoef = shapeCoefficients;
        }
    }

    Mesh finalMesh = ssm->instance(bestCoef);
    finalMesh.scale(1000.0);
    ObjLoader::saveObj("/Users/abdelrahmanabdelghany/Documents/college/semester10/GP/Human3D/tests/finalMesh.obj",
         finalMesh.points, finalMesh.pointIds, finalMesh.triangleCells,
         finalMesh.normals, finalMesh.textureCoords);

    std::cout << "*********** countAccepted: " << countAccepted << "\n";
    std::cout << "*********** countRejected: " << countRejected << "\n";
}