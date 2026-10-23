@echo off
powershell.exe -NoProfile -STA -ExecutionPolicy Bypass -File "%~dp0New-LoomProjectGui.ps1" %*
