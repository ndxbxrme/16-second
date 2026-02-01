#pragma once

class LFO
{
public:
    void reset(double sampleRate);
    void setFrequency(float frequencyHz);
    float process();
    float getPhase() const { return phase; }

private:
    void updateIncrement();

    double sampleRate = 44100.0;
    float frequency = 1.0f;
    float phase = 0.0f;
    float phaseInc = 0.0f;
};
