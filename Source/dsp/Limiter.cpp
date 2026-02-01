#include "Limiter.h"

#include <algorithm>
#include <cmath>

void Limiter::reset(double newSampleRate)
{
    sampleRate = (newSampleRate > 0.0) ? newSampleRate : 44100.0;
    envelope = 0.0f;
    updateCoefficients();
}

void Limiter::setThreshold(float newThreshold)
{
    threshold = std::clamp(newThreshold, 0.01f, 1.0f);
}

void Limiter::setAttackMs(float newAttackMs)
{
    attackMs = std::max(0.1f, newAttackMs);
    updateCoefficients();
}

void Limiter::setReleaseMs(float newReleaseMs)
{
    releaseMs = std::max(1.0f, newReleaseMs);
    updateCoefficients();
}

float Limiter::process(float input)
{
    const auto absSample = std::abs(input);

    const auto coeff = (absSample > envelope) ? attackCoeff : releaseCoeff;
    envelope = coeff * envelope + (1.0f - coeff) * absSample;

    if (envelope <= threshold)
        return input;

    const auto gain = threshold / envelope;
    return input * gain;
}

void Limiter::updateCoefficients()
{
    const auto attackSamples = std::max(1.0f, attackMs * 0.001f * static_cast<float>(sampleRate));
    const auto releaseSamples = std::max(1.0f, releaseMs * 0.001f * static_cast<float>(sampleRate));

    attackCoeff = std::exp(-1.0f / attackSamples);
    releaseCoeff = std::exp(-1.0f / releaseSamples);
}
