import os
import json
import shutil
import subprocess
from pathlib import Path

TEMPLATE_DIR = Path(__file__).parent.parent / "templates"

def create_project_structure(project_name, destination_path):
    root = Path(destination_path).resolve()
    (root / "Config").mkdir(parents=True, exist_ok=True)
    (root / "Source").mkdir(parents=True, exist_ok=True)
    (root / "Assets").mkdir(parents=True, exist_ok=True)
    (root / "Generated").mkdir(parents=True, exist_ok=True)
    (root / "Build").mkdir(parents=True, exist_ok=True)
    (root / "Scripts").mkdir(parents=True, exist_ok=True)

    with open(TEMPLATE_DIR / "main.cpp.txt") as f:
        (root / "Source" / "main.cpp").write_text(f.read().replace("{{PROJECT_NAME}}", project_name))

    with open(TEMPLATE_DIR / "LoomProject.json.txt") as f:
        (root / "Config" / "LoomProject.json").write_text(f.read().replace("{{PROJECT_NAME}}", project_name))

    with open(TEMPLATE_DIR / "CMakeLists.txt.txt") as f:
        (root / "CMakeLists.txt").write_text(f.read().replace("{{PROJECT_NAME}}", project_name))

    with open(TEMPLATE_DIR / "CMakePresets.json.txt") as f:
        (root / "CMakePresets.json").write_text(f.read())

    with open(TEMPLATE_DIR / "UpdateEngine.bat.txt") as f:
        (root / "Scripts" / "UpdateEngine.bat").write_text(f.read())

    with open(TEMPLATE_DIR / "ReloadModules.bat.txt") as f:
        (root / "Scripts" / "ReloadModules.bat").write_text(f.read())

    engine_path = Path(__file__).resolve().parents[3].as_posix()
    tools_path = Path(__file__).resolve().parents[2].as_posix()

    # .loomproject
    (root / f"{project_name}.loomproject").write_text(f"""\
project={project_name}
config=Config/LoomProject.json
engine={engine_path}/Loom
tools={tools_path}
        """)

    print(f"Created project at {root}")
    return root

def generate_game_modules(project_root, project_name):
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
        f'target_link_libraries({project_name} PRIVATE LoomEngine)'
    ]

    if enabled_modules.get("Render2D"):
        lines.append(f'set(LOOM_ENABLE_RENDERER2D ON CACHE BOOL "Enable Renderer2D" FORCE)')
        lines.append(f'target_link_libraries({project_name} PRIVATE LoomRender2D)')
    else:
        lines.append(f'set(LOOM_ENABLE_RENDERER2D OFF CACHE BOOL "Enable Renderer2D" FORCE)')

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