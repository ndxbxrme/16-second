#include <catch2/catch_test_macros.hpp>

#include <cmath>

#include "dsp/FeedbackModel.h"

TEST_CASE("FeedbackModel outputs finite values", "[feedback]")
{
    FeedbackModel model;
    model.reset(48000.0);

    float value = 0.5f;
    for (int i = 0; i < 512; ++i)
    {
        value = model.process(value, 0.5f, 0.5f, 1.1f, 0.5f);
        REQUIRE(std::isfinite(value));
    }
}
