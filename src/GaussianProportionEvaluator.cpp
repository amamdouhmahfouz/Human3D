#include "GaussianProportionEvaluator.h"

float GaussianProportionEvaluator::evaluateLogProportions(float standardDeviation, BodyParameters observedParams, BodyParameters calculatedParams) {
    // mean: calculatedParam
    // x: observedParam

    float denom = -0.5 * log(2 * M_PI) - log(standardDeviation);
    
    float diffSquaredArmSpan =  (observedParams.armSpan - calculatedParams.armSpan) * (observedParams.armSpan - calculatedParams.armSpan);
    float valueArmSpan = denom - 0.5 * diffSquaredArmSpan / (standardDeviation * standardDeviation);

    float diffSquaredHeight =  (observedParams.height - calculatedParams.height) * (observedParams.height - calculatedParams.height);
    float valueHeight = denom - 0.5 * diffSquaredHeight / (standardDeviation * standardDeviation);

    return valueArmSpan + valueHeight;

}

