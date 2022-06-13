#ifndef BODYPARAMETERS_H
#define BODYPARAMETERS_H


class BodyParameters {
public:
    BodyParameters() { }
    ~BodyParameters() { }

    float armSpan;
    float shoulderWidth;
    float chestWidth;
    float thighWidth;
    float stomachWidth;
    float legHeight;
    float height;

    // ratios
    float armSpanRatio;
    float shoulderWidthRatio;
    float chestWidthRatio;
    float thighWidthRatio;
    float stomachWidthRatio;
    float legHeightRatio;
};

#endif // BODYPARAMETERS_H