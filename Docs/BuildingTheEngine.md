# Building the Engine

For working on Loom itself, and for setting the repository up on a new machine.

---

## Prerequisites

| Tool | Notes |
|---|---|
| Visual Studio 2022 Build Tools | MSVC v143. The presets pin this generator explicitly. |
| CMake 3.25+ | Bundled with CLion, or install standalone. |
| vcpkg | Needed to build the engine. Not needed to *use* an installed one. |
| Git | With Git LFS if you intend to commit binary assets. |

### Set up vcpkg

```powershell
git clone https://github.com/microsoft/vcpkg C:\dev\vcpkg
C:\dev\vcpkg\bootstrap-vcpkg.bat
[Environment]::SetEnvironmentVariable('VCPKG_ROOT','C:\dev\vcpkg','User')
```

**Open a new terminal afterwards.** `CMakePresets.json` reads `$env{VCPKG_ROOT}`
at configure time, and running processes do not see newly set variables.

Dependencies are declared in `vcpkg.json` and installed automatically during
CMake configure — you never run `vcpkg install` by hand.

---

## Clone and build

```powershell
git clone https://github.com/RebeccaSharkey/Loom.git
cd Loom
```

```powershell
.\Tools\Build.bat
```

That configures and builds both Debug and Release. Artifacts land in `Build\`.

The first configure is slow: vcpkg builds SDL3 from source. Later configures
reuse the binary cache and take seconds.

To build a single configuration:

```powershell
.\Tools\Build.ps1 -Config Debug
```

---

## Install your changes

Building does **not** update the engine that your games use. An install is a
snapshot, so games keep seeing the old engine until you install again.

```powershell
.\Tools\Install.bat
```

This does a **clean install**: it deletes the target folder, builds both
configurations, installs them, and sets `LOOM_ROOT`. Press Enter at the prompt to
reuse your current install location.

The installer refuses to delete a folder that is not an existing Loom install, so
a mistyped path cannot destroy unrelated files.

Avoid `C:\Program Files` — it requires administrator rights on every install, and
you will be doing this often.

---

## Repository layout

```
Loom/
├── CMake/                LoomConfig.cmake.in, LoomProject.cmake (shipped in the install)
├── Docs/                 This documentation
├── LoomEngine/
│   ├── LoomCore/         Application, window, input, events, log, math
│   └── LoomRender2D/     2D renderer (stub)
├── Sandbox/              Test application for engine development
├── Tests/
│   └── InstallSmokeTest/ Builds against the INSTALLED engine, run by CI
├── Tools/                Build.bat, Install.bat and their PowerShell scripts
├── CMakePresets.json     Generator, toolchain and triplet
└── vcpkg.json            Dependency manifest
```

`Sandbox` builds as part of the engine and is for trying things out. It is never
installed.

---

## Adding a module

1. Create `LoomEngine/LoomYourModule/` with `Include/` and `Source/`.
2. Give it a `CMakeLists.txt` that mirrors `LoomRender2D`: a `STATIC` library,
   a `Loom::YourModule` alias, `set_target_properties(... EXPORT_NAME YourModule)`,
   and `$<BUILD_INTERFACE>` / `$<INSTALL_INTERFACE:include/LoomYourModule>`.
3. Add it to `LoomEngine/CMakeLists.txt`.
4. In the root `CMakeLists.txt`, add install rules following the existing pattern:
   its own `EXPORT LoomYourModuleTargets`, its own header install destination.

`LoomConfig.cmake` discovers modules by looking for `Loom<Name>Targets.cmake`, so
no change is needed there — a game can request `COMPONENTS YourModule` as soon as
the module installs.

### Link visibility

If a module's **public headers** include another module's headers, link it
`PUBLIC` so the include directory propagates. If it is only used inside `.cpp`
files, use `PRIVATE` — that is why SDL3 is `PRIVATE` on `LoomCore` and invisible
to games.

---

## Conventions that matter

**Never include an SDL header from anything under `Include/`.** Third-party
headers must stay in `Source/`. This is what keeps games from needing to know
what the engine is built on.

**`CMAKE_DEBUG_POSTFIX` is load-bearing.** Debug libraries get a `d` suffix so
they can coexist with Release in one install. Remove it and the second install
silently overwrites the first, producing `_ITERATOR_DEBUG_LEVEL` link errors in
consuming projects.

**Static libraries only.** CI fails if any `.dll` or `.exp` appears under
`Build\`.

---

## Releasing

Releases are produced by `.github/workflows/release.yml`, triggered by a version
tag:

```powershell
git tag v0.1.0
git push origin v0.1.0
```

The workflow builds, installs, verifies the result against the smoke test, then
attaches `Loom-v0.1.0-win64.zip` to the GitHub Release. Nothing is published
unless a tag is pushed.

---

## Build state gets stale

Most confusing failures in this project have turned out to be caches, not code.
If behaviour does not match what you just wrote, suspect the build tree first:

```powershell
cmake --preset msvc --fresh
```

Changing a toolchain file, triplet, generator or compiler **requires** a fresh
tree — those are read once when a build directory is first configured and are
ignored on later runs.

The same applies to installs: editing engine source changes nothing for games
until you run `Tools\Install.bat` again.
