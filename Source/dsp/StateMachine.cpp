#include "StateMachine.h"

LoopState StateMachine::update(bool record, bool play, bool overdub, bool hasLoop, bool clearEdge)
{
    if (clearEdge)
        return LoopState::Idle;

    if (record)
        return LoopState::Record;

    if (overdub)
        return hasLoop ? LoopState::Overdub : LoopState::Record;

    if (play)
        return hasLoop ? LoopState::Play : LoopState::Idle;

    return LoopState::Idle;
}
