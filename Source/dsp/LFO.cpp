#include "LFO.h"

#include <cmath>

void LFO::reset(double newSampleRate)
{
    sampleRate = (newSampleRate > 0.0) ? newSampleRate : 44100.0;
    phase = 0.0f;
    updateIncrement();
}

void LFO::setFrequency(float frequencyHz)
{
    frequency = (frequencyHz > 0.0f) ? frequencyHz : 0.0f;
    updateIncrement();
}

float LFO::process()
{
    const auto value = std::sin(phase);
    constexpr float kTwoPi = 6.28318530717958647692f;
    phase += phaseInc;
    if (phase >= kTwoPi)
        phase -= kTwoPi;
    return value;
}

void LFO::updateIncrement()
{
    constexpr float kTwoPi = 6.28318530717958647692f;
    phaseInc = static_cast<float>(kTwoPi * frequency / sampleRate);
}
