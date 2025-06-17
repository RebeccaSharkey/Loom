import os
import json
import shutil
import subprocess
from pathlib import Path

TEMPLATE_DIR = Path(__file__).parent.parent / "Templates"

def create_project_structure(project_name, destination_path):
    root = Path(destination_path).resolve()
    (root / "Config").mkdir(parents=True, exist_ok=True)
    (root / "Source").mkdir(parents=True, exist_ok=True)
    (root / "Assets").mkdir(parents=True, exist_ok=True)
    (root / "Generated").mkdir(parents=True, exist_ok=True)
    (root / "Build").mkdir(parents=True, exist_ok=True)
    (root / "Scripts").mkdir(parents=True, exist_ok=True)

    engine_path = Path(__file__).resolve().parents[3].as_posix()
    tools_path = Path(__file__).resolve().parents[2].as_posix()

    with open(TEMPLATE_DIR / "main.cpp.txt") as f:
        (root / "Source" / "main.cpp").write_text(f.read().replace("{{PROJECT_NAME}}", project_name))

    with open(TEMPLATE_DIR / "LoomProject.json.txt") as f:
        (root / "Config" / "LoomProject.json").write_text(f.read().replace("{{PROJECT_NAME}}", project_name))

    with open(TEMPLATE_DIR / "CMakeLists.txt.txt") as f:
        template = f.read()
        content = (
            template
            .replace("{{ENGINE_PATH}}", engine_path)
            .replace("{{PROJECT_NAME}}", project_name)
        )
        (root / "CMakeLists.txt").write_text(content)

    with open(TEMPLATE_DIR / "CMakePresets.json.txt") as f:
        (root / "CMakePresets.json").write_text(f.read())

    with open(TEMPLATE_DIR / "UpdateEngine.bat.txt") as f:
        (root / "Scripts" / "UpdateEngine.bat").write_text(f.read())

    with open(TEMPLATE_DIR / "ReloadModules.bat.txt") as f:
        (root / "Scripts" / "ReloadModules.bat").write_text(f.read())

    # .loomproject
    (root / f"{project_name}.loomproject").write_text(f"""\
project={project_name}
config=Config/LoomProject.json
engine={engine_path}/LoomEngine
tools={tools_path}
        """)

    print(f"Created project at {root}")
    return root

def generate_game_modules(project_root, project_name):
    engine_path = Path(__file__).resolve().parents[3].as_posix()
    config_path = project_root / "Config" / "LoomProject.json"
    modules_path = project_root / "Generated" / "GameModules.cmake"

    with open(config_path) as f:
        config = json.load(f)

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

def run_cmake_configure(project_root):
    subprocess.run(["cmake", "--preset", "msvc"], cwd=project_root, check=True)

def init_project(project_name, destination_path):
    root = create_project_structure(project_name, destination_path)
    generate_game_modules(root, project_name)
    run_cmake_configure(root)
    print(f"Project '{project_name}' created and configured.")

if __name__ == "__main__":
    import sys
    if len(sys.argv) == 4 and sys.argv[1] == "init":
        init_project(sys.argv[2], sys.argv[3])
    else:
        print("Usage: python LoomCLI.py init <ProjectName> <DestinationPath>")