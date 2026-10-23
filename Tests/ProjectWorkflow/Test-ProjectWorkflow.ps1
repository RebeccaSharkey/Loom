[CmdletBinding()]
param(
    [Parameter(Mandatory)][string]$EngineRoot,
    [string]$WorkRoot,
    [string]$CMake = 'cmake',
    [switch]$RunApplications,
    [switch]$RunGui
)
$ErrorActionPreference = 'Stop'
if (-not $WorkRoot) { $WorkRoot = Join-Path $PSScriptRoot '../../Generated/WorkflowAcceptance' }
$EngineRoot = [IO.Path]::GetFullPath($EngineRoot)
$WorkRoot = [IO.Path]::GetFullPath($WorkRoot)
New-Item -ItemType Directory -Force -Path $WorkRoot | Out-Null
$root = Join-Path $WorkRoot ([guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $root | Out-Null
$workflow = Join-Path $EngineRoot 'tools/ProjectWorkflow'
Import-Module (Join-Path $workflow 'Loom.Project.psm1') -Force
$checks = New-Object 'Collections.Generic.List[string]'

function Check([bool]$condition, [string]$label) {
    if (-not $condition) { throw "FAIL: $label" }
    $checks.Add($label)
    Write-Host "PASS: $label"
}
function Expect-Failure([scriptblock]$action, [string]$pattern, [string]$label) {
    $caught = $null
    try { & $action | Out-Null } catch { $caught = $_.Exception.Message }
    Check ([bool]($caught -and $caught -match $pattern)) $label
}
function Invoke-Native([string]$executable, [string[]]$arguments, [string]$failurePattern = '') {
    $saved = $ErrorActionPreference
    $ErrorActionPreference = 'Continue'
    $result = & $executable @arguments 2>&1
    $code = $LASTEXITCODE
    $ErrorActionPreference = $saved
    $text = [regex]::Replace(($result | Out-String), '\x1B\[[0-?]*[ -/]*[@-~]', '')
    Add-Content -LiteralPath (Join-Path $root 'commands.log') -Value ($executable + ' ' + ($arguments -join ' ') + "`n" + $text)
    if ($failurePattern) {
        if ($code -eq 0 -or $text -notmatch $failurePattern) { throw "Expected failure '$failurePattern', exit $code`n$text" }
    } elseif ($code -ne 0) { throw "Command failed ($code): $executable`n$text" }
    return $text
}
function Configure([string]$directory, [string]$engine = $EngineRoot, [string]$failure = '') {
    Invoke-Native $CMake @('-S', $directory, '-B', (Join-Path $directory 'Generated/msvc'), '-A', 'x64', "-DLOOM_ENGINE_ROOT=$engine") $failure
}
function Build([string]$directory, [string]$config = 'Debug', [string]$failure = '') {
    Invoke-Native $CMake @('--build', (Join-Path $directory 'Generated/msvc'), '--config', $config) $failure
}

$oldDefault = $env:LOOM_ROOT
try {
    # Use the installed script, not a parallel test implementation of generation.
    $cliDirectory = Join-Path $root 'CLI Project With Spaces'
    $cliJson = & (Join-Path $workflow 'New-LoomProject.ps1') -Name CliProof -DisplayName 'CLI Proof' -Destination $cliDirectory -EngineRoot $EngineRoot -Modules Render2D
    $cli = $cliJson | ConvertFrom-Json
    Check (Test-Path -LiteralPath $cli.Descriptor) 'CLI produces descriptor and useful result'
    Check ((Test-Path -LiteralPath (Join-Path $cli.Directory 'Automation/Run.ps1')) -and -not (Test-Path -LiteralPath (Join-Path $cli.Directory 'Run.ps1'))) 'Run helper lives only in Automation'
    Invoke-Native $CMake @('--list-presets','-S',$cli.Directory) | Out-Null
    Push-Location $cli.Directory
    try { Invoke-Native $CMake @('--preset','loom-local') | Out-Null }
    finally { Pop-Location }
    Check $true 'Shared and personal presets parse and configure successfully'
    $defaultProject = New-LoomProject -Name DefaultProof -Destination (Join-Path $root 'Environment Default')
    $env:LOOM_ROOT = $EngineRoot
    Push-Location $defaultProject.Directory
    try { $defaultConfigure = Invoke-Native $CMake @('--preset','msvc') }
    finally { Pop-Location }
    Check ($defaultConfigure.Contains(($EngineRoot -replace '\\','/'))) 'A fresh project discovers LOOM_ROOT without personal configuration'
    $second = New-LoomProject -Name SecondProof -DisplayName 'Second Proof' -Destination (Join-Path $root 'Second Project') -EngineRoot $EngineRoot
    $env:LOOM_ROOT = Join-Path $root 'Wrong Default'
    $configured = Configure $cli.Directory
    Check ($configured.Contains(($EngineRoot -replace '\\','/'))) 'Explicit selection wins over a different LOOM_ROOT'
    Build $cli.Directory Debug | Out-Null
    Build $cli.Directory Release | Out-Null
    Configure $second.Directory | Out-Null
    Build $second.Directory Debug | Out-Null
    Build $second.Directory Release | Out-Null
    Check $true 'Two differently named generated projects build Debug and Release'
    $cmakeSource = Get-Content -LiteralPath (Join-Path $cli.Directory 'CMakeLists.txt') -Raw
    Check ($cmakeSource -notmatch 'find_package\(SDL|vcpkg|add_subdirectory\(') 'Game declares only Loom, with no engine source or dependency lookup'
    Check (-not (Get-ChildItem -LiteralPath $cli.Directory -Recurse -File | Where-Object { $_.FullName -notlike '*\Generated\*' -and ([IO.File]::ReadAllText($_.FullName) -match '__LOOM_[A-Z_]+__') })) 'No unresolved template tokens'
    Check (-not (Test-Path -LiteralPath (Join-Path $cli.Directory 'Source/.idea'))) 'Template excludes IDE state'

    $sentinel = Join-Path $cli.Directory 'keep.txt'
    'preserve me' | Set-Content -LiteralPath $sentinel
    Expect-Failure { New-LoomProject -Name CliProof -Destination $cli.Directory } 'already exists' 'Occupied destination is rejected'
    Check ((Get-Content -LiteralPath $sentinel -Raw).Trim() -eq 'preserve me') 'Occupied destination preserves existing files'
    Expect-Failure { New-LoomProject -Name '../Bad' -Destination (Join-Path $root 'Bad') } 'Name:' 'Invalid identifier is rejected'
    Expect-Failure { New-LoomProject -Name CON -Destination (Join-Path $root 'Bad') } 'Name:' 'Reserved Windows name is rejected'
    Expect-Failure { New-LoomProject -Name Missing -Destination (Join-Path $root 'Bad') -TemplatePath (Join-Path $root 'Missing Template') } 'Template:' 'Missing template fails without publication'
    Expect-Failure { New-LoomProject -Name BadModule -Destination (Join-Path $root 'Bad') -Modules Unknown } 'Modules:' 'Unknown generator module is rejected'
    Check (-not (Test-Path -LiteralPath (Join-Path $root 'Bad'))) 'Failed generation leaves no project'
    $brokenTemplate = Join-Path $root 'Broken Template'
    Copy-Item -LiteralPath (Join-Path $EngineRoot 'templates/Minimal') -Destination $brokenTemplate -Recurse
    Add-Content -LiteralPath (Join-Path $brokenTemplate 'Source/main.cpp') -Value '// __LOOM_UNRESOLVED__'
    Expect-Failure { New-LoomProject -Name Broken -Destination (Join-Path $root 'Bad') -TemplatePath $brokenTemplate } 'unresolved token' 'A deliberately broken template fails before publication'
    $brokenDescriptorPath = Join-Path $brokenTemplate 'Project.loomproject'
    $brokenDescriptor = Get-Content -LiteralPath $brokenDescriptorPath -Raw | ConvertFrom-Json
    $brokenDescriptor.FileVersion = 999
    $brokenDescriptor | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $brokenDescriptorPath -Encoding UTF8
    Expect-Failure { New-LoomProject -Name Broken -Destination (Join-Path $root 'Bad') -TemplatePath $brokenTemplate } 'project contract' 'Unsupported template descriptor fails before publication'
    Check (-not (Get-ChildItem -LiteralPath $root -Directory -Force -Filter '.loom-new-*')) 'Failed generation cleans only owned staging files'

    $descriptorPath = $cli.Descriptor
    $original = Get-Content -LiteralPath $descriptorPath -Raw
    $descriptor = $original | ConvertFrom-Json
    $descriptor.DisplayName = 'Changed title after configure'
    $descriptor.Modules = @()
    $descriptor | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $descriptorPath -Encoding UTF8
    Build $cli.Directory | Out-Null
    Check ((Get-Content -LiteralPath (Join-Path $cli.Directory 'Generated/msvc/LoomProjectConfig.h') -Raw) -match 'Changed title after configure') 'Normal build regenerates descriptor metadata'
    $descriptor.Modules = @('UnavailableModule')
    $descriptor | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $descriptorPath -Encoding UTF8
    Build $cli.Directory Debug 'unknown module' | Out-Null
    Check $true 'Unknown descriptor module fails the next normal build'
    $unsupportedSchema = $original | ConvertFrom-Json
    $unsupportedSchema.FileVersion = 999
    $missingField = $original | ConvertFrom-Json
    $missingField.PSObject.Properties.Remove('DisplayName')
    $escapingContent = $original | ConvertFrom-Json
    $escapingContent.ContentRoot = '../escape'
    $unsupportedEngine = $original | ConvertFrom-Json
    $unsupportedEngine.EngineVersion = '99.0.0'
    foreach ($case in @(
        @{ Text = 'not JSON'; Pattern = 'JSON object'; Label = 'Malformed JSON' },
        @{ Text = ($unsupportedSchema | ConvertTo-Json -Depth 8); Pattern = 'FileVersion'; Label = 'Unsupported schema' },
        @{ Text = ($missingField | ConvertTo-Json -Depth 8); Pattern = 'DisplayName'; Label = 'Missing required field' },
        @{ Text = ($escapingContent | ConvertTo-Json -Depth 8); Pattern = 'ContentRoot'; Label = 'Escaping content root' },
        @{ Text = ($unsupportedEngine | ConvertTo-Json -Depth 8); Pattern = 'version'; Label = 'Unsupported engine version' }
    )) {
        $case.Text | Set-Content -LiteralPath $descriptorPath -Encoding UTF8
        Configure $cli.Directory $EngineRoot $case.Pattern | Out-Null
        Check $true ($case.Label + ' fails clearly')
    }
    # Exercise both project and content paths containing spaces.
    Move-Item -LiteralPath (Join-Path $cli.Directory 'Content') -Destination (Join-Path $cli.Directory 'Content With Spaces')
    $descriptor = $original | ConvertFrom-Json
    $descriptor.ContentRoot = 'Content With Spaces'
    $descriptor | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $descriptorPath -Encoding UTF8
    Configure $cli.Directory | Out-Null
    Build $cli.Directory Debug | Out-Null
    Build $cli.Directory Release | Out-Null
    Check $true 'Project and content paths containing spaces build'
    Configure $cli.Directory (Join-Path $root 'Missing Engine') 'invalid engine selection' | Out-Null
    Check $true 'Invalid explicit engine selection fails without fallback'

    $source = Join-Path $cli.Directory 'Source/main.cpp'
    Add-Content -LiteralPath $source -Value '// Hand edit: must survive engine selection changes.'
    $hash = (Get-FileHash -LiteralPath $source).Hash
    $alternate = Join-Path $root 'Alternate SDK With Spaces'
    New-Item -ItemType Directory -Path $alternate | Out-Null
    Copy-Item -Path (Join-Path $EngineRoot '*') -Destination $alternate -Recurse -Force
    Configure $cli.Directory $alternate | Out-Null
    Build $cli.Directory Debug | Out-Null
    Build $cli.Directory Release | Out-Null
    Check ((Get-FileHash -LiteralPath $source).Hash -eq $hash) 'Relocated SDK builds without overwriting hand-edited game source'
    $cache = Get-Content -LiteralPath (Join-Path $cli.Directory 'Generated/msvc/CMakeCache.txt') -Raw
    Check ($cache.Contains(($alternate -replace '\\','/'))) 'Cache records the newly selected SDK'

    if ($RunApplications) {
        $oldPause = $env:LOOM_AUTOMATION_NO_PAUSE
        $env:LOOM_AUTOMATION_NO_PAUSE = '1'
        try {
            foreach ($action in @('BuildAndRunDebug.bat','BuildAndRunRelease.bat','Run.bat','SmokeTest.bat','RunPackaged.bat')) {
                $arguments = @('-SmokeTest')
                if ($action -eq 'SmokeTest.bat') { $arguments = @() }
                $result = Invoke-Native (Join-Path $cli.Directory "Automation/$action") $arguments
                Check ($result -match 'Loom content path proof' -and $result -match 'Application shutdown') "Clickable action works: $action"
            }
            Invoke-Native (Join-Path $defaultProject.Directory 'Automation/Run.bat') @() 'Executable missing' | Out-Null
            Check $true 'Clickable action preserves failure exit status and readable error'
        } finally { $env:LOOM_AUTOMATION_NO_PAUSE = $oldPause }
        $unrelated = Join-Path $root 'Unrelated Working Directory'
        New-Item -ItemType Directory -Path $unrelated | Out-Null
        foreach ($config in @('Debug','Release')) {
            $exe = Join-Path $cli.Directory "Generated/msvc/$config/CliProof.exe"
            foreach ($cwd in @($cli.Directory, $unrelated, (Split-Path -Parent $exe))) {
                Push-Location $cwd
                try {
                    $result = Invoke-Native $exe @('--loom-smoke-test')
                    Check ($result -match 'Loom content path proof' -and $result -match 'Application shutdown' -and $result -match 'Logs:') "$config lifecycle/content/logs from $cwd"
                    $logMatch = [regex]::Match($result, 'Logs: ([^\r\n]+)')
                    $logDirectory = $logMatch.Groups[1].Value.Trim()
                    $logs = Get-ChildItem -LiteralPath $logDirectory -Filter '*.log'
                    Check ([bool]($logs | Where-Object { (Get-Content -LiteralPath $_.FullName -Raw) -match 'Application shutdown' })) "$config writes a log containing normal shutdown"
                } finally { Pop-Location }
            }
            Push-Location $unrelated
            try {
                $result = Invoke-Native $exe @('--loom-smoke-test','--loom-packaged')
                Check ($result -match 'Loom content path proof') "$config packaged content works from unrelated cwd"
            } finally { Pop-Location }
        }
        $welcome = Join-Path $cli.Directory 'Content With Spaces/welcome.txt'
        Move-Item -LiteralPath $welcome -Destination ($welcome + '.hidden')
        Invoke-Native (Join-Path $cli.Directory 'Generated/msvc/Debug/CliProof.exe') @('--loom-smoke-test') 'cannot read content file' | Out-Null
        Move-Item -LiteralPath ($welcome + '.hidden') -Destination $welcome
        Check $true 'Missing content produces a useful error and nonzero exit'
    }
    if ($RunGui) {
        & (Join-Path $workflow 'New-LoomProjectGui.ps1') -AcceptanceDestination (Join-Path $root 'GUI Project') -AcceptanceEngineRoot $EngineRoot
        $guiDirectory = Join-Path $root 'GUI Project'
        Configure $guiDirectory | Out-Null
        Build $guiDirectory Debug | Out-Null
        Build $guiDirectory Release | Out-Null
        $equivalent = New-LoomProject -Name GuiProof -DisplayName 'GUI Proof' -Destination (Join-Path $root 'Equivalent CLI') -EngineRoot $EngineRoot -Modules Render2D
        foreach ($file in Get-ChildItem -LiteralPath $equivalent.Directory -File -Recurse -Force) {
            $relative = $file.FullName.Substring($equivalent.Directory.Length + 1)
            Check ((Get-FileHash -LiteralPath $file.FullName).Hash -eq (Get-FileHash -LiteralPath (Join-Path $guiDirectory $relative)).Hash) "GUI/CLI equivalent: $relative"
        }
        if ($RunApplications) {
            foreach ($config in @('Debug','Release')) {
                Invoke-Native (Join-Path $guiDirectory "Generated/msvc/$config/GuiProof.exe") @('--loom-smoke-test') | Out-Null
            }
            Check $true 'GUI-generated application launches and shuts down in both configurations'
        }
    }
    [ordered]@{ Passed = $true; Checks = @($checks.ToArray()); Runtime = [bool]$RunApplications; Gui = [bool]$RunGui; Artifacts = $root } |
        ConvertTo-Json -Depth 8 | Set-Content -LiteralPath (Join-Path $root 'acceptance.json') -Encoding UTF8
    Write-Host "Acceptance passed. Evidence: $root"
} finally { $env:LOOM_ROOT = $oldDefault }
