# Installing the Engine

This guide covers installing the engine, and making sure you can build against it.

---

Two options. Pick one.

---

### Option A — download a release (no build required)

1. Download the latest `Loom-<version>-win64.zip` from the
   [Releases page](https://github.com/RebeccaSharkey/Loom/releases).
2. Extract it somewhere permanent, for example `C:\Loom`.
3. Set a `LOOM_ROOT` environment variable pointing at that folder:

   ```powershell
   [Environment]::SetEnvironmentVariable('LOOM_ROOT','C:\Loom','User')
   ```

4. Open a new terminal. Environment variables do not reach programs that are
   already running, so any terminal or IDE open at this point will not see it.

5. To update the engine, delete old files and extract new release into the directory.

### Option B — build from source

Follow [Building the Engine](BuildingTheEngine.md). 

`Tools\Install.bat` builds the engine, installs it, and sets `LOOM_ROOT` for you.

### Verify

In a new terminal:

```powershell
echo $env:LOOM_ROOT
```

You should see your install path, and that folder should contain `include`,
`lib` and `third-party`.

---