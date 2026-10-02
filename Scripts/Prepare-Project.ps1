[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)] [string] $EngineRoot,
    [switch] $OpenEditor
)
$ErrorActionPreference = 'Stop'
$taskProjectRoot = Split-Path -Parent $PSScriptRoot
$taskProject = Join-Path $taskProjectRoot 'Kurukshetra.uproject'
$taskEngineRoot = (Resolve-Path -LiteralPath $EngineRoot).Path
$taskBuild = Join-Path $taskEngineRoot 'Engine\Build\BatchFiles\Build.bat'
$taskEditor = Join-Path $taskEngineRoot 'Engine\Binaries\Win64\UnrealEditor.exe'
$taskCommandlet = Join-Path $taskEngineRoot 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe'
foreach ($taskRequired in @($taskBuild, $taskEditor, $taskCommandlet)) {
    if (-not (Test-Path -LiteralPath $taskRequired)) { throw "Unreal Engine file missing: $taskRequired" }
}

Write-Host 'Building the Unreal Editor module. The first build may take several minutes.'
& $taskBuild KurukshetraEditor Win64 Development "-Project=$taskProject" -WaitMutex
if ($LASTEXITCODE -ne 0) { throw "Unreal build failed (exit $LASTEXITCODE). Review the compiler output above." }

$taskLogDir = Join-Path $taskProjectRoot 'Saved\Logs'
New-Item -ItemType Directory -Path $taskLogDir -Force | Out-Null
$taskLog = Join-Path $taskLogDir 'Bootstrap.log'
$taskBootstrap = Join-Path $PSScriptRoot 'bootstrap_content.py'
# Override the not-yet-created startup map for this first commandlet run.
& $taskCommandlet $taskProject '-run=pythonscript' "-script=$taskBootstrap" '-unattended' '-nullrhi' '-nosplash' "-abslog=$taskLog" '-ini:Engine:[/Script/EngineSettings.GameMapsSettings]:EditorStartupMap=/Engine/Maps/Entry'
if ($LASTEXITCODE -ne 0) { throw "Content generation failed (exit $LASTEXITCODE). See $taskLog" }
if (-not (Test-Path -LiteralPath $taskLog) -or -not (Select-String -LiteralPath $taskLog -Pattern 'KURU_BOOTSTRAP_COMPLETE' -Quiet)) {
    throw "Content generation did not report success. See $taskLog"
}
if (-not (Test-Path -LiteralPath (Join-Path $taskProjectRoot 'Content\Maps\Chapter01.umap'))) {
    throw 'The launch map was not generated. Inspect the bootstrap log before continuing.'
}
Write-Host 'Project prepared. Open it in Unreal Editor, then press Play.'
if ($OpenEditor) {
    # The user explicitly requested an interactive editor window with this switch.
    Start-Process -FilePath $taskEditor -ArgumentList @('"' + $taskProject + '"')
}
