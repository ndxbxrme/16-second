@echo off
setlocal

set "BUILD_DIR=build_win_2026"
set "VST3_DIR=C:\Program Files\Common Files\VST3"

cmake --preset win-release-2026 || exit /b 1
cmake --build --preset win-release-2026 || exit /b 1

PowerShell -ExecutionPolicy Bypass -File "%~dp0copy_vst3_windows.ps1" -BuildDir "%BUILD_DIR%" -Vst3Dir "%VST3_DIR%" || exit /b 1

echo Done.
endlocal
