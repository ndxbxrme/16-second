#include <catch2/catch_test_macros.hpp>

#include "dsp/RateStepper.h"

TEST_CASE("RateStepper forward stepping", "[rate]")
{
    RateStepper stepper;
    stepper.reset(0.0);
    stepper.setRate(1.0);

    REQUIRE(stepper.getIndex(10) == 0);
    stepper.advance();
    REQUIRE(stepper.getIndex(10) == 1);
    stepper.advance();
    REQUIRE(stepper.getIndex(10) == 2);
}

TEST_CASE("RateStepper reverse stepping", "[rate]")
{
    RateStepper stepper;
    stepper.reset(0.0);
    stepper.setRate(-1.0);

    REQUIRE(stepper.getIndex(10) == 0);
    stepper.advance();
    REQUIRE(stepper.getIndex(10) == 9);
    stepper.advance();
    REQUIRE(stepper.getIndex(10) == 8);
}

TEST_CASE("RateStepper half-speed stepping", "[rate]")
{
    RateStepper stepper;
    stepper.reset(0.0);
    stepper.setRate(0.5);

    REQUIRE(stepper.getIndex(10) == 0);
    stepper.advance();
    REQUIRE(stepper.getIndex(10) == 0);
    stepper.advance();
    REQUIRE(stepper.getIndex(10) == 1);
}

TEST_CASE("RateStepper wraps buffer size", "[rate]")
{
    RateStepper stepper;
    stepper.reset(9.0);
    stepper.setRate(1.0);

    REQUIRE(stepper.getIndex(10) == 9);
    stepper.advance();
    REQUIRE(stepper.getIndex(10) == 0);
}
