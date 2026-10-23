# Loom Engine

[![Build](https://github.com/RebeccaSharkey/Loom/actions/workflows/build.yml/badge.svg)](https://github.com/RebeccaSharkey/Loom/actions/workflows/build.yml)

**Loom** is a lightweight 2D game engine written in modern C++. It is built from
scratch, stays small on purpose, and is designed so that games live in their own
repositories and consume the engine as an installed package — the way you would
use any other SDK.

> *Weave your worlds with speed and precision.*

---

## Status

**Early development.** The engine is not yet capable of drawing anything.

What works today:

- Windowing and a fixed application lifecycle (Win32)
- Event system with typed events and scoped subscriptions
- Logging with console and file sinks
- Install and packaging: games can consume an installed Loom from anywhere on disk
- Validated project descriptors, generator and Windows new-project launcher
- Content paths independent of working directory and per-user writable data

What does not exist yet: the renderer, audio, asset loading, scripting.

---

## Documentation

| Guide                                                | For                                                         |
|------------------------------------------------------|-------------------------------------------------------------|
| [Installing the Engine](Docs/InstallingTheEngine.md) | For Installing the latest release of the engine             |
| [Building the Engine](Docs/BuildingTheEngine.md)     | For working on Loom itself, and installing your own changes |
| [Making a Game](Docs/MakingAGame.md)                 | Installing Loom and starting a game project                 |


---

## Modules

Loom is split into modules. `Core` is always present; everything else is opt-in
through your project's `.loomproject` descriptor.

| Module | Contents |
|---|---|
| `Loom::Core` | Application, window events, logging, paths, math, time |
| `Loom::Render2D` | 2D rendering (stub — not yet implemented) |

---

## Requirements

- Windows 10/11
- Visual Studio C++ Build Tools matching the installed SDK (Windows x64)
- CMake 3.25 or newer; 4.2+ for Visual Studio 2026
- PowerShell 5.1+ for project creation and the launcher
- [vcpkg](https://github.com/microsoft/vcpkg) — only needed to *build* the engine,
  not to use an installed one

---

## Third-party dependencies

Bundled into the engine install, so games never declare them.

| Library | Purpose | Licence |
|---|---|---|
| [SDL3](https://www.libsdl.org/) | Window, input, platform, audio backend | Zlib |

---

## Licence

Loom is licensed under the [Apache 2.0 License](LICENSE).
