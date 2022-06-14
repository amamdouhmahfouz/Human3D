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
    Eigen::VectorXf bestCoef = shapeCoefficients;
    int countAccepted = 0;
    int countRejected = 0;
    nlohmann::json idsIndicesJson = ssm->getIdsIndicesJson();

    // uniform distribution
    std::random_device rand_dev;
    unsigned seed = 0;
    std::mt19937 generator(seed);
    std::uniform_real_distribution<> uniform_dist(0, 1);
    //srand( (unsigned)time( NULL ) );

    for (int i = 0; i < iterations; i++) {
        std::cout << "......... iteration #: " << i << " / "<< iterations<<" ............\n";
        // 1.
        Eigen::VectorXf proposalShapeCoeff = proposalDistribution->propose(shapeCoefficients);
        // 2.
        Mesh meshInstance = ssm->instanceNoNormals(shapeCoefficients);
        meshInstance.scale(1000.0);
        Model modelInstance(meshInstance, idsIndicesJson);
        
        BodyParameters calculatedBodyParams = modelInstance.computeBodyRatios();//modelInstance.computeBodyParameters();
        

        float likelihood = GaussianProportionEvaluator::evaluateLogProportions(likelihoodStddev, observedBodyParams, calculatedBodyParams);
        float prior = GaussianPrior::evaluateLogPrior(0.0, priorStddev, shapeCoefficients);
        float posterior = likelihood + prior;
        std::cout << "likelihood: " << likelihood << "\n";
        std::cout << "prior: " << prior << "\n";
        std::cout << "posterior: " << posterior << "\n";

        //std::cout << "proposalShapeCoeff: " << proposalShapeCoeff << "\n";
        // 3. 
        Mesh proposedMesh = ssm->instanceNoNormals(proposalShapeCoeff);
        proposedMesh.scale(1000.0);

        if (i == 0) {
            std::cout << "proposalShapeCoeff: " << proposalShapeCoeff << "\n";
            ObjLoader::saveObj("/Users/abdelrahmanabdelghany/Documents/college/semester10/GP/Human3D/tests/proposedMesh.obj",
                proposedMesh.points, proposedMesh.pointIds, proposedMesh.triangleCells,
                proposedMesh.normals, proposedMesh.textureCoords);
        }

        // 4.
        Model proposedModel(proposedMesh, idsIndicesJson);
        // 5.
        BodyParameters proposedBodyParameters = proposedModel.computeBodyRatios();//proposedModel.computeBodyParameters();

        // 6. 
        float likelihoodProposed = GaussianProportionEvaluator::evaluateLogProportions(likelihoodStddev, observedBodyParams, proposedBodyParameters);
        float priorProposed = GaussianPrior::evaluateLogPrior(0.0, priorStddev, proposalShapeCoeff);
        float posteriorProposed = likelihoodProposed + priorProposed;
        std::cout << "likelihoodProposed: " << likelihoodProposed << "\n";
        std::cout << "priorProposed: " << priorProposed << "\n";
        std::cout << "posteriorProposed: " << posteriorProposed << "\n";
        // 7.
        float transitionProbRatio = proposalDistribution->evaluateLogTransitionProbability(shapeCoefficients, proposalShapeCoeff);

        ///std::cout << "transitionProbRatio: " << transitionProbRatio << "\n";

        float alpha = posteriorProposed - posterior - transitionProbRatio; //min(0, ...) and do not check if alpha > 0 then
        //alpha = std::min(0.0f, alpha);
        
        //std::cout << "alpha: " << alpha << "\n";
        
        //srand( (unsigned)time( NULL ) );
        //float u = (float) rand()/RAND_MAX;
        float u = uniform_dist(generator);
        
        std::cout << "u: " << u << "\n";
        std::cout << "log(u): " << log(u) << "\n";
        //std::cout << "exp(alpha): " << exp(alpha) << "\n";
        std::cout << "transitionProbRatio: " << transitionProbRatio << "\n";
        if (alpha > 0 || exp(alpha) > u) {
        //if (alpha > log(u)) {
            // accept
            std::cout << "accepted\n";
            std::cout << "alpha: " << alpha << "\n";
            countAccepted++;
            shapeCoefficients = proposalShapeCoeff;

            //if (alpha > bestAlpha) {
                bestAlpha = alpha;
                bestCoef = shapeCoefficients;
            //}
          //  int ii;
        //std::cin >> ii;
        } else {
            std::cout << "rejected\n";
            std::cout << "alpha: " << alpha << "\n";
            countRejected++;
            //shapeCoefficients = proposalShapeCoeff;
            //bestCoef = shapeCoefficients;
        }

        
    }

    std::cout << "\n*********** countAccepted: " << countAccepted << "\n";
    std::cout << "*********** countRejected: " << countRejected << "\n";
    Mesh finalMesh = ssm->instance(bestCoef);
    finalMesh.scale(1000.0);
    // ObjLoader::saveObj("/Users/abdelrahmanabdelghany/Documents/college/semester10/GP/Human3D/tests/finalMesh3.obj",
    //      finalMesh.points, finalMesh.pointIds, finalMesh.triangleCells,
    //      finalMesh.normals, finalMesh.textureCoords);
    Model finalModel(finalMesh, idsIndicesJson);
    finalModel.saveMesh("/Users/abdelrahmanabdelghany/Documents/college/semester10/GP/Human3D/tests/finalMesh6.obj");
    finalModel.saveLandmarks(idsIndicesJson, "/Users/abdelrahmanabdelghany/Documents/college/semester10/GP/Human3D/tests/finalMesh6_landmarks.json");

    BodyParameters finalBodyParameters = finalModel.computeBodyRatios();//finalModel.computeBodyParameters();
    std::cout << "finalBodyParameters: \n";
    std::cout << "finalBodyParameters.armSpan; " << finalBodyParameters.armSpan << "\n";
    std::cout << "finalBodyParameters.shoulderWidth; " << finalBodyParameters.shoulderWidth << "\n";
    std::cout << "finalBodyParameters.height; " << finalBodyParameters.height << "\n";

    std::cout << "finalBodyParameters.armSpanRatio; " << finalBodyParameters.armSpanRatio << "\n";
    //std::cout << "finalBodyParameters.shoulderWidthRatio; " << finalBodyParameters.shoulderWidthRatio << "\n";
    std::cout << "finalBodyParameters.stomachWidthRatio; " << finalBodyParameters.stomachWidthRatio << "\n";
    std::cout << "finalBodyParameters.chestWidthRatio; " << finalBodyParameters.chestWidthRatio << "\n";
    std::cout << "finalBodyParameters.legHeightRatio; " << finalBodyParameters.legHeightRatio << "\n";
    std::cout << "finalBodyParameters.neckLengthRatio; " << finalBodyParameters.neckLengthRatio << "\n";
    std::cout << "finalBodyParameters.headWidthRatio; " << finalBodyParameters.headWidthRatio << "\n";
    std::cout << "finalBodyParameters.thighRightWidthRatio; " << finalBodyParameters.thighRightWidthRatio << "\n";
   //std::cout << "finalBodyParameters.headIntoLengthRatio; " << finalBodyParameters.headIntoLengthRatio << "\n";
    //std::cout << "finalBodyParameters.heightRatio; " << finalBodyParameters.heightRatio << "\n";
    



    // std::cout << "*********** countAccepted: " << countAccepted << "\n";
    // std::cout << "*********** countRejected: " << countRejected << "\n";
}