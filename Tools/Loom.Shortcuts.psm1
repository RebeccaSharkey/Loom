function Set-LoomStartMenuShortcut {
    [CmdletBinding()]
    param(
        [Parameter(Mandatory)][string]$InstallRoot,
        [string]$ProgramsDirectory
    )
    $ErrorActionPreference = 'Stop'
    $InstallRoot = [IO.Path]::GetFullPath($InstallRoot)
    $launcher = Join-Path $InstallRoot 'Loom.exe'
    if (-not (Test-Path -LiteralPath $launcher -PathType Leaf)) { throw "Cannot create Start menu shortcut: launcher not found: $launcher" }
    if (-not $ProgramsDirectory) { $ProgramsDirectory = [Environment]::GetFolderPath('Programs') }
    if (-not $ProgramsDirectory) { throw 'Windows did not provide a per-user Start menu folder.' }
    $ProgramsDirectory = [IO.Path]::GetFullPath($ProgramsDirectory)
    New-Item -ItemType Directory -Force -Path $ProgramsDirectory | Out-Null
    $shortcutPath = Join-Path $ProgramsDirectory 'Loom.lnk'
    $shell = $null
    $shortcut = $null
    try {
        $shell = New-Object -ComObject WScript.Shell
        $shortcut = $shell.CreateShortcut($shortcutPath)
        $shortcut.TargetPath = $launcher
        $shortcut.WorkingDirectory = $InstallRoot
        $shortcut.Arguments = ''
        $shortcut.IconLocation = "$launcher,0"
        $shortcut.Description = 'Loom Engine project launcher'
        $shortcut.WindowStyle = 1
        $shortcut.Save()
        return $shortcutPath
    } finally {
        if ($shortcut) { [void][Runtime.InteropServices.Marshal]::ReleaseComObject($shortcut) }
        if ($shell) { [void][Runtime.InteropServices.Marshal]::ReleaseComObject($shell) }
    }
}

Export-ModuleMember -Function Set-LoomStartMenuShortcut
