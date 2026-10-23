# __LOOM_NAME__

Created with Loom template version 1. Requires Windows, PowerShell 5.1+, CMake
3.25+ and MSVC x64 matching the engine installation. VS 2026 requires CMake 4.2+.

Set `LOOM_ROOT` to your installed SDK, or put `LOOM_ENGINE_ROOT` in the
`loom-local` configure preset of an ignored `CMakeUserPresets.json` (see Loom's game guide).
An explicit local selection wins. Invalid selections fail without fallback.

Open the **Automation** folder and double-click an action:

| File | Action |
|---|---|
| BuildAndRunDebug.bat | Configure, build Debug, then launch |
| BuildAndRunRelease.bat | Build Release, then launch |
| Run.bat | Launch the existing Debug build |
| SmokeTest.bat | Launch Debug, read starter content, then exit normally |
| RunPackaged.bat | Launch Debug using content beside the executable |

Use BuildAndRunDebug.bat once before the other actions. The console stays open
after completion so you can read output and errors. No PowerShell commands are
needed; the batch files call Automation/Run.ps1 internally.

The helper chooses the personal `loom-local` preset when present, otherwise
`msvc`. It finds CMake on PATH, from an existing build cache or in Visual Studio's
CMake tools. Command-line configure/build remains available through CMake presets.

Edit the `.loomproject` to change the display title, content root or modules.
Core is implicit; the currently available optional module is Render2D (a stub).
A normal build notices descriptor edits and reruns CMake. EngineVersion is the
minimum patch version in the same major/minor series; it is not an ABI promise.

Development content is relative to the descriptor, independent of working
directory. Packaged content is relative to the executable. Per-user logs, saves
and settings are under `%APPDATA%/Loom/__LOOM_NAME__/`.

Source, descriptor, content, presets and this README belong in version control.
Generated output, personal presets and writable data do not. To update Loom,
install a compatible engine, change your local selection and reconfigure.
Keep a fresh build tree for compiler/runtime changes. Never regenerate over
an existing game: creation does not update or migrate hand-edited source.

This application proves the engine workflow. Graphics and gameplay arrive later.
