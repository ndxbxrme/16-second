#pragma once

class Overdub
{
public:
    static float computeRetain(float erodeAmount);
    static float apply(float existingSample, float inputSample, float readSample, float overdubLevel,
                       float feedback, float erodeAmount);
};
