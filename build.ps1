$ErrorActionPreference = "Stop"

$Root = Split-Path -Parent $MyInvocation.MyCommand.Path
$BuildDir = Join-Path $Root "build"
$Output = Join-Path $BuildDir "afterglow_raylib.exe"
$RaylibDll = "D:\msys64\ucrt64\bin\libraylib.dll"

New-Item -ItemType Directory -Force -Path $BuildDir | Out-Null

$Sources = Get-ChildItem -Path (Join-Path $Root "src") -Recurse -Filter *.cpp |
    ForEach-Object { $_.FullName }

g++ -std=c++17 -Wall -Wextra -pedantic @Sources -o $Output `
    -I(Join-Path $Root "src") `
    -ID:\msys64\ucrt64\include `
    -LD:\msys64\ucrt64\lib `
    -lraylib -lopengl32 -lgdi32 -lwinmm

if (Test-Path $RaylibDll) {
    Copy-Item -Force $RaylibDll $BuildDir
}

Write-Host "Built $Output"
