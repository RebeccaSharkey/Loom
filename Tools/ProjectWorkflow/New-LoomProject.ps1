[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$Name,
    [Parameter(Mandatory)][string]$Destination,
    [string]$DisplayName,
    [string]$EngineRoot,
    [string[]]$Modules = @(),
    [string]$TemplatePath
)
$ErrorActionPreference = 'Stop'
try {
    Import-Module (Join-Path $PSScriptRoot 'Loom.Project.psm1') -Force
    New-LoomProject @PSBoundParameters | ConvertTo-Json -Compress
} catch {
    [Console]::Error.WriteLine($_.Exception.Message)
    exit 1
}
