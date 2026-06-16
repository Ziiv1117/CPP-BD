$ErrorActionPreference = "Stop"

$ToolsRoot = "C:\Users\Lenovo\Documents\dev-tools"
$Emsdk = if ($env:EMSDK) {
    $env:EMSDK
} else {
    Join-Path $ToolsRoot "emsdk"
}
$DesktopRaylib = if ($env:RAYLIB_ROOT) {
    $env:RAYLIB_ROOT
} else {
    Join-Path $ToolsRoot "raylib-src-5.5\src"
}
$WebRaylibRoot = if ($env:RAYLIB_WEB_ROOT) {
    $env:RAYLIB_WEB_ROOT
} else {
    Join-Path $ToolsRoot "raylib-web-src-5.5"
}
$Emcc = Join-Path $Emsdk "upstream\emscripten\emcc.exe"

if (!(Test-Path $Emcc)) {
    throw "Emscripten not found at $Emcc. Install emsdk at $Emsdk or set EMSDK."
}
if (!(Test-Path (Join-Path $DesktopRaylib "raylib.h"))) {
    throw "Desktop raylib source not found at $DesktopRaylib"
}

if (!(Test-Path $WebRaylibRoot)) {
    New-Item -ItemType Directory -Force -Path $WebRaylibRoot | Out-Null
    New-Item -ItemType Directory -Force -Path (Join-Path $WebRaylibRoot "src") | Out-Null
    Copy-Item -Recurse -Force (Join-Path $DesktopRaylib "*") (Join-Path $WebRaylibRoot "src")
}
$RaylibSrc = Join-Path $WebRaylibRoot "src"
Get-ChildItem -Path $RaylibSrc -Include *.o,*.a -File -Recurse -ErrorAction SilentlyContinue |
    Remove-Item -Force

$NodeDir = Get-ChildItem -Path (Join-Path $Emsdk "node") -Directory -ErrorAction SilentlyContinue |
    Select-Object -First 1
$PythonDir = Get-ChildItem -Path (Join-Path $Emsdk "python") -Directory -ErrorAction SilentlyContinue |
    Select-Object -First 1
if ($NodeDir) {
    $env:EMSDK_NODE = Join-Path $NodeDir.FullName "bin\node.exe"
    $env:PATH = "$($NodeDir.FullName)\bin;$env:PATH"
}
if ($PythonDir) {
    $env:EMSDK_PYTHON = Join-Path $PythonDir.FullName "python.exe"
}
$env:EMSDK = $Emsdk.Replace("\", "/")
$env:PATH = "$Emsdk;$($Emsdk)\upstream\emscripten;$env:PATH"

Push-Location $RaylibSrc
try {
    & $Emcc --version
    if ($LASTEXITCODE -ne 0) {
        throw "emcc failed with exit code $LASTEXITCODE"
    }

    & emmake mingw32-make PLATFORM=PLATFORM_WEB RAYLIB_LIBTYPE=STATIC
    if ($LASTEXITCODE -ne 0) {
        throw "raylib web build failed with exit code $LASTEXITCODE"
    }
} finally {
    Pop-Location
}

Write-Host "Built $(Join-Path $RaylibSrc 'libraylib.a')"
