# SPEC.md — “16-Second” Inspired Digital Delay/Looper VST (Unsafe Memory Instrument)

## 0. One-line pitch
A VST3 (and optionally AU) plugin inspired by the Electro-Harmonix 16-Second Digital Delay: a single shared memory buffer with sliders and intentionally “unsafe” behavior (reclock artifacts, zippering, destructive overdub, evolving feedback), designed as a performance instrument rather than a clean looper.

## 1. Goals & Non-goals

### Goals
- Capture the *behavioral essence*:
  - One shared buffer acting as delay/looper.
  - Abrupt time changes that cause pitch tearing / zippering / discontinuities.
  - Half-speed that doubles perceived time (not time-stretch).
  - Destructive sound-on-sound overdub that erodes the past.
  - Feedback that “mutates” (lossy + noise + filtering), not clean repeats.
  - Reverse playback.
- Slider-first UI reminiscent of the hardware workflow.
- Two operating personalities:
  - **AUTHENTIC (unsafe):** minimal smoothing, clicks allowed, discontinuities are part of the sound.
  - **SAFE-ish:** light guardrails and smoothing for usability.
- Robust state save/restore and crash-free automation, even under extreme parameter movement.

### Non-goals (initial versions)
- Multi-track looping.
- Beat-synced quantized looping.
- Exact circuit-level emulation (we’re modeling behavior, not schematics).
- Sophisticated preset browser (basic presets OK).

## 2. Target platforms & plugin formats
- Plugin formats: **VST3** (must), **AU** (nice), **Standalone** (nice).
- OS: macOS + Windows in MVP.
- DAW E2E test targets: **FL Studio** and **Audacity**.
- Sample rates: 44.1k / 48k / 96k supported.
- Latency: report 0 samples.

## 3. Recommended tech stack
- Framework: **JUCE** (recommended).
- Language: C++17 (or C++20).
- Build: CMake (preferred).
- Unit test framework: Catch2 OR doctest OR JUCE UnitTest (pick one; keep it simple).
- Optional later: GitHub Actions CI for Win/mac build + tests.

## 4. Core model: “One Memory Buffer” engine

### 4.1 Terminology
- `buffer`: circular audio memory of fixed length in samples.
- `writeHead`: index where incoming audio writes.
- `readHead`: index where output reads.
- `rate`: playback rate multiplier (1.0 normal, 0.5 half-speed, -1.0 reverse).
- `delayTimeSamples`: delay time in samples (maps to read position / offset).
- `loopLengthSamples`: loop length in samples (<= max).

### 4.2 Max time
- Base max time at 1.0 rate: **16 seconds**.
- Half-speed doubles perceived duration: **32 seconds**.
- Optional “Extended” mode (non-authentic): 32 sec base (64 sec @ half-speed), clearly labeled.

### 4.3 Modes / states
State machine (minimum viable):

- **IDLE:** not recording; buffer may still run for “delay” behavior.
- **RECORD:** first capture; sets `loopLengthSamples` on stop.
- **PLAY:** loop playback repeats indefinitely.
- **OVERDUB:** destructive sound-on-sound write into buffer.
- **BYPASS:** dry-through (host bypass still supported).

Minimum requirement: RECORD toggle, PLAY toggle, OVERDUB toggle, CLEAR action.

### 4.4 Primary parameters (all automatable + MIDI-mappable)
Sliders:
1. **Delay Time**: 0 ms .. max (16 s) (display in ms/s)
2. **Feedback**: 0.0 .. 1.2 (allow >1 in AUTHENTIC)
3. **Mix**: 0..1 (constant-power curve)
4. **Output Gain**: -24 dB .. +12 dB
5. **Mod Depth**: 0..1
6. **Mod Speed**: 0..1 (map to Hz, e.g., 0.05–8 Hz)
7. **Filter/Tone**: 0..1 (map to cutoff, e.g., 800 Hz–12 kHz)
8. **Noise/Grit**: 0..1 (maps to noise + quantization intensity)
9. **Overdub Level**: 0..1
10. **Erode Amount**: 0..1 (controls retain factor)

Toggles:
- **Reverse**
- **Half-speed**
- **Kill Dry**
- **Limiter ON/OFF**
- **Mode: AUTHENTIC / SAFE-ish**

Buttons/footswitches:
- **Record**
- **Play**
- **Bypass**
- **Clear**

### 4.5 Loop length behavior
- When RECORD starts: start accumulating length.
- When RECORD stops: set `loopLengthSamples = clamp(elapsed, 1..max)`.
- Option “Fixed Length Record”: lock loop length to Delay Time at record start.
- Loop length persists across parameter changes unless user clears it.

