param(
  [string]$BuildDir = "build_win",
  [string]$Vst3Dir = "C:\Program Files\Common Files\VST3"
)

$ErrorActionPreference = "Stop"

$source = Join-Path $BuildDir "16Second_artefacts\VST3\16-Second.vst3"
if (-not (Test-Path $source)) {
  Write-Error "VST3 bundle not found at: $source"
}

if (-not (Test-Path $Vst3Dir)) {
  Write-Error "VST3 directory not found: $Vst3Dir"
}

Copy-Item -Path $source -Destination $Vst3Dir -Recurse -Force
Write-Host "Copied to $Vst3Dir"
