#pragma once

enum class LoopState
{
    Idle,
    Record,
    Play,
    Overdub
};

class StateMachine
{
public:
    LoopState update(bool record, bool play, bool overdub, bool hasLoop, bool clearEdge);
};