### 4.6 Clear / Reset
- Zero buffer, reset heads, exit record/play/overdub.
- Optional later: one-step undo of last overdub pass.

## 5. The “weird” sauce: artifact modeling

### 5.1 Reclock/zipper behavior (moving Delay Time)
AUTHENTIC mode:
- No (or minimal) smoothing.
- Delay time changes cause:
  - abrupt read pointer jumps OR quantized stepping OR nearest-sample reads
  - discontinuities (clicks), pitch tears, aliasing.
SAFE-ish mode:
- Apply smoothing (5–20 ms) and linear interpolation on reads.
- Optional tiny crossfades on major pointer jumps.

### 5.2 Lossy feedback loop (“mutation”)
Feedback path is intentionally degradative:
- Low-pass filter (cutoff controlled by Filter slider, optionally darkens as feedback rises).
- Soft clip / saturation.
- Quantization (bit reduction) scaled by Noise/Grit.
- Noise injection scaled by Noise/Grit.
- (Optional later) gentle high-pass to keep washes from booming.

Suggested chain:
`feedbackSignal -> LPF -> softClip -> quantize -> addNoise -> back into buffer`

### 5.3 Destructive overdub erosion
Overdub write rule:
`buffer[write] = (buffer[write] * retain) + (input * overdubLevel)`

- `retain` derived from Erode Amount:
  - e.g., retain = lerp(0.97, 0.9995, 1 - erode)
- Add a small amount of the feedback-chain degradation on overdub too (for “memory wear”).

### 5.4 Reverse switching artifacts
AUTHENTIC:
- Switching direction may click; do not force clean alignment.
SAFE-ish:
- Optional short crossfade (e.g., 64–256 samples) when toggling reverse/half-speed.

## 6. UX / UI specification

### 6.1 Visual concept
- Dark panel with slider columns.
- LED indicators: REC, PLAY, OVERDUB, CLIP.
- Group boxes: DELAY, SWEEP/MOD, MIX, OUTPUT/VENT (avoid trademarked typography).

### 6.2 Minimal layout (suggested)
- Left: Record / Play / Bypass / Clear
- Middle-left: Delay Time + Feedback
- Middle: Mod Depth + Speed
- Middle-right: Mix + Filter + Noise/Grit
- Right: Output Gain + Limiter + Mode switch

### 6.3 Meters & safety
- Output meter (RMS-ish or peak).
- Clip LED.
- Limiter ON by default in SAFE-ish.

### 6.4 Presets
- Host preset support.
- Include starter presets:
  - Unsafe Fripp Wash
  - Clock Tear
  - Half-speed Ghosts
  - Reverse Smear
  - Erode Drone

## 7. MIDI & automation
- All parameters host-automatable.
- MIDI learn for:
  - Record/Play/Overdub/Reverse/Half-speed
  - Delay Time, Feedback, Mix
- Document expected behavior with automation at audio-rate-ish changes:
  - Must never crash, may sound chaotic in AUTHENTIC (desired).

## 8. Audio I/O behavior
- Stereo in/out.
- Internal buffer: separate L/R or interleaved stereo.
- If mono input, duplicate to stereo output or respect host channel layout.
- Kill Dry for send/return.

## 9. Safety & ear protection
- Transparent limiter at the end (toggle).
- Hard safety clamp on NaNs/Inf.
- “Danger” indicator when feedback > 1 or limiter off + output near clip.
- Output gain range limited to avoid accidental +30 dB disasters.

## 10. Quality targets
- No denormals; use JUCE ScopedNoDenormals or equivalent.
- CPU: low single digits typical at 48k.
- State save/restore robust across DAW reload.
- Parameter changes in AUTHENTIC may click; SAFE-ish should be much smoother.

---

## 11. Parameter table (exact ranges & units)

