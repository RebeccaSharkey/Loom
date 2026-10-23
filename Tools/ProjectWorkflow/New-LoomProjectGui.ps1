[CmdletBinding()]
param(
    [string]$AcceptanceDestination,
    [string]$AcceptanceEngineRoot,
    [switch]$AcceptanceExerciseTasks,
    [string]$PreviewPath,
    [double]$PreviewWidth = 0,
    [double]$PreviewHeight = 0,
    [switch]$AcceptanceWindowControls,
    [switch]$RequireNoConsole
)
$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName PresentationFramework
Add-Type -AssemblyName PresentationCore
Add-Type -AssemblyName WindowsBase
Import-Module (Join-Path $PSScriptRoot 'Loom.Project.psm1') -Force
if (-not ('Loom.ProjectWorkflow.WindowsFolderPicker' -as [type])) {
    Add-Type -Path (Join-Path $PSScriptRoot 'WindowsFolderPicker.cs')
}

if (-not ('Loom.ProjectWorkflow.WindowsWindowFrame' -as [type])) {
    Add-Type -Path (Join-Path $PSScriptRoot 'WindowsWindowFrame.cs') -ReferencedAssemblies @('System.dll', [Windows.Interop.HwndSource].Assembly.Location, [Windows.Threading.Dispatcher].Assembly.Location)
}
if ($RequireNoConsole -and [Loom.ProjectWorkflow.WindowsWindowFrame]::GetConsoleWindow() -ne [IntPtr]::Zero) { throw 'The executable launcher opened a console.' }

[xml]$markup = Get-Content -LiteralPath (Join-Path $PSScriptRoot 'New-LoomProjectGui.xaml') -Raw
$reader = New-Object System.Xml.XmlNodeReader $markup
try { $window = [Windows.Markup.XamlReader]::Load($reader) } finally { $reader.Close() }
$chrome = New-Object Windows.Shell.WindowChrome
$chrome.CaptionHeight = 42
$chrome.ResizeBorderThickness = New-Object Windows.Thickness 6
$chrome.GlassFrameThickness = New-Object Windows.Thickness 0
$chrome.CornerRadius = New-Object Windows.CornerRadius 12
[Windows.Shell.WindowChrome]::SetWindowChrome($window, $chrome)

$name = $window.FindName('NameInput')
$title = $window.FindName('DisplayNameInput')
$destination = $window.FindName('DestinationInput')
$engine = $window.FindName('EngineInput')
$module = $window.FindName('Render2DInput')
$status = $window.FindName('StatusText')
$output = $window.FindName('OutputText')
$create = $window.FindName('CreateButton')
$build = $window.FindName('BuildButton')
$run = $window.FindName('RunButton')
$open = $window.FindName('OpenButton')
$debug = $window.FindName('DebugOption')
$release = $window.FindName('ReleaseOption')
$errorLabels = @{
    Name = $window.FindName('NameError')
    DisplayName = $window.FindName('DisplayNameError')
    Destination = $window.FindName('DestinationError')
    EngineRoot = $window.FindName('EngineError')
}
$script:createdDirectory = $null
$script:job = $null
$script:task = $null
$script:acceptanceError = $null
$script:acceptanceStage = 0
$script:taskChecks = New-Object 'Collections.Generic.List[string]'
$script:previewSaved = $false

$engine.Text = $env:LOOM_ROOT
if (-not $engine.Text) { $engine.Text = [Environment]::GetEnvironmentVariable('LOOM_ROOT', 'User') }

function Update-EngineVersion {
    $versionLabel = $window.FindName('EngineVersionText')
    $versionLabel.Text = 'Version unavailable'
    $versionLabel.ToolTip = 'Select a valid Loom engine installation to see its version.'
    if (-not $engine.Text) { return }
    try {
        $config = Join-Path $engine.Text 'lib/cmake/Loom/LoomConfig.cmake'
        if (-not (Test-Path -LiteralPath $config -PathType Leaf)) { return }
        $metadata = Get-Content -LiteralPath $config -Raw -ErrorAction Stop
        $match = [regex]::Match($metadata, '(?m)^\s*set\(Loom_VERSION\s+"([0-9]+\.[0-9]+\.[0-9]+)"\s*\)')
        if ($match.Success) {
            $versionLabel.Text = 'v' + $match.Groups[1].Value
            $versionLabel.ToolTip = 'Engine version from the selected Loom installation.'
        }
    } catch {
        # An incomplete or unreadable selection should not interrupt path editing.
    }
}
$engine.Add_TextChanged({ Update-EngineVersion })
Update-EngineVersion

