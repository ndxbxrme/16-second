# CHANGELOG

## 0.1.0 - 2026-02-01
- Initial JUCE+CMake scaffold for VST3.
- Pass-through audio with Output Gain parameter.
- Basic editor with Output Gain slider.
- Catch2 test harness with a sanity test.
- Pinned JUCE to 6.1.6 for CMake 3.16 compatibility.
- Added basic delay/loop engine with Delay Time, Feedback, Mix, Record, and Play.
- Added MemoryBuffer DSP helper and tests.
- Added state machine with Record/Play/Overdub and Clear.
- Added destructive overdub with Erode Amount + Overdub Level parameters.
- Added StateMachine and Overdub tests.
