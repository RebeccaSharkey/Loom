@echo off
echo === Running Engine Module Configurator ===

:: Move to the Loom root directory
cd /d "%~dp0\..\.."

:: Run the Python script with full paths
python Tools/EngineModuleLoader/configure_engine_modules.py --json Stitch/EngineConfig.json --out Stitch/

echo.
echo EngineBuildOptions.cmake has been regenerated.
echo You may now reload CMake in your IDE.
pause


