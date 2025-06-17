@echo Create Loom Game...

set /p PROJECT_NAME=Project Name:

set /p PROJECT_DIR=Project Location:

python Scripts/LoomCLI.py init %PROJECT_NAME% %PROJECT_DIR%

pause