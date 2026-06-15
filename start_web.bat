@echo off
setlocal

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0start_web.ps1"
if errorlevel 1 (
    echo.
    echo Failed to start the web version.
    pause
)
