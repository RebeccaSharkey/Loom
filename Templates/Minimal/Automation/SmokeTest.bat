@echo off
call "%~dp0Run.bat" -SmokeTest %*
exit /b %ERRORLEVEL%
