param(
    [switch]$SkipBuild,
    [switch]$Smoke
)

$ErrorActionPreference = "Stop"

$Root = Split-Path -Parent $MyInvocation.MyCommand.Path
$BuildScript = Join-Path $Root "build.ps1"
$GameExe = Join-Path $Root "build\afterglow_raylib.exe"

if (-not $SkipBuild) {
    & $BuildScript
}

if (-not (Test-Path $GameExe)) {
    throw "Game executable was not found: $GameExe"
}

if ($Smoke) {
    & $GameExe --smoke
} else {
    & $GameExe
}
