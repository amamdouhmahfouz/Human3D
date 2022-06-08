#ifndef GAUSSIANPROPOSAL_H
#define GAUSSIANPROPOSAL_H

#include "ProposalDistribution.h"
#include <random>
#include <iostream>

class GaussianProposal : public ProposalDistribution {
public:
    GaussianProposal(float standardDeviation);
    ~GaussianProposal();

    std::vector<float> propose(std::vector<float> params) override;
    float evaluateLogTransitionProbability(std::vector<float> fromParams, std::vector<float> toParams) override;

private:
    std::default_random_engine generator;
    std::normal_distribution<float> distribution{0.0, 1.0};
};

#endif // GAUSSIANPROPOSAL_H