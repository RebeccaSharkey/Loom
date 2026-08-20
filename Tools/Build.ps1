# Builds the engine without installing it. Use this when working on the engine
# itself; use Install.bat when you want games to pick up your changes.

[CmdletBinding()]
param(
    [ValidateSet('Debug','Release','Both')]
    [string]$Config = 'Both'
)

$ErrorActionPreference = 'Stop'
$RepoRoot = Split-Path -Parent $PSScriptRoot

Write-Host "=== Loom Engine Build ===" -ForegroundColor Cyan

Push-Location $RepoRoot
try {
    cmake --preset msvc
    if ($LASTEXITCODE -ne 0) { throw "Configure failed." }

    $configs = if ($Config -eq 'Both') { @('Debug','Release') } else { @($Config) }
    foreach ($cfg in $configs) {
        Write-Host ""
        Write-Host "--- $cfg ---" -ForegroundColor Cyan
        cmake --build --preset "msvc-$($cfg.ToLower())"
        if ($LASTEXITCODE -ne 0) { throw "Build ($cfg) failed." }
    }
}
finally { Pop-Location }

Write-Host ""
Write-Host "Build complete. Artifacts are in Build\" -ForegroundColor Green
Write-Host "Run Tools\Install.bat to make these changes available to game projects."