| Parameter | Type | Range | Default | Unit / Notes |
|---|---|---:|---:|---|
| Delay Time | slider | 0 .. 16000 | 450 | ms (display ms < 1000, else s) |
| Feedback | slider | 0.0 .. 1.2 | 0.65 | >1 allowed in AUTHENTIC |
| Mix | slider | 0 .. 1 | 0.5 | constant-power |
| Output Gain | slider | -24 .. +12 | 0 | dB |
| Mod Depth | slider | 0 .. 1 | 0.15 | scaled to time modulation depth |
| Mod Speed | slider | 0 .. 1 | 0.25 | map to 0.05–8 Hz |
| Filter/Tone | slider | 0 .. 1 | 0.6 | map to 800 Hz–12 kHz |
| Noise/Grit | slider | 0 .. 1 | 0.25 | noise + bit reduction intensity |
| Overdub Level | slider | 0 .. 1 | 0.6 | amount of input written |
| Erode Amount | slider | 0 .. 1 | 0.35 | controls retain factor |
| Reverse | toggle | off/on | off | negative rate when on |
| Half-speed | toggle | off/on | off | rate *= 0.5 |
| Kill Dry | toggle | off/on | off | for send/return |
| Limiter | toggle | off/on | on | default on in SAFE-ish |
| Mode | toggle | SAFE-ish/AUTHENTIC | SAFE-ish | default safe for first run |

---

## 12. Signal flow diagram (ASCII)

### 12.1 High-level
Input (L/R)
  |
  +--> [Dry Path] -----------------------------+
  |                                           |
  +--> [Record/Overdub Write] --> [Buffer] -->+--> [Read/Playback] --> [Wet Path] --> [Mix] --> [Output Gain] --> [Limiter] --> Out
                         ^                          |
                         |                          |
                    [Feedback Model] <--------------+

### 12.2 Feedback Model (default chain)
[Read Signal] -> LPF (Filter) -> SoftClip -> Quantize (Noise/Grit) -> AddNoise (Noise/Grit) -> Feedback Gain (Feedback slider) -> back to Write Mix

### 12.3 Overdub Write rule
WriteInput = (Input * OverdubLevel) + (ReadSignal * FeedbackGainProcessed)
BufferWrite = (BufferWrite * Retain(Erode)) + WriteInput

---

## 13. Testing strategy (unit, integration, and E2E)

### 13.1 Unit testing (must-have)
Write deterministic tests for DSP components that do not require a DAW.

**Scope to unit test**
- Circular buffer wrap-around correctness:
  - writeHead/readHead incrementing and wrapping at bounds.
- Rate handling:
  - forward, reverse, half-speed pointer stepping.
- Loop length clamp and persistence:
  - record time -> loopLengthSamples within max.
- State machine transitions:
  - IDLE -> RECORD -> PLAY -> OVERDUB -> PLAY, etc.
- Overdub erosion math:
  - verify retain/overdub levels produce expected sample values.
- Feedback model stability:
  - verify no NaNs/Inf even with extreme parameters.
- Parameter smoothing behavior:
  - SAFE-ish smoothing produces bounded slew rate.
  - AUTHENTIC mode bypasses smoothing.

**Guidelines**
- Tests should run headless via `ctest` / command line.
- Avoid floating-point brittle asserts:
  - use epsilon comparisons.
- Use fixed random seed for any noise tests (or test noise bounds, not exact values).
- Include regression tests for any crash bugs found during DAW E2E.

### 13.2 Integration tests (optional but recommended)
- Offline render tests:
  - feed known input (sine, impulse, ramp) and render N blocks.
  - verify output energy, length, and pointer progression.
- Golden-file approach (optional):
  - store expected output for a small input under SAFE-ish deterministic mode (noise off).
  - keep short to avoid repo bloat.

### 13.3 E2E manual testing (your workflow)
Primary hosts:
- **FL Studio**:
  - automation stress tests (rapid Delay Time sweeps, toggling reverse/half-speed).
  - record/play/overdub workflows.
  - session save/restore.
- **Audacity**:
  - verify basic processing stability and bounce/export.
  - check extreme feedback doesn’t produce runaway NaNs or silence.

**E2E acceptance checklist**
- Plugin loads, processes audio, and can be removed without crashing.
- Rapid automation does not crash; AUTHENTIC can sound brutal (intended).
- State restores correctly after project reload.
- No stuck states (e.g., Play LED on but silence forever) after extreme toggling.
- Output never becomes NaN/Inf (can be tested by adding internal asserts/logging in debug).

### 13.4 Debug instrumentation (dev-only)
- Optional in debug builds:
  - NaN/Inf detection in audio callback with counter.
  - lightweight ring-buffer log of last 256 parameter changes and state transitions.
  - “panic” button to Clear + reset heads if engine gets wedged.

---

# 14. Milestones

## Milestone 1 — Repo scaffolding + pass-through plugin
**Goal:** Buildable VST3 that passes audio through.
- JUCE project with CMake.
- Basic GUI window.
- Parameter system stubbed.
- Add unit test harness skeleton.

