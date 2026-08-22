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
#    pwsh Workbench/scripts/gen-web.ps1              # configure + build (Release)
#    pwsh Workbench/scripts/gen-web.ps1 debug         # Debug
#    pwsh Workbench/scripts/gen-web.ps1 run           # build, then serve
#    pwsh Workbench/scripts/gen-web.ps1 probe         # build, then verify the wire protocol
#    pwsh Workbench/scripts/gen-web.ps1 clean         # wipe build/web
#
#  Output goes flat into build/web/bin/ (exe + aria_*.dll together). Switching
#  build type or generator automatically wipes build/web first — a flat bin/
#  cannot hold two builds side by side.
#
#  Generator: prefers Ninja (single-config -> flat bin/workbench_web.exe, same
#  layout as the win build and the CI job), falling back to the default Visual
#  Studio multi-config generator (-> bin/<Config>/workbench_web.exe) when ninja
#  is unavailable. Ninja builds bootstrap the MSVC toolchain via the shared
#  msvc-env.ps1 helper (the VS generator locates cl.exe internally). Override
#  with $env:ARIA_VS_GENERATOR="Visual Studio 18 2026".
# ============================================================================
param(
    [string]$Mode = "release"
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
    { $_ -in "release", "debug", "run", "probe" } { }
    default {
        Write-Host "unknown mode: $Mode"
        Write-Host "valid: release | debug | run | probe | clean"
        exit 1
    }
}

# ── Build type: default Release; Debug only when explicitly requested ──────
$BuildConfig = if ($Mode -eq "debug") { "Debug" } else { "Release" }

# ── Generator: prefer Ninja (single-config) so the exe lands flat in bin/
#    like the win build; fall back to the default VS multi-config generator. ──
$NinjaExe = $null
if ($env:ARIA_VS_GENERATOR) {
    $Generator = $env:ARIA_VS_GENERATOR
} else {
    $ninjaCmd = Get-Command ninja -ErrorAction SilentlyContinue
    if ($ninjaCmd) { $NinjaExe = $ninjaCmd.Source }
    if (-not $NinjaExe) {
        $ninjaCandidates = @(
            "C:\Program Files\Microsoft Visual Studio\2026\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe",
            "C:\Program Files\Microsoft Visual Studio\2026\Professional\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe",
            "C:\Program Files\Microsoft Visual Studio\2026\Enterprise\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe",
            "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe",
            "D:\worksoft\VS2026\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe",
            "D:\worksoft\VS2022\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe"
        )
        foreach ($p in $ninjaCandidates) {
            if (Test-Path $p) { $NinjaExe = $p; break }
        }
    }
    if ($NinjaExe) {
        $Generator = "Ninja"
        $ninjaDir = Split-Path -Parent $NinjaExe
        if ($env:PATH -notlike "*$ninjaDir*") { $env:PATH = "$ninjaDir;$env:PATH" }
    }
}
# Ninja cannot find cl.exe by itself (unlike the VS generator). Bootstrap the
# MSVC toolchain; if that fails, fall back to the default generator.
if ($Generator -eq "Ninja" -and -not (Get-Command cl.exe -ErrorAction SilentlyContinue)) {
    . "$PSScriptRoot\msvc-env.ps1"
    if (-not (Initialize-MsvcToolchain)) {
        Write-Warning "[gen-web] MSVC toolchain not found; Ninja cannot find cl.exe. Falling back to the default (multi-config) generator -> bin/<Config>/ layout."
        $Generator = $null
    }
}
if ($Generator) { Write-Host "[gen-web] Gen   : $Generator" }

# ── Flat bin/ cannot host multiple builds side by side: auto-clean on change ──
# Marker records "generator|config"; wipe the build dir when either differs
# (e.g. an existing VS-generated dir + a new Ninja run, or release -> debug).
$MarkerFile = Join-Path $BuildDir ".last-config"
$BuildStamp = "$(if ($Generator) { $Generator } else { 'Default' })|$BuildConfig"
$LastStamp  = if (Test-Path $MarkerFile) { (Get-Content $MarkerFile -Raw).Trim() } else { $null }
if ($LastStamp -and $LastStamp -ne $BuildStamp) {
    Write-Host "[gen-web] Build stamp changed: $LastStamp -> $BuildStamp. Cleaning build dir (flat bin/ holds one build at a time)..."
    if (Test-Path $BuildDir) { Remove-Item -Recurse -Force $BuildDir }
}

$CfgArgs = @("-S", $WbRoot, "-B", $BuildDir)
if ($Generator) { $CfgArgs += @("-G", $Generator) }
$CfgArgs += @(
    "-DCMAKE_BUILD_TYPE=$BuildConfig",
    "-DWORKBENCH_TARGET_WEB=ON",
    "-DWORKBENCH_TARGET_QT=OFF"
)
& cmake @CfgArgs
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

if ($Generator -eq "Ninja") {
    # Ninja is a single-config generator: parallel by default, no --config needed.
    $buildArgs = @("--build", $BuildDir)
    if ($env:ARIA_BUILD_JOBS) { $buildArgs += @("--parallel", $env:ARIA_BUILD_JOBS) }
    & cmake @buildArgs
} else {
    # Multi-config generator: build only the configured config.
    & cmake --build $BuildDir --config $BuildConfig
}
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

# Record generator|config so the next run can detect a switch.
Set-Content -Path $MarkerFile -Value $BuildStamp

# Flat bin/ is the norm (Ninja); the <Config> subdir only appears with a
# multi-config generator fallback.
$Bin = Join-Path $BuildDir "bin\workbench_web.exe"
if (-not (Test-Path $Bin)) {
    $alt = Join-Path $BuildDir "bin\$BuildConfig\workbench_web.exe"
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
