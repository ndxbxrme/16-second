#include <catch2/catch_test_macros.hpp>

#include "dsp/StateMachine.h"

TEST_CASE("StateMachine basic transitions", "[state]")
{
    StateMachine sm;

    REQUIRE(sm.update(false, false, false, false, false) == LoopState::Idle);
    REQUIRE(sm.update(true, false, false, false, false) == LoopState::Record);
    REQUIRE(sm.update(false, true, false, false, false) == LoopState::Idle);
    REQUIRE(sm.update(false, true, false, true, false) == LoopState::Play);
    REQUIRE(sm.update(false, false, true, false, false) == LoopState::Record);
    REQUIRE(sm.update(false, false, true, true, false) == LoopState::Overdub);
}

TEST_CASE("StateMachine clear edge forces idle", "[state]")
{
    StateMachine sm;

    REQUIRE(sm.update(true, false, false, true, true) == LoopState::Idle);
    REQUIRE(sm.update(false, true, true, true, true) == LoopState::Idle);
}
