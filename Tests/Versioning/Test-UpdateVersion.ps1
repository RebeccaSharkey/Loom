[CmdletBinding()]
param()
$ErrorActionPreference = 'Stop'
$repo = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '../..'))
Import-Module (Join-Path $repo 'Tools/Loom.Version.psm1') -Force
$sourceVersion = Get-LoomVersion -RepoRoot $repo
$files = @('CMakeLists.txt','vcpkg.json','Templates/Minimal/Project.loomproject','Docs/MakingAGame.md','Docs/BuildingTheEngine.md')
$testRoot = Join-Path $repo ('Generated/VersionTests/' + [guid]::NewGuid().ToString('N'))

function Check([bool]$condition, [string]$label) {
    if (-not $condition) { throw "FAIL: $label" }
    Write-Host "PASS: $label"
}
function New-Fixture([string]$name) {
    $root = Join-Path $testRoot $name
    foreach ($relative in $files) {
        $target = Join-Path $root $relative
        New-Item -ItemType Directory -Force -Path (Split-Path -Parent $target) | Out-Null
        $text = [IO.File]::ReadAllText((Join-Path $repo $relative)).Replace($sourceVersion, '0.1.2')
        [IO.File]::WriteAllText($target, $text, (New-Object Text.UTF8Encoding ($relative -eq 'vcpkg.json')))
    }
    return $root
}
function Snapshot([string]$root) {
    $hashes = @{}
    foreach ($relative in $files) { $hashes[$relative] = (Get-FileHash -LiteralPath (Join-Path $root $relative)).Hash }
    return $hashes
}
function Unchanged([string]$root, $hashes) {
    foreach ($relative in $files) {
        if ((Get-FileHash -LiteralPath (Join-Path $root $relative)).Hash -ne $hashes[$relative]) { return $false }
    }
    return $true
}

$fixture = New-Fixture 'Versions With Spaces'
$original = Snapshot $fixture
foreach ($candidate in @('0.1.2','0.1.1','0.0.99','v0.2.0','0.2','0.2.0.1','0.02.0','-1.2.3','0.2.0-beta','','999999999999.0.0')) {
    $failed = $false
    try { Set-LoomVersion -RepoRoot $fixture -Version $candidate | Out-Null } catch [ArgumentException] { $failed = $true }
    Check ($failed -and (Unchanged $fixture $original)) "Reject '$candidate' without changing any files"
}
$game = Join-Path $fixture 'Games/Example.loomproject'
New-Item -ItemType Directory -Force -Path (Split-Path -Parent $game) | Out-Null
'{"EngineVersion":"0.1.2"}' | Set-Content -LiteralPath $game -Encoding UTF8
$gameHash = (Get-FileHash -LiteralPath $game).Hash
$result = Set-LoomVersion -RepoRoot $fixture -Version '0.1.10'
Check ((Get-LoomVersion -RepoRoot $fixture) -eq '0.1.10') 'Numeric comparison accepts patch 10 after patch 2'
Check ($result.Files.Count -eq 5) 'Exactly the five engine/documentation files are updated'
Check ((Get-Content -LiteralPath (Join-Path $fixture 'vcpkg.json') -Raw | ConvertFrom-Json).version -eq '0.1.10') 'vcpkg metadata updated'
$descriptor = Get-Content -LiteralPath (Join-Path $fixture 'Templates/Minimal/Project.loomproject') -Raw | ConvertFrom-Json
Check ($descriptor.EngineVersion -eq '0.1.10' -and $descriptor.FileVersion -eq 1 -and $descriptor.TemplateVersion -eq 1) 'Template engine requirement changes; schema/template versions stay unchanged'
Check ((Get-Content -LiteralPath (Join-Path $fixture 'Docs/MakingAGame.md') -Raw) -match '"EngineVersion": "0.1.10"') 'Game guide example updated'
$guide = Get-Content -LiteralPath (Join-Path $fixture 'Docs/BuildingTheEngine.md') -Raw
Check ($guide.Contains('git tag v0.1.10') -and $guide.Contains('git push origin v0.1.10') -and $guide.Contains('Loom-v0.1.10-win64.zip')) 'Release tag commands and archive filename updated'
Check ((Get-FileHash -LiteralPath $game).Hash -eq $gameHash) 'Existing game descriptors remain untouched'
$bytes = [IO.File]::ReadAllBytes((Join-Path $fixture 'vcpkg.json'))
Check ($bytes[0] -eq 239 -and $bytes[1] -eq 187 -and $bytes[2] -eq 191) 'Existing UTF-8 BOM is preserved'
foreach ($relative in $files) {
    Check ((Get-FileHash -LiteralPath (Join-Path $result.BackupDirectory $relative)).Hash -eq $original[$relative]) "Original bytes backed up: $relative"
}

