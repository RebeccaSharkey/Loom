@echo off
call "%~dp0Run.bat" -Build -Configuration Release %*
exit /b %ERRORLEVEL%
