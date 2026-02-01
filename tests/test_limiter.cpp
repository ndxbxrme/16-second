#include <catch2/catch_test_macros.hpp>

#include "dsp/Limiter.h"

TEST_CASE("Limiter clamps peaks above threshold", "[limiter]")
{
    Limiter limiter;
    limiter.reset(48000.0);
    limiter.setThreshold(0.5f);
    limiter.setAttackMs(0.1f);
    limiter.setReleaseMs(10.0f);

    const auto input = 1.0f;
    float output = input;
    for (int i = 0; i < 512; ++i)
        output = limiter.process(input);

    REQUIRE(output <= 0.5f + 1.0e-3f);
}