$broken = New-Fixture 'Broken Documentation'
$guidePath = Join-Path $broken 'Docs/BuildingTheEngine.md'
[IO.File]::WriteAllText($guidePath, ([IO.File]::ReadAllText($guidePath).Replace('git tag v', 'git tag archived-v')))
$hashes = Snapshot $broken
$failed = $false
try { Set-LoomVersion -RepoRoot $broken -Version 0.2.0 | Out-Null } catch { $failed = $true }
Check ($failed -and (Unchanged $broken $hashes)) 'Unexpected documentation format aborts before changing any files'

$locked = New-Fixture 'Locked Metadata'
$hashes = Snapshot $locked
$lock = [IO.File]::Open((Join-Path $locked 'vcpkg.json'), [IO.FileMode]::Open, [IO.FileAccess]::Read, [IO.FileShare]::Read)
$failure = ''
try {
    try { Set-LoomVersion -RepoRoot $locked -Version 0.2.0 | Out-Null } catch { $failure = $_.Exception.Message }
} finally { $lock.Dispose() }
Check ($failure -match 'original files restored' -and (Unchanged $locked $hashes)) 'A write failure after the first file restores the original versions'

# Exercise the actual Windows PowerShell prompt, not a parallel test input loop.
$interactive = New-Fixture 'Interactive Prompt'
$start = New-Object Diagnostics.ProcessStartInfo
$start.FileName = Join-Path $env:WINDIR 'System32/WindowsPowerShell/v1.0/powershell.exe'
$start.Arguments = '-NoProfile -ExecutionPolicy Bypass -File "' + (Join-Path $repo 'Tools/UpdateVersion.ps1') + '" -RepoRoot "' + $interactive + '"'
$start.UseShellExecute = $false
$start.CreateNoWindow = $true
$start.RedirectStandardInput = $start.RedirectStandardOutput = $start.RedirectStandardError = $true
$process = New-Object Diagnostics.Process
$process.StartInfo = $start
try {
    [void]$process.Start()
    foreach ($answer in @('invalid','0.1.2','0.1.1','0.2.0')) { $process.StandardInput.WriteLine($answer) }
    $process.StandardInput.Close()
    if (-not $process.WaitForExit(30000)) { $process.Kill(); throw 'Version prompt test timed out.' }
    $stdout = $process.StandardOutput.ReadToEnd()
    $stderr = $process.StandardError.ReadToEnd()
    Check ($process.ExitCode -eq 0 -and ([regex]::Matches($stdout, 'Please try again.').Count -eq 3) -and (Get-LoomVersion -RepoRoot $interactive) -eq '0.2.0') 'Windows PowerShell rejects invalid/equal/lower inputs, asks again, then applies a valid version'
    if ($process.ExitCode -ne 0) { throw $stderr }
} finally { $process.Dispose() }
Check ((Get-LoomVersion -RepoRoot $repo) -eq $sourceVersion) 'The real repository version was not changed by tests'
Write-Host "Version updater acceptance passed. Evidence: $testRoot"
