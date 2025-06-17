import os
import subprocess
from pathlib import Path
import json

def rewrite_cmake_options(project_name, project_root):
    from pathlib import Path
    root = Path(project_root)

    engine_path = Path(__file__).resolve().parents[3].as_posix()
    config_path = root / "Config" / "LoomProject.json"
    modules_path = root / "Generated" / "GameModules.cmake"

    print(f"Reading config from: {config_path}")

    if os.path.getsize(config_path) == 0:
        raise ValueError("JSON file is empty")

    with open(config_path, 'r') as f:
        config = json.load(f)
        print(f"File contents:\n{config}")

    enabled_modules = config.get("modules", {})
    lines = [
        f'file(GLOB_RECURSE GAME_SOURCES "${{CMAKE_CURRENT_SOURCE_DIR}}/Source/*.cpp")',
        f'file(GLOB_RECURSE GAME_HEADERS "${{CMAKE_CURRENT_SOURCE_DIR}}/Source/*.h")',
        f'add_executable({project_name} ${{GAME_SOURCES}} ${{GAME_HEADERS}})',
        f'target_include_directories({project_name} PRIVATE "${{CMAKE_CURRENT_SOURCE_DIR}}/Source")',
    ]

    if enabled_modules.get("Render2D"):
        lines.append(f'')
        lines.append(f'add_library(LoomRender2D SHARED IMPORTED)')
        lines.append(f'set_target_properties(LoomRender2D PROPERTIES')
        lines.append(f'        IMPORTED_LOCATION "{engine_path}/Build/Bin/libLoomRender2D.dll"')
        lines.append(f'        IMPORTED_IMPLIB   "{engine_path}/Build/Lib/libLoomRender2D.dll.a"')
        lines.append(f'        INTERFACE_INCLUDE_DIRECTORIES "{engine_path}/LoomEngine/LoomRender2D/Include"')
        lines.append(f')')
        lines.append(f'target_link_libraries({project_name} PRIVATE LoomRender2D)')
        lines.append(f'add_custom_command(TARGET {project_name} POST_BUILD')
        lines.append(f'        COMMAND ${{CMAKE_COMMAND}} -E copy_if_different')
        lines.append(f'        "{engine_path}/Build/Bin/libLoomRender2D.dll"')
        lines.append(f'        "$<TARGET_FILE_DIR:{project_name}>"')
        lines.append(f')')

    modules_path.write_text("\n".join(lines))
    print(f"Generated GameModules.cmake")

def update_engine(project_root):
    script_path = Path(__file__).resolve().parent / "LoomUpdateEngine.py"
    subprocess.run(["python", script_path, "remake", project_root], check=True)

def main(project_name, project_root):
    rewrite_cmake_options(project_name, project_root)
    update_engine(project_root)
    print(f"Project '{project_name}' updated.")

if __name__ == "__main__":
    import sys
    if len(sys.argv) == 4 and sys.argv[1] == "run":
        main(sys.argv[2], sys.argv[3])
    else:
        print("Usage: python LoomUpdateEngineModules.py run <ProjectName> <DestinationPath>")