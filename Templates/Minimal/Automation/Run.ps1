[CmdletBinding()]
param(
    [ValidateSet('Debug','Release')][string]$Configuration = 'Debug',
    [switch]$Configure,
    [switch]$Build,
    [switch]$SmokeTest,
    [switch]$Packaged
)
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot

function Find-CMake {
    $command = Get-Command cmake -ErrorAction SilentlyContinue
    if ($command) { return $command.Source }
    $cache = Join-Path $projectRoot 'Generated/msvc/CMakeCache.txt'
    if (Test-Path -LiteralPath $cache) {
        foreach ($line in Get-Content -LiteralPath $cache) {
            if ($line -match '^CMAKE_COMMAND:INTERNAL=(.+)$' -and (Test-Path -LiteralPath $Matches[1] -PathType Leaf)) { return $Matches[1] }
        }
    }
    $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
    if (Test-Path -LiteralPath $vswhere) {
        $bundled = & $vswhere -latest -products '*' -find 'Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe'
        foreach ($path in $bundled) { if (Test-Path -LiteralPath $path -PathType Leaf) { return $path } }
    }
    throw 'CMake was not found. Install CMake or the Visual Studio C++ CMake tools.'
}

Push-Location $projectRoot
try {
    if ($Configure -or $Build) { $cmake = Find-CMake }
    if ($Configure) {
        $preset = 'msvc'
        $personalPresets = Join-Path $projectRoot 'CMakeUserPresets.json'
        if (Test-Path -LiteralPath $personalPresets) {
            $personal = Get-Content -LiteralPath $personalPresets -Raw | ConvertFrom-Json
            if (@($personal.configurePresets | Where-Object { $_.name -eq 'loom-local' }).Count) { $preset = 'loom-local' }
        }
        & $cmake --preset $preset
        if ($LASTEXITCODE -ne 0) { throw 'CMake configure failed.' }
    }
    if ($Build) {
        & $cmake --build --preset "msvc-$($Configuration.ToLowerInvariant())"
        if ($LASTEXITCODE -ne 0) { throw 'CMake build failed.' }
    }
    $arguments = @()
    if ($SmokeTest) { $arguments += '--loom-smoke-test' }
    if ($Packaged) { $arguments += '--loom-packaged' }
    $executable = Join-Path $projectRoot "Generated/msvc/$Configuration/__LOOM_NAME__.exe"
    if (-not (Test-Path -LiteralPath $executable)) { throw 'Executable missing. Double-click Automation/BuildAndRunDebug.bat or BuildAndRunRelease.bat first.' }
    & $executable @arguments
    if ($LASTEXITCODE -ne 0) { throw "Application failed with exit code $LASTEXITCODE" }
} finally { Pop-Location }
