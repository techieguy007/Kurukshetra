[CmdletBinding()]
param([string] $EngineRoot)
$ErrorActionPreference = 'Stop'
$taskProjectRoot = Split-Path -Parent $PSScriptRoot
$taskProject = Join-Path $taskProjectRoot 'Kurukshetra.uproject'
if (-not $EngineRoot) {
    $taskCandidates = @('C:\Program Files\Epic Games\UE_5.8')
    $taskManifest = 'C:\ProgramData\Epic\UnrealEngineLauncher\LauncherInstalled.dat'
    if (Test-Path -LiteralPath $taskManifest) {
        $taskInstalled = Get-Content -LiteralPath $taskManifest -Raw | ConvertFrom-Json
        $taskCandidates += @($taskInstalled.InstallationList | Where-Object { $_.AppName -eq 'UE_5.8' } | ForEach-Object { $_.InstallLocation })
    }
    $taskRegistration = Get-ItemProperty -LiteralPath 'HKLM:\SOFTWARE\EpicGames\Unreal Engine\5.8' -ErrorAction SilentlyContinue
    if ($taskRegistration.InstalledDirectory) { $taskCandidates += $taskRegistration.InstalledDirectory }
    foreach ($taskCandidate in $taskCandidates) {
        if (Test-Path -LiteralPath (Join-Path $taskCandidate 'Engine\Binaries\Win64\UnrealEditor.exe')) {
            $EngineRoot = $taskCandidate
            break
        }
    }
}
if (-not $EngineRoot) {
    throw 'Unreal Engine 5.8 is not installed. Install it in Epic Games Launcher > Unreal Engine > Library, then run this script again. For a custom installation, supply -EngineRoot.'
}
& (Join-Path $PSScriptRoot 'Prepare-Project.ps1') -EngineRoot $EngineRoot
$taskEditor = Join-Path (Resolve-Path -LiteralPath $EngineRoot).Path 'Engine\Binaries\Win64\UnrealEditor.exe'
# This switch launches an interactive game window, as requested by the user.
Start-Process -FilePath $taskEditor -ArgumentList @(('"' + $taskProject + '"'), '-game', '-windowed', '-ResX=1280', '-ResY=720')
Write-Host 'Prototype launch requested. This runs the game through Unreal; it is not a packaged executable.'
