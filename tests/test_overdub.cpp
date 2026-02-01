#include <catch2/catch_test_macros.hpp>

#include <cmath>

#include "dsp/Overdub.h"

static bool approxEqual(float a, float b, float epsilon = 1.0e-4f)
{
    return std::abs(a - b) <= epsilon;
}

TEST_CASE("Overdub retain factor maps erode amount", "[overdub]")
{
    REQUIRE(approxEqual(Overdub::computeRetain(0.0f), 0.9995f));
    REQUIRE(approxEqual(Overdub::computeRetain(1.0f), 0.97f));
}

TEST_CASE("Overdub apply blends retain + input + feedback", "[overdub]")
{
    const auto existing = 0.5f;
    const auto input = 0.2f;
    const auto read = 0.5f;
    const auto overdubLevel = 0.6f;
    const auto feedback = 0.5f;
    const auto erode = 0.5f;

    const auto retain = Overdub::computeRetain(erode);
    const auto expected = existing * retain + (input * overdubLevel + read * feedback);
    const auto result = Overdub::apply(existing, input, read, overdubLevel, feedback, erode);

    REQUIRE(approxEqual(result, expected));
}
