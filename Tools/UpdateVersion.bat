@echo off
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0UpdateVersion.ps1" %*
set "LOOM_EXIT_CODE=%ERRORLEVEL%"
if not defined LOOM_UPDATE_VERSION_NO_PAUSE pause
exit /b %LOOM_EXIT_CODE%
