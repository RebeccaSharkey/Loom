[CmdletBinding()]
param([string]$Version, [string]$RepoRoot)
$ErrorActionPreference = 'Stop'
if (-not $RepoRoot) { $RepoRoot = Split-Path -Parent $PSScriptRoot }
$interactive = -not $PSBoundParameters.ContainsKey('Version')
try {
    Import-Module (Join-Path $PSScriptRoot 'Loom.Version.psm1') -Force
    Write-Host '=== Loom Version Update ===' -ForegroundColor Cyan
    Write-Host ('Current engine version: ' + (Get-LoomVersion -RepoRoot $RepoRoot))
    while ($true) {
        $candidate = if ($interactive) { Read-Host 'New version (major.minor.patch)' } else { $Version }
        try {
            $result = Set-LoomVersion -RepoRoot $RepoRoot -Version $candidate
            break
        } catch [ArgumentException] {
            if (-not $interactive) { throw }
            Write-Host $_.Exception.Message -ForegroundColor Red
            Write-Host 'Please try again.'
        }
    }
    Write-Host "Updated Loom from $($result.PreviousVersion) to $($result.Version)." -ForegroundColor Green
    foreach ($file in $result.Files) { Write-Host "  $file" }
    Write-Host "Original files: $($result.BackupDirectory)"
    Write-Host 'Run Install.bat to update your local SDK. Commit and release when ready.'
} catch {
    [Console]::Error.WriteLine($_.Exception.Message)
    exit 1
}
