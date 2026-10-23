@echo off
if exist "%~dp0..\..\Loom.exe" (
    start "" "%~dp0..\..\Loom.exe" %*
    exit /b 0
)
powershell.exe -NoProfile -STA -ExecutionPolicy Bypass -File "%~dp0New-LoomProjectGui.ps1" %*
