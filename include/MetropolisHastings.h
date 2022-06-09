#ifndef METROPOLISHASTINGS_H
#define METROPOLISHASTINGS_H

#include <string>
#include <fstream>
#include <json/json.hpp>
#include <iostream>
#include <time.h>
#include "BuildSSM.h"
#include "BodyParameters.h"
#include "GaussianPrior.h"
#include "GaussianProportionEvaluator.h"
#include "GaussianProposal.h"

class MetropolisHastings {
public:
    MetropolisHastings(BuildSSM* shapeModel, Eigen::VectorXf initCoefficients, BodyParameters observedParams, float proposalStddev, float likelihoodStddev, float priorStddev);
    ~MetropolisHastings();

    void run(unsigned int iterations);

private:
    BuildSSM* ssm;
    Eigen::VectorXf shapeCoefficients;
    BodyParameters observedBodyParams;
    GaussianProposal* proposalDistribution;
    float likelihoodStddev;
    float priorStddev;
};

#endif // METROPOLISHASTINGS_H