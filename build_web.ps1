$ErrorActionPreference = "Stop"

$Root = Split-Path -Parent $MyInvocation.MyCommand.Path
$ToolsRoot = "C:\Users\Lenovo\Documents\dev-tools"
$Emsdk = if ($env:EMSDK) {
    $env:EMSDK
} elseif (Test-Path "C:\Users\15963\Desktop\emsdk\upstream\emscripten\emcc.exe") {
    "C:\Users\15963\Desktop\emsdk"
} else {
    Join-Path $ToolsRoot "emsdk"
}
$RaylibRoot = if ($env:RAYLIB_WEB_ROOT) {
    $env:RAYLIB_WEB_ROOT
} elseif (Test-Path "C:\Users\15963\Desktop\raylib-5.5-web\src\libraylib.a") {
    "C:\Users\15963\Desktop\raylib-5.5-web"
} else {
    Join-Path $ToolsRoot "raylib-web-src-5.5"
}
$WebDir = Join-Path $Root "web"
$Output = Join-Path $WebDir "index.html"
$Emcc = Join-Path $Emsdk "upstream\emscripten\emcc.exe"
$RaylibLib = Join-Path $RaylibRoot "src\libraylib.a"

if (!(Test-Path $Emcc)) {
    throw "Emscripten not found at $Emcc. Install emsdk at $Emsdk or set EMSDK."
}
if (!(Test-Path $RaylibLib)) {
    throw "Raylib web library not found at $RaylibLib. Build raylib for web or set RAYLIB_WEB_ROOT."
}
$FontDir = Join-Path $Root "assets\fonts"
if (!(Test-Path (Join-Path $FontDir "LXGWWenKai-Regular.ttf"))) {
    throw "Missing web font: assets\fonts\LXGWWenKai-Regular.ttf"
}

New-Item -ItemType Directory -Force -Path $WebDir | Out-Null

$env:EMSDK = $Emsdk.Replace("\", "/")
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
$env:PATH = "$Emsdk;$($Emsdk)\upstream\emscripten;$env:PATH"

$Sources = Get-ChildItem -Path (Join-Path $Root "src") -Recurse -Filter *.cpp |
    ForEach-Object { $_.FullName }
$PreloadFonts = ($FontDir.Replace("\", "/")) + "@/assets/fonts"

& $Emcc -std=c++17 -Wall -Wextra -pedantic -DPLATFORM_WEB @Sources `
    -o $Output `
    -I(Join-Path $Root "src") `
    -I(Join-Path $RaylibRoot "src") `
    -L(Join-Path $RaylibRoot "src") `
    -lraylib `
    -s USE_GLFW=3 `
    -s ASYNCIFY `
    -s ALLOW_MEMORY_GROWTH=1 `
    -s NO_EXIT_RUNTIME=1 `
    -s FORCE_FILESYSTEM=1 `
    --preload-file $PreloadFonts `
    --shell-file (Join-Path $WebDir "shell.html")

if ($LASTEXITCODE -ne 0) {
    throw "emcc failed with exit code $LASTEXITCODE"
}

Write-Host "Built $Output"