function Set-Status([string]$message, [bool]$failed = $false) {
    $status.Text = $message
    $status.Foreground = [Windows.Media.BrushConverter]::new().ConvertFromString($(if ($failed) { '#FFB4AB' } else { '#A6A6A6' }))
}
function Save-Preview {
    if (-not $PreviewPath -or $script:previewSaved) { return }
    $window.UpdateLayout()
    $bitmap = New-Object Windows.Media.Imaging.RenderTargetBitmap ([int]$window.ActualWidth), ([int]$window.ActualHeight), 96, 96, ([Windows.Media.PixelFormats]::Pbgra32)
    $bitmap.Render($window)
    $encoder = New-Object Windows.Media.Imaging.PngBitmapEncoder
    $encoder.Frames.Add([Windows.Media.Imaging.BitmapFrame]::Create($bitmap))
    $stream = [IO.File]::Create([IO.Path]::GetFullPath($PreviewPath))
    try { $encoder.Save($stream) } finally { $stream.Dispose() }
    $script:previewSaved = $true
}
function Select-Folder($inputBox, [bool]$projectFolder) {
    $initial = $inputBox.Text
    if ($initial -and -not (Test-Path -LiteralPath $initial -PathType Container)) { $initial = Split-Path -Parent $initial }
    if (-not $initial -or -not (Test-Path -LiteralPath $initial -PathType Container)) { $initial = [Environment]::GetFolderPath('MyDocuments') }
    $caption = if ($projectFolder) { 'Choose a parent folder for your project' } else { 'Choose the Loom engine installation' }
    try {
        $owner = (New-Object Windows.Interop.WindowInteropHelper $window).Handle
        $selected = [Loom.ProjectWorkflow.WindowsFolderPicker]::Show($owner, $caption, $initial)
        if ($selected) { $inputBox.Text = if ($projectFolder) { Join-Path $selected $name.Text } else { $selected } }
    } catch { Set-Status ('Could not open folder picker: ' + $_.Exception.Message) $true }
}
$window.FindName('BrowseDestinationButton').Add_Click({ Select-Folder $destination $true })
$window.FindName('BrowseEngineButton').Add_Click({ Select-Folder $engine $false })
function Toggle-WindowState {
    if ($window.WindowState -eq 'Maximized') { $window.WindowState = 'Normal' }
    else { $window.WindowState = 'Maximized' }
}
foreach ($buttonName in @('MinimizeButton','MaximizeButton','CloseButton')) {
    [Windows.Shell.WindowChrome]::SetIsHitTestVisibleInChrome($window.FindName($buttonName), $true)
}
$window.FindName('MinimizeButton').Add_Click({ $window.WindowState = 'Minimized' })
$window.FindName('MaximizeButton').Add_Click({ Toggle-WindowState })
$window.FindName('CloseButton').Add_Click({ [Windows.SystemCommands]::CloseWindow($window) })
$window.Add_StateChanged({
    $button = $window.FindName('MaximizeButton')
    $maximized = $window.WindowState -eq 'Maximized'
    $button.Content = if ($maximized) { [string][char]0x2750 } else { [string][char]0x25A1 }
    $button.ToolTip = if ($maximized) { 'Restore' } else { 'Maximize' }
    [Windows.Automation.AutomationProperties]::SetName($button, $button.ToolTip)
})
$window.Add_PreviewKeyDown({
    if ($_.Key -eq 'F11' -or ($_.Key -eq 'Escape' -and $window.WindowState -eq 'Maximized')) {
        Toggle-WindowState
        $_.Handled = $true
    }
})
$window.Add_SourceInitialized({
    $handle = (New-Object Windows.Interop.WindowInteropHelper $window).Handle
    $source = [Windows.Interop.HwndSource]::FromHwnd($handle)
    [Loom.ProjectWorkflow.WindowsWindowFrame]::Attach($handle)
    $area = [Loom.ProjectWorkflow.WindowsWindowFrame]::WorkArea($handle)
    $transform = $source.CompositionTarget.TransformFromDevice
    $availableWidth = ($area.Right - $area.Left) * $transform.M11
    $availableHeight = ($area.Bottom - $area.Top) * $transform.M22
    $window.MinWidth = [Math]::Min(640, $availableWidth)
    $window.MinHeight = [Math]::Min(420, $availableHeight)
    $window.Width = [Math]::Min($window.Width, $availableWidth - 24)
    $window.Height = [Math]::Min($window.Height, $availableHeight - 24)
})
$create.Add_Click({
    foreach ($label in $errorLabels.Values) { $label.Visibility = 'Collapsed'; $label.Text = '' }
    try {
        $modules = @()
        if ($module.IsChecked) { $modules = @('Render2D') }
        $result = New-LoomProject -Name $name.Text -DisplayName $title.Text -Destination $destination.Text -EngineRoot $engine.Text -Modules $modules
        $script:createdDirectory = $result.Directory
        $script:acceptanceError = $null
        Set-Status ('Created ' + $name.Text + '. Ready to build.')
        $output.Text = "Project created.`r`n`r`nFolder: $($result.Directory)`r`nDescriptor: $($result.Descriptor)`r`nTemplate version: $($result.TemplateVersion)"
        $build.IsEnabled = $run.IsEnabled = $open.IsEnabled = $true
        $create.IsEnabled = $false
    } catch {
        $message = $_.Exception.Message
        Set-Status $message $true
        $field = 'Destination'
        foreach ($key in @('Name','DisplayName','EngineRoot')) { if ($message -like ($key + ':*')) { $field = $key } }
        $errorLabels[$field].Text = $message
        $errorLabels[$field].Visibility = 'Visible'
        $script:acceptanceError = $message
    }
})

