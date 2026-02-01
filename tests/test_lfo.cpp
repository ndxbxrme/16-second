#include <catch2/catch_test_macros.hpp>

#include <algorithm>

#include "dsp/LFO.h"

TEST_CASE("LFO produces oscillation in range", "[lfo]")
{
    LFO lfo;
    lfo.reset(48000.0);
    lfo.setFrequency(1.0f);

    float minValue = 1.0f;
    float maxValue = -1.0f;
    for (int i = 0; i < 48000; ++i)
    {
        const auto value = lfo.process();
        minValue = std::min(minValue, value);
        maxValue = std::max(maxValue, value);
    }

    REQUIRE(minValue < -0.5f);
    REQUIRE(maxValue > 0.5f);
}
