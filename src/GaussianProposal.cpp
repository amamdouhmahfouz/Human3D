#include "GaussianProposal.h"

GaussianProposal::GaussianProposal(float standardDeviation) : ProposalDistribution(standardDeviation) {
    // create a tmp distribution with standardDeviation
    std::normal_distribution<float> distributionTmp{0.0, standardDeviation};
    // update the standardDeviation
    distribution.param(distributionTmp.param());
}

GaussianProposal::~GaussianProposal() {

}

Eigen::VectorXf GaussianProposal::propose(Eigen::VectorXf params) {
    Eigen::VectorXf proposedParams(params.size());
    
    for (int i = 0; i < params.size(); i++) {
        float r = distribution(generator);
        //std::cout << "*********************** r = " << r << "\n";
        proposedParams[i] = params[i] + standardDeviation *  r;
        std::cout << "params[i] = " << params[i] << ",   proposedParams[i] = " <<  proposedParams[i] << "\n";
    }

    return proposedParams;
}

float GaussianProposal::evaluateLogTransitionProbability(Eigen::VectorXf fromParams, Eigen::VectorXf toParams) {
    
    float sumLogValues = 0.0;

    if (fromParams.size() != toParams.size()) {
        std::cerr << "[ERROR]::fromParams and toParams are not the same size!\n";
        return -1.0;
    }

    float denom = -0.5 * log(2 * M_PI) - log(standardDeviation);
    // x: to
    // mean: from
    for (int i = 0; i < fromParams.size(); i++) {
        sumLogValues += denom - (0.5 * (toParams[i] - fromParams[i]) * (toParams[i] - fromParams[i])) / (standardDeviation * standardDeviation);
    }

    return sumLogValues;
}