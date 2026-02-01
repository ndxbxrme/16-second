#pragma once

class Smoother
{
public:
    void reset(double sampleRate, float initialValue, float timeMs);
    void setTimeMs(float timeMs);
    void setTarget(float targetValue);
    float process();
    float getCurrent() const { return current; }

private:
    void updateCoefficient();

    double sampleRate = 44100.0;
    float timeMs = 10.0f;
    float current = 0.0f;
    float target = 0.0f;
    float coefficient = 1.0f;
};
