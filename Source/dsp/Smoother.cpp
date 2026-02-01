#include "Smoother.h"

#include <algorithm>
#include <cmath>

void Smoother::reset(double newSampleRate, float initialValue, float newTimeMs)
{
    sampleRate = (newSampleRate > 0.0) ? newSampleRate : 44100.0;
    current = initialValue;
    target = initialValue;
    timeMs = std::max(0.0f, newTimeMs);
    updateCoefficient();
}

void Smoother::setTimeMs(float newTimeMs)
{
    timeMs = std::max(0.0f, newTimeMs);
    updateCoefficient();
}

void Smoother::setTarget(float targetValue)
{
    target = targetValue;
}

float Smoother::process()
{
    current += (target - current) * coefficient;
    return current;
}

void Smoother::updateCoefficient()
{
    if (timeMs <= 0.0f)
    {
        coefficient = 1.0f;
        return;
    }

    const auto timeSeconds = timeMs * 0.001f;
    const auto samples = static_cast<float>(std::max(1.0, timeSeconds * sampleRate));
    coefficient = 1.0f - std::exp(-1.0f / samples);
}
