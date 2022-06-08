#ifndef PROPOSALDISTRIBUTION_H
#define PROPOSALDISTRIBUTION_H

#include <vector>

class ProposalDistribution {
public:
    ProposalDistribution(float standardDeviation) {
        this->standardDeviation = standardDeviation;
    }
    // add a random walk perturbation to params using standard deviation and a gaussian number
    // i.e foreach x in params: x = x + standardDeviation * randomGaussian
    virtual std::vector<float> propose(std::vector<float> params) = 0;
    virtual float evaluateLogTransitionProbability(std::vector<float> fromParams, std::vector<float> toParams) = 0;

protected:
    float standardDeviation;
};


#endif // PROPOSALDISTRIBUTION_H