function Start-ProjectTask([bool]$compile) {
    if ($script:job) { return }
    $configuration = if ($release.IsChecked) { 'Release' } else { 'Debug' }
    $script:job = [PowerShell]::Create()
    [void]$script:job.AddCommand((Join-Path $script:createdDirectory 'Automation/Run.ps1'))
    [void]$script:job.AddParameter('Configuration', $configuration)
    if ($compile) {
        [void]$script:job.AddParameter('Configure', $true)
        [void]$script:job.AddParameter('Build', $true)
        [void]$script:job.AddParameter('SmokeTest', $true)
    } elseif ($AcceptanceExerciseTasks) {
        # Acceptance uses the real Run action but exits through the normal lifecycle.
        [void]$script:job.AddParameter('SmokeTest', $true)
    }
    $script:task = $script:job.BeginInvoke()
    $build.IsEnabled = $run.IsEnabled = $debug.IsEnabled = $release.IsEnabled = $false
    Set-Status ($(if ($compile) { 'Building ' } else { 'Running ' }) + $configuration + '...')
    $output.Text = 'Working. Output will appear when the task completes.'
}
$build.Add_Click({ Start-ProjectTask $true })
$run.Add_Click({ Start-ProjectTask $false })
$open.Add_Click({ Start-Process -FilePath explorer.exe -ArgumentList ('"' + $script:createdDirectory + '"') })

