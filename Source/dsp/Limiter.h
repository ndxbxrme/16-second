#pragma once

class Limiter
{
public:
    void reset(double sampleRate);
    void setThreshold(float newThreshold);
    void setAttackMs(float newAttackMs);
    void setReleaseMs(float newReleaseMs);

    float process(float input);

private:
    void updateCoefficients();

    double sampleRate = 44100.0;
    float threshold = 0.98f;
    float attackMs = 1.0f;
    float releaseMs = 50.0f;
    float envelope = 0.0f;
    float attackCoeff = 1.0f;
    float releaseCoeff = 1.0f;
};
