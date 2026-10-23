@echo off
call "%~dp0Run.bat" -Packaged %*
exit /b %ERRORLEVEL%
