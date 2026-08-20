[CmdletBinding()]
param(
    [string]$Prefix,   # optional, for scripted use later (CI)
    [switch]$Yes
)

$ErrorActionPreference = 'Stop'
$RepoRoot = Split-Path -Parent $PSScriptRoot
$BuildDir = Join-Path $RepoRoot "Generated\msvc"

function Assert-SafeTarget([string]$Path) {
    if ([string]::IsNullOrWhiteSpace($Path)) { throw "Install path is empty." }
    $full = [IO.Path]::GetFullPath($Path)
    if ($full -match '^[A-Za-z]:\\?$') { throw "Refusing to use a drive root: $full" }
    if (Test-Path $full) {
        $marker  = Join-Path $full "lib\cmake\Loom"
        $isEmpty = -not (Get-ChildItem -Force $full -ErrorAction SilentlyContinue)
        if (-not (Test-Path $marker) -and -not $isEmpty) {
            throw "$full exists and is not a Loom install. Refusing to delete it.`nChoose an empty folder or a previous Loom install."
        }
    }
    return $full
}

Write-Host "=== Loom Engine Installer ===" -ForegroundColor Cyan
Write-Host ""

if (-not $Prefix) {
    $default = if ($env:LOOM_ROOT) { $env:LOOM_ROOT } else { "C:\Loom" }
    Write-Host "Where should Loom be installed?"
    Write-Host "  Press Enter for: $default"
    $entered = Read-Host "Path"
    $Prefix = if ([string]::IsNullOrWhiteSpace($entered)) { $default } else { $entered }
}

$Prefix = Assert-SafeTarget $Prefix
Write-Host ""
Write-Host "Installing to: $Prefix"

if (Test-Path $Prefix) {
    Write-Host "Existing install found - it will be DELETED first." -ForegroundColor Yellow
    if (-not $Yes) {
        if ((Read-Host "Continue? [y/N]") -ne 'y') { Write-Host "Aborted."; exit 1 }
    }
    Remove-Item -Recurse -Force $Prefix
}

Push-Location $RepoRoot
try {
    cmake --preset msvc
    if ($LASTEXITCODE -ne 0) { throw "Configure failed." }

    foreach ($cfg in @('Debug','Release')) {
        Write-Host ""
        Write-Host "--- $cfg ---" -ForegroundColor Cyan
        cmake --build --preset "msvc-$($cfg.ToLower())"
        if ($LASTEXITCODE -ne 0) { throw "Build ($cfg) failed." }
        cmake --install $BuildDir --prefix $Prefix --config $cfg
        if ($LASTEXITCODE -ne 0) { throw "Install ($cfg) failed." }
    }
}
finally { Pop-Location }

[Environment]::SetEnvironmentVariable('LOOM_ROOT', $Prefix, 'User')
$env:LOOM_ROOT = $Prefix

Write-Host ""
Write-Host "Loom installed to $Prefix" -ForegroundColor Green
Write-Host "LOOM_ROOT set for your user account."
Write-Host "Open a NEW terminal (and restart CLion) for it to take effect." -ForegroundColor Yellow