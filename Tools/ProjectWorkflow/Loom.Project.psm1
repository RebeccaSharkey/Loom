Set-StrictMode -Version Latest

function New-LoomProject {
    [CmdletBinding()]
    param(
        [Parameter(Mandatory)][string]$Name,
        [Parameter(Mandatory)][string]$Destination,
        [string]$DisplayName,
        [string]$EngineRoot,
        [string[]]$Modules = @(),
        [string]$TemplatePath = (Join-Path $PSScriptRoot '../../templates/Minimal')
    )
    $ErrorActionPreference = 'Stop'
    if ($Name -cnotmatch '^[A-Za-z][A-Za-z0-9_]*$' -or $Name -match '^(CON|PRN|AUX|NUL|COM[0-9]|LPT[0-9])$') {
        throw 'Name: use a letter followed by letters, digits or underscores; Windows device names are reserved.'
    }
    if ($Name -match '^(all|ALL_BUILD|ZERO_CHECK|RUN_TESTS|PACKAGE|install|clean|test|help|edit_cache|rebuild_cache)$') { throw 'Name: reserved CMake target name.' }
    if (-not $DisplayName) { $DisplayName = $Name }
    if ($DisplayName -match '[;\r\n]' -or $DisplayName -match '__LOOM_') { throw 'DisplayName: semicolons, newlines and template tokens are not supported.' }
    foreach ($module in $Modules) {
        if ($module -cne 'Render2D') { throw "Modules: unknown module '$module'; supported: Render2D. Core is implicit." }
    }
    if (@($Modules | Select-Object -Unique).Count -ne $Modules.Count) { throw 'Modules: duplicates are not allowed.' }
    $destinationPath = [IO.Path]::GetFullPath($Destination)
    if (Test-Path -LiteralPath $destinationPath) { throw "Destination: already exists; no files were changed: $destinationPath" }
    $parent = Split-Path -Parent $destinationPath
    if (-not (Test-Path -LiteralPath $parent -PathType Container)) { throw "Destination: parent directory must exist: $parent" }
    $templateFiles = @('CMakeLists.txt','CMakePresets.json','Project.loomproject','Source/main.cpp','Content/welcome.txt','.gitignore','README.md',
        'Automation/Run.ps1','Automation/Run.bat','Automation/BuildAndRunDebug.bat','Automation/BuildAndRunRelease.bat',
        'Automation/SmokeTest.bat','Automation/RunPackaged.bat')
    foreach ($required in $templateFiles) {
        if (-not (Test-Path -LiteralPath (Join-Path $TemplatePath $required) -PathType Leaf)) { throw "Template: missing $required in $TemplatePath" }
    }
    if ($EngineRoot) {
        $EngineRoot = [IO.Path]::GetFullPath($EngineRoot)
        if (-not (Test-Path -LiteralPath (Join-Path $EngineRoot 'lib/cmake/Loom/LoomConfigVersion.cmake'))) { throw "EngineRoot: not a current Loom installation: $EngineRoot" }
    }
    # A sibling staging directory permits an atomic final rename on this volume.
    $stage = Join-Path $parent ('.loom-new-' + [guid]::NewGuid().ToString('N'))
    try {
        New-Item -ItemType Directory -Path $stage | Out-Null
        foreach ($relative in $templateFiles) {
            $source = Join-Path $TemplatePath $relative
            $output = Join-Path $stage $relative
            New-Item -ItemType Directory -Force -Path (Split-Path -Parent $output) | Out-Null
            $text = [IO.File]::ReadAllText($source).Replace('__LOOM_NAME__', $Name)
            [IO.File]::WriteAllText($output, $text, (New-Object Text.UTF8Encoding $false))
        }
        $descriptor = Get-Content -LiteralPath (Join-Path $stage 'Project.loomproject') -Raw | ConvertFrom-Json
        $descriptor.DisplayName = $DisplayName
        $descriptor.Modules = @($Modules)
        if ($descriptor.FileVersion -ne 1 -or $descriptor.TemplateVersion -ne 1 -or $descriptor.Name -cne $Name -or
            $descriptor.EngineVersion -notmatch '^[0-9]+\.[0-9]+\.[0-9]+$' -or $descriptor.ContentRoot -cne 'Content') {
            throw 'Template: descriptor does not match the supported v1 project contract.'
        }
        $descriptorPath = Join-Path $stage "$Name.loomproject"
        $descriptor | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $descriptorPath -Encoding UTF8
        Remove-Item -LiteralPath (Join-Path $stage 'Project.loomproject')
        if ($EngineRoot) {
            @{ version = 3; configurePresets = @(@{ name = 'loom-local'; inherits = 'msvc'; binaryDir = '${sourceDir}/Generated/msvc'; cacheVariables = @{ LOOM_ENGINE_ROOT = $EngineRoot } }) } |
                ConvertTo-Json -Depth 8 | Set-Content -LiteralPath (Join-Path $stage 'CMakeUserPresets.json') -Encoding UTF8
        }
        foreach ($file in Get-ChildItem -LiteralPath $stage -Recurse -File -Force) {
            if ([IO.File]::ReadAllText($file.FullName) -match '__LOOM_[A-Z_]+__') { throw "Template: unresolved token in $($file.Name)" }
        }
        # Directory.Move refuses an occupied destination, including a concurrent creator.
        [IO.Directory]::Move($stage, $destinationPath)
        [pscustomobject]@{ Descriptor = (Join-Path $destinationPath "$Name.loomproject"); Directory = $destinationPath; TemplateVersion = 1 }
    } finally {
        # Clean only this invocation's exact GUID-named staging directory.
        if ((Split-Path -Parent ([IO.Path]::GetFullPath($stage))) -ne $parent -or (Split-Path -Leaf $stage) -notmatch '^\.loom-new-[a-f0-9]{32}$') { throw 'Unsafe staging path.' }
        if (Test-Path -LiteralPath $stage) { Remove-Item -LiteralPath $stage -Recurse -Force }
    }
}

Export-ModuleMember -Function New-LoomProject
