# 16-Second (VST3)
[![CI](https://github.com/ndxbxrme/16-second/actions/workflows/ci.yml/badge.svg)](https://github.com/ndxbxrme/16-second/actions/workflows/ci.yml)

Experimental VST3 inspired by the Electro-Harmonix 16-Second Digital Delay. The focus is on the behavioral feel (unsafe time changes, destructive overdub, evolving feedback), not a circuit-perfect emulation.

## Status
- Milestone 6: added lossy feedback model (filter + noise/grit) and feedback degradation on overdub.

## Build (Linux/WSL)
Dependencies (Ubuntu/Debian):
```
sudo apt-get update
sudo apt-get install -y \
  cmake pkg-config \
  libx11-dev libxext-dev libxinerama-dev libxrandr-dev libxcursor-dev libxi-dev \
  libxcomposite-dev libxdamage-dev libxfixes-dev libxrender-dev libxss-dev \
  libxkbcommon-dev libxkbcommon-x11-dev \
  libfreetype6-dev libasound2-dev libcurl4-openssl-dev
```

Configure and build:
```
cmake -S . -B build_juce6
cmake --build build_juce6
```

Run tests:
```
cd build_juce6
ctest -V
```

## Scripts
If you have npm installed, convenience scripts exist:
```
npm run configure
npm run build
npm run ctest
```

## Docs
- Spec: `docs/SPEC.md`
- Usage: `docs/USAGE.md`
- Changelog: `docs/CHANGELOG.md`

## Notes
- We pin JUCE 6.1.6 for CMake 3.16 compatibility.
- AU/Standalone targets are not enabled yet.
