#pragma once

class FeedbackModel
{
public:
    void reset(double sampleRate);
    float process(float input,
                  float filterAmount,
                  float noiseAmount,
                  float feedbackGain,
                  float random01);

private:
    void updateFilter(float filterAmount, float noiseAmount);
    float lowpass(float input);

    double sampleRate = 44100.0;
    float lpAlpha = 1.0f;
    float lpState = 0.0f;
    int quantizeLevels = 0;
};