$timer = New-Object Windows.Threading.DispatcherTimer
$timer.Interval = [TimeSpan]::FromMilliseconds(100)
$timer.Add_Tick({
    if ($script:task -and $script:task.IsCompleted) {
        $failed = $false
        try {
            $result = $script:job.EndInvoke($script:task)
            $output.Text = [regex]::Replace((($result | Out-String) + ($script:job.Streams.Error | Out-String)), '\x1B\[[0-?]*[ -/]*[@-~]', '')
            $failed = $script:job.HadErrors
            Set-Status $(if ($failed) { 'Task failed. See the output below.' } else { 'Done. Your project is ready.' }) $failed
        } catch { $output.Text = $_ | Out-String; Set-Status 'Task failed. See the output below.' $true; $failed = $true }
        $script:job.Dispose()
        $script:job = $script:task = $null
        $build.IsEnabled = $run.IsEnabled = $debug.IsEnabled = $release.IsEnabled = $true
        if ($AcceptanceExerciseTasks) {
            if ($failed) { $script:acceptanceError = $output.Text; $window.Close(); return }
            $script:taskChecks.Add("Task stage $script:acceptanceStage passed")
            if ($script:acceptanceStage -eq 1) {
                $release.IsChecked = $true
                $script:acceptanceStage = 2
                $build.RaiseEvent((New-Object Windows.RoutedEventArgs ([Windows.Controls.Button]::ClickEvent)))
            } elseif ($script:acceptanceStage -eq 2) {
                $script:acceptanceStage = 3
                $run.RaiseEvent((New-Object Windows.RoutedEventArgs ([Windows.Controls.Button]::ClickEvent)))
            } else { Save-Preview; $window.Close() }
        }
    }
})
$timer.Start()
$window.Add_Closing({
    if ($script:job) { $_.Cancel = $true; Set-Status 'Wait for the build to finish or close the game before closing the launcher.' }
})
$window.Add_ContentRendered({
    if ($AcceptanceWindowControls) {
        $maximize = $window.FindName('MaximizeButton')
        $maximize.RaiseEvent((New-Object Windows.RoutedEventArgs ([Windows.Controls.Button]::ClickEvent)))
        $window.UpdateLayout()
        if ($window.WindowState -ne 'Maximized') { throw 'Maximize did not work.' }
        $handle = (New-Object Windows.Interop.WindowInteropHelper $window).Handle
        $area = [Loom.ProjectWorkflow.WindowsWindowFrame]::WorkArea($handle)
        $bounds = [Loom.ProjectWorkflow.WindowsWindowFrame]::Bounds($handle)
        if ($bounds.Left -lt $area.Left - 8 -or $bounds.Top -lt $area.Top - 8 -or $bounds.Right -gt $area.Right + 8 -or $bounds.Bottom -gt $area.Bottom + 8) { throw 'Maximized window extends outside the work area.' }
        $maximize.RaiseEvent((New-Object Windows.RoutedEventArgs ([Windows.Controls.Button]::ClickEvent)))
        if ($window.WindowState -ne 'Normal') { throw 'Restore did not work.' }
        $window.FindName('MinimizeButton').RaiseEvent((New-Object Windows.RoutedEventArgs ([Windows.Controls.Button]::ClickEvent)))
        if ($window.WindowState -ne 'Minimized') { throw 'Minimize did not work.' }
        $window.WindowState = 'Normal'
        if ($window.WindowState -ne 'Normal') { throw 'Restore from minimized did not work.' }
        foreach ($key in @([Windows.Input.Key]::F11, [Windows.Input.Key]::Escape)) {
            $event = New-Object Windows.Input.KeyEventArgs ([Windows.Input.Keyboard]::PrimaryDevice), ([Windows.PresentationSource]::FromVisual($window)), 0, $key
            $event.RoutedEvent = [Windows.Input.Keyboard]::PreviewKeyDownEvent
            $window.RaiseEvent($event)
            $expected = if ($key -eq [Windows.Input.Key]::F11) { 'Maximized' } else { 'Normal' }
            if ($window.WindowState -ne $expected) { throw "Window keyboard shortcut failed: $key" }
        }
    }
    if ($PreviewWidth -gt 0) { $window.Width = $PreviewWidth }
    if ($PreviewHeight -gt 0) { $window.Height = $PreviewHeight }
    $window.UpdateLayout()
    if ($AcceptanceWindowControls -and $PreviewHeight -gt 0) {
        $viewport = $window.FindName('FormViewport')
        if ($viewport.ScrollableHeight -le 0) { throw 'Compact layout does not scroll.' }
        $viewport.ScrollToBottom()
        $window.UpdateLayout()
        $bottom = $output.TransformToAncestor($window).Transform((New-Object Windows.Point 0, $output.ActualHeight)).Y
        if ($bottom -gt $window.ActualHeight) { throw 'The bottom of the form cannot be reached.' }
        $viewport.ScrollToTop()
    }
    if ($AcceptanceDestination) {
        # Native folder dialogs remain interactive; check inline validation and Create here.
        $name.Text = '../Invalid'
        $destination.Text = $AcceptanceDestination
        $engine.Text = $AcceptanceEngineRoot
        if ($window.FindName('EngineVersionText').Text -notmatch '^v[0-9]+\.[0-9]+\.[0-9]+$') { throw 'The engine version footer did not resolve the installed SDK version.' }
        $create.RaiseEvent((New-Object Windows.RoutedEventArgs ([Windows.Controls.Button]::ClickEvent)))
        if ($errorLabels.Name.Visibility -ne 'Visible') { $script:acceptanceError = 'Inline validation was not shown.'; $window.Close(); return }
        $name.Text = 'GuiProof'
        $title.Text = 'GUI Proof'
        $module.IsChecked = $true
        $create.RaiseEvent((New-Object Windows.RoutedEventArgs ([Windows.Controls.Button]::ClickEvent)))
        if ($AcceptanceExerciseTasks -and $script:createdDirectory) {
            $script:acceptanceStage = 1
            $build.RaiseEvent((New-Object Windows.RoutedEventArgs ([Windows.Controls.Button]::ClickEvent)))
        } else { Save-Preview; $window.Close() }
    } elseif ($PreviewPath) { Save-Preview; $window.Close() }
})
try { [void]$window.ShowDialog() }
finally { $timer.Stop() }
if ($AcceptanceDestination -and (-not $script:createdDirectory -or $script:acceptanceError)) { throw "GUI acceptance failed: $script:acceptanceError" }
if ($AcceptanceExerciseTasks) {
    if ($script:taskChecks.Count -ne 3) { throw 'GUI task acceptance did not complete Debug build, Release build and Run.' }
    $script:taskChecks | Write-Output
}
