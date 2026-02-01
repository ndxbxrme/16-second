#include "Overdub.h"

#include <algorithm>

float Overdub::computeRetain(float erodeAmount)
{
    erodeAmount = std::clamp(erodeAmount, 0.0f, 1.0f);
    const auto retainMin = 0.97f;
    const auto retainMax = 0.9995f;
    return retainMin + (retainMax - retainMin) * (1.0f - erodeAmount);
}

float Overdub::apply(float existingSample, float inputSample, float readSample, float overdubLevel,
                     float feedback, float erodeAmount)
{
    const auto retain = computeRetain(erodeAmount);
    const auto writeInput = inputSample * overdubLevel + readSample * feedback;
    return existingSample * retain + writeInput;
}
