#include "GaussianProposal.h"

GaussianProposal::GaussianProposal(float standardDeviation) : ProposalDistribution(standardDeviation) {
    // create a tmp distribution with standardDeviation
    std::normal_distribution<float> distributionTmp{0.0, standardDeviation};
    // update the standardDeviation
    distribution.param(distributionTmp.param());
}

GaussianProposal::~GaussianProposal() {

}

std::vector<float> GaussianProposal::propose(std::vector<float> params) {
    std::vector<float> proposedParams;

    for (const auto& param : params) {
        proposedParams.push_back(param + standardDeviation * distribution(generator) );
    }

    return proposedParams;
}

float GaussianProposal::evaluateLogTransitionProbability(std::vector<float> fromParams, std::vector<float> toParams) {
    
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