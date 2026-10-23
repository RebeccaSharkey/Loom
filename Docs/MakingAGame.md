# Making a Game with Loom

Games live outside the engine checkout and consume the installed SDK. Phase 0
provides an empty window, content-path proof and normal startup/shutdown.
Rendering and gameplay belong to later phases.

## Prerequisites

- Windows 10/11 x64 and PowerShell 5.1+ (built into Windows).
- CMake 3.25+; Visual Studio 2026 generators require CMake 4.2+.
- Visual Studio C++ Build Tools matching the engine installation. Current local
  builds use VS 2026; CI/release builds use the compiler on windows-2025.
- The DLL runtime: /MDd for Debug, /MD for Release. The generated project sets it.
- An installed Loom SDK. Games need neither vcpkg nor an SDL declaration.

The package checks compiler version, pointer size, runtime and engine version.
Use a source-built SDK if the downloaded SDK targets a different compiler.
Run `cmake --version` and ensure cmake is on PATH (or use CLion's bundled CMake).

## Create a project

Install or rebuild the SDK using [Installing the Engine](InstallingTheEngine.md)
and [Building the Engine](BuildingTheEngine.md), then open a fresh terminal.

```powershell
& "$env:LOOM_ROOT/tools/ProjectWorkflow/New-LoomProject.ps1" `
  -Name MyGame -DisplayName 'My Game' -Destination 'C:/Games/My Game' `
  -EngineRoot $env:LOOM_ROOT -Modules Render2D
```

The parent directory must exist. The final destination must not exist, even if
empty. The script prints a JSON result with Descriptor, Directory and
TemplateVersion; failures return a nonzero status. Omit Modules for a Core-only
project. Render2D is an optional stub, not a working renderer.

For the launcher, double-click the installed
`tools/ProjectWorkflow/New-LoomProjectGui.bat`, or run:

```powershell
& "$env:LOOM_ROOT/tools/ProjectWorkflow/New-LoomProjectGui.bat"
```

Enter a code identifier, display name, final destination and engine installation.
Browse for a parent directory to fill destination with parent/code-identifier.
Review the final path and choose Create. The launcher uses the same generator.
After creation, choose Debug/Release and Build, Run, or Open Folder. Builds run
in the background; output and status appear in the launcher. Build includes a
short startup/content/shutdown test. Run leaves the window open until you close it.

Names start with a letter and contain letters, digits or underscores. Windows
device names and CMake target names such as install/clean are reserved. Display
names can contain spaces and quotes, but not semicolons/newlines/template tokens.
Generation validates and stages all files before publishing a directory. It
never merges into or overwrites an existing project and needs no compiler.

## Configure, build and run

Open your game's **Automation** folder and double-click one of these files:

| File | Action |
|---|---|
| BuildAndRunDebug.bat | Configure CMake, build Debug, then launch |
| BuildAndRunRelease.bat | Build Release, then launch |
| Run.bat | Launch the existing Debug executable |
| SmokeTest.bat | Launch Debug, read starter content, then shut down |
| RunPackaged.bat | Launch Debug using content copied beside the executable |

Run BuildAndRunDebug.bat first for a new project. Console windows stay open at
completion so output and errors are readable. You do not need to open PowerShell
or type commands. The files call Automation/Run.ps1 internally; the helper can
find CMake on PATH, in the existing build cache or in Visual Studio's CMake tools.
They work regardless of the folder from which you launch them.

The launcher GUI and CMake/IDE workflows use the same project. Advanced users can
still configure with `cmake --preset loom-local` (or `msvc` with LOOM_ROOT) and
build with `cmake --build --preset msvc-debug` / `msvc-release`.

## Engine selection and updates

Selection precedence is:

1. LOOM_ENGINE_ROOT in the CMake cache, normally supplied by the loom-local user preset.
2. LOOM_ROOT from the environment when no explicit selection is defined.

An invalid explicit choice fails; it never falls back to a different installation.
The generator writes a personal preset when EngineRoot is supplied:

```json
{
  "version": 3,
  "configurePresets": [
    {
      "name": "loom-local",
      "inherits": "msvc",
      "binaryDir": "${sourceDir}/Generated/msvc",
      "cacheVariables": { "LOOM_ENGINE_ROOT": "C:/SDKs/Loom" }
    }
  ]
}
```

The personal loom-local preset inherits shared msvc settings and uses the same
build directory. Use cmake --preset loom-local when this preset exists; use
cmake --preset msvc with the environment default. The Automation helper chooses automatically.
The personal file is ignored. Ordinary commands also work:

```powershell
cmake -S . -B Generated/msvc -A x64 -DLOOM_ENGINE_ROOT='C:/SDKs/Loom'
cmake --build Generated/msvc --config Debug
```

If multiple VS versions are installed, add a matching generator to the local
preset, or use `-G 'Visual Studio 17 2022'` / `-G 'Visual Studio 18 2026'` in a
fresh build directory. The package reports compiler mismatches explicitly.

Install another compatible SDK, change the local selection, then reconfigure.
CMake prints the chosen package path and replaces cached Loom_DIR. If switching
back to the environment default, remove the local setting and run
`cmake --preset msvc -U LOOM_ENGINE_ROOT`. Changing LOOM_ROOT alone does not
override an explicit cached choice. Use a fresh build directory after changing
compiler/toolchain/runtime. Source edits survive engine updates; never rerun the
generator over an existing game. API changes require explicit source migration.

## Descriptor and paths

See [descriptor schema](LoomProject.schema.json) and
[decision 001](Decisions/001-ProjectWorkflow.md). Required fields:

```json
{
  "FileVersion": 1,
  "EngineVersion": "0.1.0",
  "Name": "MyGame",
  "DisplayName": "My Game",
  "ContentRoot": "Content",
  "Modules": ["Render2D"],
  "TemplateVersion": 1
}
```

Core is implicit. Modules is the only optional-module list; the helper validates
names and links the corresponding installed targets. Editing the descriptor
triggers reconfiguration on a normal build. Invalid JSON, required fields,
schema versions and module names fail with descriptor/field diagnostics.
EngineVersion requires at least that patch in the same major/minor series.
This version policy does not promise arbitrary compiler or future ABI compatibility.
Name matches the CMake project/target identifier; edit DisplayName for title changes.

Development ContentRoot is relative to the descriptor. The build copies its
contents to the same relative directory beside the executable. In packaged mode,
that executable directory is the base. Both support spaces and unrelated working
directories. The template reads welcome.txt through Loom::Paths, without SDL calls.
Missing content reports its resolved path and returns a failure status.

Logs, saves and settings use SDL's per-user preference directory, normally
`%APPDATA%/Loom/<Name>/Logs`, `Saves` and `Settings`. Startup prints the resolved
content/data/log paths. Failure to create writable directories is reported.
The engine never changes the process working directory.

## Files and acceptance checks

Commit source, descriptor, Content, shared presets, Automation and README. Ignore
Generated/Build, personal presets, IDE state and writable data. The generated
header is in the build tree; normal builds do not rewrite game source.
TemplateVersion identifies the creation format; existing projects are not
silently migrated when templates change.

The source repository provides the same check used by CI:

```powershell
.\Tests\ProjectWorkflow\Test-ProjectWorkflow.ps1 -EngineRoot $env:LOOM_ROOT `
  -RunApplications -RunGui
```

It generates separate projects, builds both configurations, checks paths with
spaces, invalid inputs, regeneration, GUI/CLI equivalence and source preservation
when switching SDK installations. Artifacts and JSON/log evidence are kept under
Generated/WorkflowAcceptance. CI runs applications with SDL's dummy driver;
that proves lifecycle and paths, not visible-window rendering. Local desktop
runs use the normal driver. See [Phase 0 acceptance](Phase0Acceptance.md).
