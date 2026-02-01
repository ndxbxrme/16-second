#include <catch2/catch_test_macros.hpp>

#include "dsp/Smoother.h"

TEST_CASE("Smoother approaches target without overshoot", "[smoother]")
{
    Smoother smoother;
    smoother.reset(48000.0, 0.0f, 10.0f);
    smoother.setTarget(1.0f);

    float previous = smoother.getCurrent();
    for (int i = 0; i < 256; ++i)
    {
        const auto value = smoother.process();
        REQUIRE(value >= previous);
        REQUIRE(value <= 1.0f);
        previous = value;
    }
}
