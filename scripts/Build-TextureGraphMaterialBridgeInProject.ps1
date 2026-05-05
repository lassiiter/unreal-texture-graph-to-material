$ErrorActionPreference = "Stop"

$repoRoot = Split-Path -Parent $PSScriptRoot
$pluginName = "TextureGraphMaterialBridge"
$projectRoot = "C:\Users\Lassiter\Documents\Unreal Projects\texturegraphtest"
$projectFile = Join-Path $projectRoot "texturegraphtest.uproject"
$pluginDestination = Join-Path $projectRoot "Plugins\$pluginName"
$candidateBuildScripts = @(
    "C:\Program Files\Epic Games\UE_5.7\Engine\Build\BatchFiles\Build.bat",
    "C:\Users\Lassiter\Documents\GitHub\UnrealEngine\Engine\Build\BatchFiles\Build.bat"
)

$buildScript = $candidateBuildScripts | Where-Object { Test-Path $_ } | Select-Object -First 1

if (-not (Test-Path $projectFile)) {
    throw "Project file not found: $projectFile"
}

if (-not $buildScript) {
    throw "No Unreal build script was found in the expected UE 5.7 locations."
}

New-Item -ItemType Directory -Force -Path $pluginDestination | Out-Null

$itemsToCopy = @(
    "Source",
    "Resources",
    "Shaders",
    "Content",
    "$pluginName.uplugin"
)

foreach ($item in $itemsToCopy) {
    $sourcePath = Join-Path $repoRoot $item
    if (-not (Test-Path $sourcePath)) {
        continue
    }

    Copy-Item -Path $sourcePath -Destination $pluginDestination -Recurse -Force
}

& $buildScript texturegraphtestEditor Win64 Development -Project="$projectFile" -WaitMutex -NoHotReloadFromIDE
exit $LASTEXITCODE
