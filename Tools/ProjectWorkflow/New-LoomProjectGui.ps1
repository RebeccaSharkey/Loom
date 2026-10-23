[CmdletBinding()]
param(
    # Exercises the real form's Create handler with deterministic inputs.
    [string]$AcceptanceDestination,
    [string]$AcceptanceEngineRoot
)
$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Windows.Forms
Add-Type -AssemblyName System.Drawing
Import-Module (Join-Path $PSScriptRoot 'Loom.Project.psm1') -Force

$form = New-Object Windows.Forms.Form
$form.Text = 'Loom - New Project'
$form.ClientSize = New-Object Drawing.Size(680, 600)
$form.StartPosition = 'CenterScreen'
$form.Font = New-Object Drawing.Font('Segoe UI', 10)
$form.FormBorderStyle = 'FixedDialog'
$form.MaximizeBox = $false
$script:createdDirectory = $null
$script:job = $null
$script:task = $null
$script:acceptanceError = $null

function Add-Field([string]$label, [int]$top, [string]$value) {
    $caption = New-Object Windows.Forms.Label
    $caption.Text = $label
    $caption.SetBounds(20, $top, 190, 24)
    $box = New-Object Windows.Forms.TextBox
    $box.Text = $value
    $box.SetBounds(215, $top, 355, 26)
    $form.Controls.AddRange(@($caption, $box))
    return $box
}
$name = Add-Field 'Code identifier' 24 'MyGame'
$title = Add-Field 'Display name' 64 'My Game'
$destination = Add-Field 'Final project folder' 104 ''
$engine = Add-Field 'Engine installation' 144 $env:LOOM_ROOT
foreach ($item in @(@($destination,104), @($engine,144))) {
    $button = New-Object Windows.Forms.Button
    $button.Text = 'Browse'
    $button.SetBounds(580, [int]$item[1], 80, 27)
    $box = $item[0]
    $button.Tag = $box
    $button.Add_Click({
        $dialog = New-Object Windows.Forms.FolderBrowserDialog
        if ($dialog.ShowDialog() -eq 'OK') {
            if ($this.Tag -eq $destination) { $this.Tag.Text = Join-Path $dialog.SelectedPath $name.Text }
            else { $this.Tag.Text = $dialog.SelectedPath }
        }
        $dialog.Dispose()
    })
    $form.Controls.Add($button)
}
$module = New-Object Windows.Forms.CheckBox
$module.Text = 'Render2D (optional stub; Core is always included)'
$module.SetBounds(215, 184, 440, 28)
$form.Controls.Add($module)
$errors = New-Object Windows.Forms.ErrorProvider
$errors.ContainerControl = $form
$status = New-Object Windows.Forms.Label
$status.Text = 'Choose a new destination folder. Its parent must exist.'
$status.SetBounds(20, 222, 640, 44)
$form.Controls.Add($status)

function Add-Button([string]$text, [int]$left, [int]$width) {
    $button = New-Object Windows.Forms.Button
    $button.Text = $text
    $button.SetBounds($left, 274, $width, 32)
    $form.Controls.Add($button)
    return $button
}
$create = Add-Button 'Create' 20 95
$build = Add-Button 'Build Debug' 125 120
$run = Add-Button 'Run Debug' 255 120
$open = Add-Button 'Open Folder' 385 120
$configuration = New-Object Windows.Forms.ComboBox
$configuration.DropDownStyle = 'DropDownList'
$configuration.Items.AddRange(@('Debug','Release'))
$configuration.SelectedIndex = 0
$configuration.SetBounds(520, 277, 140, 28)
$form.Controls.Add($configuration)
$configuration.Add_SelectedIndexChanged({ $build.Text = 'Build ' + $configuration.Text; $run.Text = 'Run ' + $configuration.Text })
$build.Enabled = $run.Enabled = $open.Enabled = $false
$output = New-Object Windows.Forms.TextBox
$output.Multiline = $true
$output.ReadOnly = $true
$output.ScrollBars = 'Both'
$output.SetBounds(20, 320, 640, 260)
$form.Controls.Add($output)

$create.Add_Click({
    $errors.Clear()
    try {
        $modules = @()
        if ($module.Checked) { $modules = @('Render2D') }
        $result = New-LoomProject -Name $name.Text -DisplayName $title.Text -Destination $destination.Text -EngineRoot $engine.Text -Modules $modules
        $script:createdDirectory = $result.Directory
        $status.Text = 'Created: ' + $result.Directory
        $output.Text = $result | ConvertTo-Json
        $build.Enabled = $run.Enabled = $open.Enabled = $true
        $create.Enabled = $false
    } catch {
        $message = $_.Exception.Message
        $status.Text = $message
        $field = $destination
        if ($message -like 'Name:*') { $field = $name }
        if ($message -like 'DisplayName:*') { $field = $title }
        if ($message -like 'EngineRoot:*') { $field = $engine }
        $errors.SetError($field, $message)
        $script:acceptanceError = $message
    }
})

function Start-ProjectTask([bool]$compile) {
    if ($script:job) { return }
    # A background runspace calls the same project helper with typed parameters.
    $script:job = [PowerShell]::Create()
    [void]$script:job.AddCommand((Join-Path $script:createdDirectory 'Automation/Run.ps1'))
    [void]$script:job.AddParameter('Configuration', $configuration.Text)
    if ($compile) {
        [void]$script:job.AddParameter('Configure', $true)
        [void]$script:job.AddParameter('Build', $true)
        [void]$script:job.AddParameter('SmokeTest', $true)
    }
    $script:task = $script:job.BeginInvoke()
    $build.Enabled = $run.Enabled = $false
    $status.Text = 'Running; output will appear when the task completes.'
}
$build.Add_Click({ Start-ProjectTask $true })
$run.Add_Click({ Start-ProjectTask $false })
$open.Add_Click({ Start-Process -FilePath explorer.exe -ArgumentList ('"' + $script:createdDirectory + '"') })
$timer = New-Object Windows.Forms.Timer
$timer.Interval = 100
$timer.Add_Tick({
    if ($script:task -and $script:task.IsCompleted) {
        try {
            $result = $script:job.EndInvoke($script:task)
            $output.Text = ($result | Out-String) + ($script:job.Streams.Error | Out-String)
            $status.Text = if ($script:job.HadErrors) { 'Task failed. See output.' } else { 'Task completed.' }
        } catch { $output.Text = $_ | Out-String; $status.Text = 'Task failed.' }
        $script:job.Dispose()
        $script:job = $script:task = $null
        $build.Enabled = $run.Enabled = $true
    }
})
$timer.Start()
$form.Add_FormClosing({
    if ($script:job) { $_.Cancel = $true; $status.Text = 'Wait for the running build or close the game before closing this window.' }
})
if ($AcceptanceDestination) {
    $form.Add_Shown({
        $name.Text = 'GuiProof'
        $title.Text = 'GUI Proof'
        $destination.Text = $AcceptanceDestination
        $engine.Text = $AcceptanceEngineRoot
        $module.Checked = $true
        $create.PerformClick()
        $form.Close()
    })
}
try { [void]$form.ShowDialog() }
finally { $timer.Stop(); $timer.Dispose(); $errors.Dispose(); $form.Dispose() }
if ($AcceptanceDestination -and (-not $script:createdDirectory -or $script:acceptanceError)) { throw "GUI acceptance failed: $script:acceptanceError" }
