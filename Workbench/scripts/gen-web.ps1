# ============================================================================
#  gen-web.ps1 — Generate and build the Web (HTTP/REST/SSE) shell on Windows
#
#  The Web shell is plain C++ (cpp-httplib) with no Qt dependency, so it
#  builds on Windows exactly like it does on macOS and Linux; the Aria HTTP
#  adapter already links ws2_32 for platform sockets. This script is the
#  PowerShell twin of gen-web.sh so Windows contributors get the same entry
#  point instead of needing a bash shell.
#
#  Usage:
#    pwsh Workbench/scripts/gen-web.ps1              # configure + build
#    pwsh Workbench/scripts/gen-web.ps1 run          # build, then serve
#    pwsh Workbench/scripts/gen-web.ps1 probe        # build, then verify the wire protocol
#    pwsh Workbench/scripts/gen-web.ps1 clean        # wipe build/web
# ============================================================================
param(
    [string]$Mode = "build"
)

$ErrorActionPreference = "Stop"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$WbRoot    = Split-Path -Parent $ScriptDir
$RepoRoot  = Split-Path -Parent $WbRoot
$BuildDir  = Join-Path $RepoRoot "build\web"

switch ($Mode) {
    "clean" {
        if (Test-Path $BuildDir) { Remove-Item -Recurse -Force $BuildDir }
        exit 0
    }
    { $_ -in "build", "run", "probe" } { }
    default {
        Write-Host "unknown mode: $Mode"
        Write-Host "valid: build | run | probe | clean"
        exit 1
    }
}

$Jobs = $env:NUMBER_OF_PROCESSORS
if (-not $Jobs) { $Jobs = 4 }

& cmake -S $WbRoot -B $BuildDir `
    -DCMAKE_BUILD_TYPE=Debug `
    -DWORKBENCH_TARGET_WEB=ON `
    -DWORKBENCH_TARGET_QT=OFF
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

& cmake --build $BuildDir -j $Jobs
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

# Single-config generators put the binary directly in platform/web/;
# the Visual Studio generator adds a per-config subdirectory.
$Bin = Join-Path $BuildDir "platform\web\workbench_web.exe"
if (-not (Test-Path $Bin)) {
    $alt = Join-Path $BuildDir "platform\web\Debug\workbench_web.exe"
    if (Test-Path $alt) { $Bin = $alt }
}
if (-not (Test-Path $Bin)) {
    Write-Error "[gen-web] built binary not found: $Bin"
    exit 1
}

if ($Mode -eq "probe") {
    & $Bin --probe
    exit $LASTEXITCODE
}

if ($Mode -eq "run") {
    & $Bin
    exit $LASTEXITCODE
}

Write-Host "[gen-web] Build complete: $Bin"
Write-Host "[gen-web] Run:   pwsh Workbench/scripts/gen-web.ps1 run"
Write-Host "[gen-web] Probe: pwsh Workbench/scripts/gen-web.ps1 probe"
