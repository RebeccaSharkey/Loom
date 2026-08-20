# Making a Game with Loom

This guide covers installing the engine and creating a game project. Your game
lives wherever you like — it does not go inside the engine repository.

---

## 1. Install the engine

Two options. Pick one.

### Option A — download a release (no build required)

1. Download `Loom-<version>-win64.zip` from the
   [Releases page](https://github.com/RebeccaSharkey/Loom/releases).
2. Extract it somewhere permanent, for example `C:\Loom`.
3. Set a `LOOM_ROOT` environment variable pointing at that folder:

   ```powershell
   [Environment]::SetEnvironmentVariable('LOOM_ROOT','C:\Loom','User')
   ```

4. **Open a new terminal.** Environment variables do not reach programs that are
   already running, so any terminal or IDE open at this point will not see it.

### Option B — build from source

Follow [Building the Engine](BuildingTheEngine.md). `Tools\Install.bat` builds
the engine, installs it, and sets `LOOM_ROOT` for you.

### Verify

In a new terminal:

```powershell
echo $env:LOOM_ROOT
```

You should see your install path, and that folder should contain `include`,
`lib` and `third-party`.

---

## 2. Create your project

A game needs four things.

### `MyGame.loomproject`

The project descriptor. `Modules` lists the optional engine modules you want;
`Loom::Core` is always available and does not need listing.

```json
{
  "FileVersion": 1,
  "EngineVersion": "0.1.0",
  "Name": "MyGame",
  "Modules": [ "Render2D" ]
}
```

### `CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.25)
project(MyGame LANGUAGES CXX)

# Read the descriptor to find out which engine modules this game uses.
include("$ENV{LOOM_ROOT}/lib/cmake/Loom/LoomProject.cmake")
loom_read_project("${CMAKE_CURRENT_SOURCE_DIR}/MyGame.loomproject" LOOM_MODULES)

find_package(Loom REQUIRED COMPONENTS ${LOOM_MODULES})

add_executable(MyGame Source/main.cpp)

target_link_libraries(MyGame PRIVATE Loom::Core)
foreach(_mod IN LISTS LOOM_MODULES)
    target_link_libraries(MyGame PRIVATE Loom::${_mod})
endforeach()
```

Editing `MyGame.loomproject` automatically re-runs CMake on your next build, so
adding a module takes effect without any manual reconfiguration.

### `CMakePresets.json`

```json
{
  "version": 3,
  "cmakeMinimumRequired": { "major": 3, "minor": 25 },
  "configurePresets": [
    {
      "name": "msvc",
      "displayName": "Visual Studio 2022 (x64)",
      "generator": "Visual Studio 17 2022",
      "architecture": "x64",
      "binaryDir": "${sourceDir}/Generated/${presetName}",
      "cacheVariables": { "CMAKE_PREFIX_PATH": "$env{LOOM_ROOT}" }
    }
  ],
  "buildPresets": [
    { "name": "msvc-debug",   "configurePreset": "msvc", "configuration": "Debug" },
    { "name": "msvc-release", "configurePreset": "msvc", "configuration": "Release" }
  ]
}
```

The generator is pinned deliberately. If you have more than one Visual Studio
installed, CMake otherwise picks the newest — and linking a VS 2022-built engine
into a VS 2026-built game produces link errors that look unrelated to the cause.

### `Source/main.cpp`

```cpp
#include "Log/Log.h"

int main()
{
    Loom::Log::Init();
    LOOM_LOG_NOTICE("MyGame", "Hello from Loom");
    Loom::Log::Flush();
    Loom::Log::Shutdown();
    return 0;
}
```

---

## 3. Build and run

```powershell
cmake --preset msvc
cmake --build --preset msvc-debug
```

In CLion, the preset is detected automatically — enable it under
**Settings → Build, Execution, Deployment → CMake** and disable CLion's own
profiles, which do not know about `LOOM_ROOT`.

---

## Notes

**You never mention SDL3, or vcpkg.** The engine bundles its dependencies inside
its install, so a game only ever names Loom. If you find yourself needing to
declare an engine dependency, that is a bug in the engine's packaging.

**Modules are enforced at compile time.** Including a header from a module you
have not listed fails to compile, rather than producing a confusing link error
later.

**Logs are written relative to the working directory**, so they appear wherever
you launch the executable from — which in CLion is the project root.

---

## Troubleshooting

| Symptom | Cause |
|---|---|
| `Could not find a package configuration file provided by "Loom"` | `LOOM_ROOT` is unset, or the terminal predates it being set. Open a new terminal. |
| `Target "Loom::Render2D" not found` | The module is missing from your `.loomproject` `Modules` array. |
| `LNK2038: mismatch detected for 'RuntimeLibrary'` | Debug and Release are being mixed. Build your game in the same configuration you are linking. |
| `Cannot open include file` for an engine header | The module owning it is not listed in your descriptor. |
| Changes to the engine are not visible | An installed engine is a snapshot. Re-run `Tools\Install.bat` in the engine repo. |
