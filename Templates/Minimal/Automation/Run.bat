@echo off
setlocal
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0Run.ps1" %*
set "LOOM_EXIT_CODE=%ERRORLEVEL%"
if not "%LOOM_EXIT_CODE%"=="0" echo Loom action failed with exit code %LOOM_EXIT_CODE%.
if not defined LOOM_AUTOMATION_NO_PAUSE pause
exit /b %LOOM_EXIT_CODE%
