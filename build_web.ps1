$ErrorActionPreference = "Stop"

$Root = Split-Path -Parent $MyInvocation.MyCommand.Path
$Emsdk = "C:\Users\15963\Desktop\emsdk"
$RaylibRoot = "C:\Users\15963\Desktop\raylib-5.5-web"
$WebDir = Join-Path $Root "web"
$Output = Join-Path $WebDir "index.html"
$Emcc = Join-Path $Emsdk "upstream\emscripten\emcc.exe"
$RaylibLib = Join-Path $RaylibRoot "src\libraylib.a"

if (!(Test-Path $Emcc)) {
    throw "Emscripten not found at $Emcc"
}
if (!(Test-Path $RaylibLib)) {
    throw "Raylib web library not found at $RaylibLib"
}
$FontDir = Join-Path $Root "assets\fonts"
if (!(Test-Path (Join-Path $FontDir "NotoSansSC-VF.ttf"))) {
    throw "Missing web font: assets\fonts\NotoSansSC-VF.ttf"
}

New-Item -ItemType Directory -Force -Path $WebDir | Out-Null

$env:EMSDK = $Emsdk.Replace("\", "/")
$env:EMSDK_NODE = Join-Path $Emsdk "node\22.16.0_64bit\bin\node.exe"
$env:EMSDK_PYTHON = Join-Path $Emsdk "python\3.13.3_64bit\python.exe"
$env:PATH = "$Emsdk;$($Emsdk)\upstream\emscripten;$($Emsdk)\node\22.16.0_64bit\bin;$env:PATH"

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

Write-Host "Built $Output"
