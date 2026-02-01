# 16-Second — Usage (Early Scaffold)

## Current state (Milestone 7)
- Basic delay line with Delay Time, Feedback, Mix, and Output Gain.
- Record/Play/Overdub state machine with destructive overdub and erosion.

## Controls
- Delay Time: 0 ms to 16 s.
- Feedback: 0.0 to 1.2.
- Mix: 0 to 1 (dry/wet).
- Overdub Level: 0 to 1 (amount of input written during overdub).
- Erode Amount: 0 to 1 (higher erodes more each overdub pass).
- Output Gain: -24 dB to +12 dB.
- Record: clears buffer and captures a loop while enabled.
- Play: plays the last recorded loop when available.
- Overdub: destructive sound-on-sound write into the loop.
- Clear: resets loop memory and state.
- Half-speed: plays loop at half speed (lower pitch, longer duration).
- Reverse: plays loop backwards.
- Authentic: toggles unsafe delay time behavior (abrupt pointer jumps).
- Filter: darkens feedback and loop writes.
- Noise/Grit: adds noise + bit reduction in the feedback loop.
- Limiter: safety limiter at output (on by default).
- Mod Depth: modulation depth for delay time.
- Mod Speed: modulation speed (0.05–8 Hz).

## Presets
Starter presets are available via the host preset menu:
- Unsafe Fripp Wash
- Clock Tear
- Half-speed Ghosts
- Reverse Smear
- Erode Drone

## Notes
- Loop capture is intentionally minimal and will be refined in later milestones.
- SAFE-ish mode uses delay-time smoothing with linear interpolation.
