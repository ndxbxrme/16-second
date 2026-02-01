#include "FeedbackModel.h"

#include <algorithm>
#include <cmath>



void FeedbackModel::reset(double newSampleRate)
{
    sampleRate = (newSampleRate > 0.0) ? newSampleRate : 44100.0;
    lpState = 0.0f;
    lpAlpha = 1.0f;
    quantizeLevels = 0;
}

float FeedbackModel::process(float input,
                             float filterAmount,
                             float noiseAmount,
                             float feedbackGain,
                             float random01)
{
    updateFilter(filterAmount, noiseAmount);

    auto value = lowpass(input);

    // Soft clip
    value = std::tanh(value);

    // Quantize (bit crush)
    if (quantizeLevels > 1)
    {
        const auto normalized = (value + 1.0f) * 0.5f;
        const auto stepped = std::round(normalized * static_cast<float>(quantizeLevels - 1)) /
                             static_cast<float>(quantizeLevels - 1);
        value = stepped * 2.0f - 1.0f;
    }

    // Noise injection
    if (noiseAmount > 0.0f)
    {
        const auto noise = random01 * 2.0f - 1.0f;
        value += noise * noiseAmount * 0.02f;
    }

    value *= feedbackGain;

    if (!std::isfinite(value))
        return 0.0f;

    return value;
}

void FeedbackModel::updateFilter(float filterAmount, float noiseAmount)
{
    filterAmount = std::clamp(filterAmount, 0.0f, 1.0f);

    const auto minHz = 800.0f;
    const auto maxHz = 12000.0f;
    const auto cutoff = minHz + (maxHz - minHz) * filterAmount;
    constexpr float kPi = 3.14159265358979323846f;
    const auto x = std::exp(-2.0f * kPi * cutoff / static_cast<float>(sampleRate));
    lpAlpha = 1.0f - x;

    const auto maxLevels = 256;
    const auto levelFloat = 2.0f + (maxLevels - 2.0f) * (1.0f - noiseAmount);
    quantizeLevels = static_cast<int>(levelFloat);
}

float FeedbackModel::lowpass(float input)
{
    lpState += lpAlpha * (input - lpState);
    return lpState;
}
