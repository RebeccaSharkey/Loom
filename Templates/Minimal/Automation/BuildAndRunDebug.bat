@echo off
call "%~dp0Run.bat" -Configure -Build -Configuration Debug %*
exit /b %ERRORLEVEL%
