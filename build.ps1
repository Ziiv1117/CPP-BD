$ErrorActionPreference = "Stop"

$Root = Split-Path -Parent $MyInvocation.MyCommand.Path
$BuildDir = Join-Path $Root "build"
$Output = Join-Path $BuildDir "afterglow_raylib.exe"
$CondaMingwBin = "C:\Users\Lenovo\miniconda3\envs\ttff\Library\mingw-w64\bin"
$Gxx = Join-Path $CondaMingwBin "g++.exe"
$RaylibRoot = if ($env:RAYLIB_ROOT) {
    $env:RAYLIB_ROOT
} else {
    "C:\Users\Lenovo\Documents\dev-tools\raylib-src-5.5\src"
}

if (!(Test-Path $Gxx)) {
    $Gxx = "g++"
}
if (!(Test-Path (Join-Path $RaylibRoot "raylib.h"))) {
    throw "raylib.h not found. Set RAYLIB_ROOT or build raylib at $RaylibRoot"
}
if (!(Test-Path (Join-Path $RaylibRoot "libraylib.a"))) {
    throw "libraylib.a not found. Build raylib first in $RaylibRoot"
}

New-Item -ItemType Directory -Force -Path $BuildDir | Out-Null

$Sources = Get-ChildItem -Path (Join-Path $Root "src") -Recurse -Filter *.cpp |
    ForEach-Object { $_.FullName }

$Arguments = @(
    "-std=c++1z",
    "-Wall",
    "-Wextra",
    "-pedantic"
) + $Sources + @(
    "-o", $Output,
    "-I", (Join-Path $Root "src"),
    "-I", $RaylibRoot,
    "-L", $RaylibRoot,
    "-lraylib",
    "-lopengl32",
    "-lgdi32",
    "-lwinmm",
    "-static-libgcc",
    "-static-libstdc++"
)

& $Gxx @Arguments
if ($LASTEXITCODE -ne 0) {
    throw "g++ failed with exit code $LASTEXITCODE"
}

Write-Host "Built $Output"
