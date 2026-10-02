[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)] [string] $EngineRoot,
    [ValidateSet('Win64')] [string] $Platform = 'Win64'
)
$ErrorActionPreference = 'Stop'
$taskProjectRoot = Split-Path -Parent $PSScriptRoot
$taskProject = Join-Path $taskProjectRoot 'Kurukshetra.uproject'
$taskEngineRoot = (Resolve-Path -LiteralPath $EngineRoot).Path
$taskUat = Join-Path $taskEngineRoot 'Engine\Build\BatchFiles\RunUAT.bat'
if (-not (Test-Path -LiteralPath $taskUat)) { throw "RunUAT not found under $taskEngineRoot" }
if (-not (Test-Path -LiteralPath (Join-Path $taskProjectRoot 'Content\Maps\Chapter01.umap'))) {
    throw 'Run Prepare-Project.ps1 successfully before packaging.'
}
$taskArchive = Join-Path $taskProjectRoot "Builds\$Platform"
$taskUatArgs = @('BuildCookRun', "-project=$taskProject", '-noP4', "-platform=$Platform", '-clientconfig=Development', '-build', '-cook', '-stage', '-pak', '-package', '-archive', "-archivedirectory=$taskArchive", '-utf8output')
& $taskUat @taskUatArgs
if ($LASTEXITCODE -ne 0) { throw "Packaging failed (exit $LASTEXITCODE). No successful build is implied." }
Write-Host "Development build packaged in $taskArchive"