**Acceptance criteria**
- Loads in FL Studio and/or Audacity.
- Clean pass-through audio.
- Parameters appear in host automation list.
- `ctest` runs at least one trivial test.

## Milestone 2 — Core delay/loop buffer engine (normal rate)
**Goal:** Implement circular buffer, delay time, mix, feedback.
- Delay time up to 16 seconds.
- Basic record/play loop capture (can be simplified initially).
- Unit tests for buffer wrap and delay time boundaries.

**Acceptance criteria**
- Audible delay up to max time.
- Basic looping works.
- Unit tests pass.

## Milestone 3 — State machine + destructive overdub
**Goal:** Proper state transitions and erosion.
- Record sets loop length.
- Play repeats.
- Overdub degrades previous content.
- Clear resets.
- Unit tests for state transitions and erosion math.

**Acceptance criteria**
- Overdub audibly layers but erodes.
- No crashes during record/play toggling.
- Tests pass.

## Milestone 4 — Half-speed + Reverse (rate-based, no time-stretch)
**Goal:** Implement rate-based pointer stepping.
- Half-speed toggles rate to 0.5.
- Reverse toggles sign.
- Unit tests for pointer stepping and wrap.

**Acceptance criteria**
- Half-speed lowers pitch and extends apparent time.
- Reverse plays backward.
- Stable under repeated toggles.

## Milestone 5 — Authentic artifacts: reclock/zipper/aliasing + SAFE-ish smoothing
**Goal:** Unsafe time changes in AUTHENTIC mode.
- Implement hard pointer jumps / stepping / nearest-sample reads.
- SAFE-ish smoothing & interpolation.
- Unit tests for smoothing bounds.

**Acceptance criteria**
- Delay Time movement produces tearing artifacts in AUTHENTIC.
- SAFE-ish is noticeably smoother.
- No crash under automation.

## Milestone 6 — Lossy feedback & grit model + limiter
**Goal:** Mutating feedback chain.
- LPF, soft clip, quantize, noise injection.
- Sliders: Filter, Noise/Grit.
- Limiter toggle.
- Unit tests for NaN/Inf safety.

**Acceptance criteria**
- Long feedback darkens and becomes grainy.
- Feedback > 1 can self-oscillate (with limiter safety).
- Tests pass.

## Milestone 7 — Sweep/Mod section
**Goal:** Modulation of delay time / read offset.
- Speed + Depth sliders.
- Integrate with unsafe engine.

**Acceptance criteria**
- Chorusy drift at low depth.
- Wild sweeps at higher depth.
- Stable under automation.

## Milestone 8 — UI polish + meters + presets
**Goal:** Instrument feel.
- Final slider UI layout.
- LEDs and output meter.
- Starter presets.
- Document controls in `/docs/USAGE.md`.

**Acceptance criteria**
- Usable in live play.
- Presets load reliably.
- Session save/restore works in FL Studio.

## Milestone 9 — Release packaging + QA notes
**Goal:** Shippable alpha.
- Build scripts for Win/mac.
- README with known quirks + “AUTHENTIC warnings”.
- Optional CI builds + unit tests.

**Acceptance criteria**
- Tagged release builds.
- No major crashes in FL Studio/Audacity smoke tests.

---

## 15. Tasking guidance for Codex CLI agent

### Coding priorities
1. Correct, stable audio engine first (no UI perfection early).
2. AUTHENTIC behavior is first-class, not tacked on.
3. Unit tests for core DSP and state machine from Milestone 1 onward.
4. Keep parameters documented and deterministic.

### Deliverables per milestone
- PR with:
  - code changes
  - update `docs/CHANGELOG.md`
  - update `docs/USAGE.md` when behavior changes
  - tests added/updated

### Suggested file layout
- /Source
  - PluginProcessor.*
  - PluginEditor.*
  - dsp/
    - MemoryBuffer.*
    - StateMachine.*
    - FeedbackModel.*
    - Modulation.*
    - Smoothing.*
- /tests
  - test_buffer.cpp
  - test_state_machine.cpp
  - test_rate.cpp
  - test_feedback_safety.cpp
- /docs
  - SPEC.md
  - USAGE.md
  - CHANGELOG.md

---

## 16. Defaults if undecided
- Default mode on first run: SAFE-ish (with big “Enable AUTHENTIC” switch).
- Strict 16s base memory in AUTHENTIC; optional Extended mode off by default.
- SAFE-ish interpolation: linear (upgrade later if needed).
- writeHead runs continuously in PLAY (more “delay-like”), but overdub write only active in OVERDUB/RECORD.

