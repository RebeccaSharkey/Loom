Set-StrictMode -Version Latest

function Get-LoomVersion {
    [CmdletBinding()]
    param([Parameter(Mandatory)][string]$RepoRoot)
    $text = [IO.File]::ReadAllText((Join-Path $RepoRoot 'CMakeLists.txt'))
    $matches = [regex]::Matches($text, '(?m)^[ \t]*project\(Loom\b[^\r\n]*?\bVERSION[ \t]+([0-9]+\.[0-9]+\.[0-9]+)\b')
    if ($matches.Count -ne 1) { throw 'Expected exactly one Loom project version in CMakeLists.txt.' }
    return $matches[0].Groups[1].Value
}

function Set-LoomVersion {
    [CmdletBinding()]
    param(
        [Parameter(Mandatory)][string]$RepoRoot,
        [Parameter(Mandatory)][AllowEmptyString()][string]$Version
    )
    $ErrorActionPreference = 'Stop'
    $RepoRoot = [IO.Path]::GetFullPath($RepoRoot)
    $current = Get-LoomVersion -RepoRoot $RepoRoot
    $Version = $Version.Trim()
    if ($Version -notmatch '^(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)$') {
        throw [ArgumentException]::new('Enter a version as major.minor.patch, without a v prefix, leading zeros or suffixes.')
    }
    try { $parsed = [version]::Parse($Version) }
    catch { throw [ArgumentException]::new('Version components must be valid non-negative 32-bit numbers.') }
    if ($parsed -le [version]::Parse($current)) {
        throw [ArgumentException]::new("The new version must be greater than $current. You entered $Version.")
    }

    $number = '[0-9]+\.[0-9]+\.[0-9]+'
    $rules = [ordered]@{
        'CMakeLists.txt' = @('(?m)^([ \t]*project\(Loom\b[^\r\n]*?\bVERSION[ \t]+)(' + $number + ')([^\r\n]*\))')
        'vcpkg.json' = @('("version"\s*:\s*")(' + $number + ')(")')
        'Templates/Minimal/Project.loomproject' = @('("EngineVersion"\s*:\s*")(' + $number + ')(")')
        'Docs/MakingAGame.md' = @('("EngineVersion"\s*:\s*")(' + $number + ')(")')
        'Docs/BuildingTheEngine.md' = @(
            ('(?m)^([ \t]*git[ \t]+tag[ \t]+v)(' + $number + ')([ \t]*\r?$)'),
            ('(?m)^([ \t]*git[ \t]+push[ \t]+origin[ \t]+v)(' + $number + ')([ \t]*\r?$)'),
            ('(Loom-v)(' + $number + ')(-win64\.zip)')
        )
    }
    $plans = New-Object 'Collections.Generic.List[object]'
    foreach ($relative in $rules.Keys) {
        $path = Join-Path $RepoRoot $relative
        $bytes = [IO.File]::ReadAllBytes($path)
        $text = [IO.File]::ReadAllText($path)
        foreach ($pattern in $rules[$relative]) {
            $regex = [regex]$pattern
            if ($regex.Matches($text).Count -ne 1) {
                throw "Expected exactly one version reference matching the known format in $relative. No files were changed."
            }
            $text = $regex.Replace($text, ('${1}' + $Version + '${3}'))
        }
        if ($relative -eq 'vcpkg.json' -or $relative -like '*.loomproject') {
            $null = ConvertFrom-Json -InputObject $text -ErrorAction Stop
        }
        $hasBom = $bytes.Length -ge 3 -and $bytes[0] -eq 239 -and $bytes[1] -eq 187 -and $bytes[2] -eq 191
        $encoding = New-Object Text.UTF8Encoding $hasBom
        $plans.Add([pscustomobject]@{
            Relative = $relative; Path = $path; Original = $bytes; Text = $text; Encoding = $encoding
            Fingerprint = [Convert]::ToBase64String($bytes)
        })
    }

    $backupRoot = Join-Path $RepoRoot ('Generated/VersionUpdates/' + [guid]::NewGuid().ToString('N'))
    New-Item -ItemType Directory -Path $backupRoot -Force | Out-Null
    foreach ($plan in $plans) {
        $staged = Join-Path $backupRoot ('Staged/' + $plan.Relative)
        New-Item -ItemType Directory -Path (Split-Path -Parent $staged) -Force | Out-Null
        New-Item -ItemType Directory -Path (Split-Path -Parent (Join-Path $backupRoot ('Original/' + $plan.Relative))) -Force | Out-Null
        [IO.File]::WriteAllText($staged, $plan.Text, $plan.Encoding)
    }
    $committed = New-Object 'Collections.Generic.List[object]'
    try {
        foreach ($plan in $plans) {
            if ([Convert]::ToBase64String([IO.File]::ReadAllBytes($plan.Path)) -ne $plan.Fingerprint) {
                throw "File changed during version preparation: $($plan.Relative)"
            }
            $staged = Join-Path $backupRoot ('Staged/' + $plan.Relative)
            $backup = Join-Path $backupRoot ('Original/' + $plan.Relative)
            [IO.File]::Replace($staged, $plan.Path, $backup)
            $committed.Add($plan)
        }
    } catch {
        $failure = $_.Exception.Message
        $rollbackErrors = @()
        for ($index = $committed.Count - 1; $index -ge 0; $index--) {
            $plan = $committed[$index]
            try {
                if ([IO.File]::ReadAllText($plan.Path) -ne $plan.Text) { throw 'File edited after replacement; preserving that edit.' }
                [IO.File]::Copy((Join-Path $backupRoot ('Original/' + $plan.Relative)), $plan.Path, $true)
            } catch { $rollbackErrors += "$($plan.Relative): $($_.Exception.Message)" }
        }
        if ($rollbackErrors.Count) { throw "Version update failed: $failure. Some files could not be restored: $($rollbackErrors -join '; '). Backups: $backupRoot" }
        throw "Version update failed; original files restored. $failure"
    }
    [pscustomobject]@{ PreviousVersion = $current; Version = $Version; Files = @($rules.Keys); BackupDirectory = (Join-Path $backupRoot 'Original') }
}

Export-ModuleMember -Function Get-LoomVersion, Set-LoomVersion
