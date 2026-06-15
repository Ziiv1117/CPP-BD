$ErrorActionPreference = "Stop"

$Root = Split-Path -Parent $MyInvocation.MyCommand.Path
$WebDir = Join-Path $Root "web"
$Port = 8080

if (!(Test-Path (Join-Path $WebDir "index.html"))) {
    & (Join-Path $Root "build_web.ps1")
}

$listener = Get-NetTCPConnection -LocalPort $Port -State Listen -ErrorAction SilentlyContinue
if ($listener) {
    Write-Host "Web server already running on http://localhost:$Port"
} else {
    Start-Process -WindowStyle Hidden -FilePath "python" -ArgumentList "-m", "http.server", "$Port", "--directory", "$WebDir"
    Start-Sleep -Seconds 1
    Write-Host "Web server started on http://localhost:$Port"
}

$Url = "http://localhost:$Port"
Start-Process $Url
Write-Host "Opened $Url"
