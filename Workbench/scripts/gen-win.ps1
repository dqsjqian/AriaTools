# ============================================================================
#  gen-win.ps1 — One-shot generate and build Windows (Qt6 + MSVC) project
#
#  Shares the platform/qt/ View layer with the Mac build; only the toolchain differs.
#  All toolchain tricks reuse the proven logic from Aria build-msvc.ps1:
#    - vswhere dynamically probes VS version (supports 2022/2026, no hardcoding)
#    - Windows Kits path is read from the registry (no hardcoded C drive)
#    - Fixes MSBuild v180 PATH case-sensitivity conflict bug
#    - Default generator is Ninja (parallel by default, sidesteps the MSBuild v18
#      MSB4166 child-node timeout crash); falls back to the VS multi-config generator
#      when ninja is absent, or when $env:ARIA_VS_GENERATOR is set.
#    - Windows SDK bin\x64 is added to PATH so Ninja can locate rc.exe/mt.exe
#      (the VS generator finds these internally; Ninja relies on PATH).
#    - Qt6 auto-detection (D:\worksoft\Qt, etc.)
#
#  Usage:
#    pwsh Workbench/scripts/gen-win.ps1              # configure + build (Release)
#    pwsh Workbench/scripts/gen-win.ps1 debug         # Debug
#    pwsh Workbench/scripts/gen-win.ps1 clean         # clean build/win
#    pwsh Workbench/scripts/gen-win.ps1 run           # run after build
#    pwsh Workbench/scripts/gen-win.ps1 tests         # build + ctest module tests
#
#  Environment variables (optional):
#    $env:QT_DIR="D:\worksoft\Qt\6.11.1\msvc2022_64"  # specify Qt6 prefix
#    $env:ARIA_VS_GENERATOR="Visual Studio 18 2026"  # override CMake generator
#    $env:ARIA_NO_QT6="1"                             # disable Qt6 (build core only)
# ============================================================================
param(
    [string]$Mode = "release"
)

$ErrorActionPreference = "Stop"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$WB_ROOT   = Split-Path -Parent $ScriptDir          # Workbench/
$REPO_ROOT = Split-Path -Parent $WB_ROOT             # repository root
$BUILD_DIR = Join-Path $REPO_ROOT "build/win"

$OriginalDir = Get-Location

try {
    # ── Clear MSYS2/GCC environment variables (would break MSVC detection) ────
    foreach ($e in @("INCLUDE", "LIB", "CPATH", "C_INCLUDE_PATH", "CPLUS_INCLUDE_PATH")) {
        $val = [Environment]::GetEnvironmentVariable($e)
        if ($val -and $val -match "msys64|mingw") {
            [Environment]::SetEnvironmentVariable($e, $null)
        }
    }

    # ── Fix MSBuild v180 (VS 2026) Path/PATH case-sensitivity conflict ──────
    # .NET ProcessStartInfo.EnvironmentVariables is a case-sensitive StringDictionary;
    # if the process environment block has multiple variants of Path/PATH/path at once
    # (some parent processes create these), MSBuild throws ArgumentException "Item has been added"
    # when invoking CL.exe. Fix: merge into a single uppercase PATH.
    $pathVariants = [System.Environment]::GetEnvironmentVariables().Keys | Where-Object { $_ -ieq "path" }
    if ($pathVariants.Count -gt 1) {
        $pathValue = [Environment]::GetEnvironmentVariable("PATH")
        foreach ($v in $pathVariants) {
            [Environment]::SetEnvironmentVariable($v, $null)
        }
        [Environment]::SetEnvironmentVariable("PATH", $pathValue)
    }

    # ── Mode dispatch ───────────────────────────────────────────────────────
    switch ($Mode) {
        "clean" {
            Write-Host "[gen-win] Cleaning $BUILD_DIR"
            if (Test-Path $BUILD_DIR) { Remove-Item -Recurse -Force $BUILD_DIR }
            exit 0
        }
        { $_ -in "release", "debug", "run", "tests" } { }
        default {
            Write-Host "unknown mode: $Mode"
            Write-Host "valid: release | debug | run | tests | clean"
            exit 1
        }
    }

    # ── Locate cmake (avoid the MSYS2 one — it would translate to POSIX paths) ─
    $cmake = Get-Command cmake.exe -ErrorAction SilentlyContinue
    if (-not $cmake) { $cmake = Get-Command cmake -ErrorAction SilentlyContinue }
    if (-not $cmake) { Write-Error "cmake not found. https://cmake.org/download/"; exit 1 }
    $cmakePath = $cmake.Source
    Write-Host "[gen-win] cmake : $cmakePath"

    # ── Auto-detect Visual Studio (vswhere, supports 2022/2026, no hardcoding) ─
    $vsWhere = "C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe"
    if (-not (Test-Path $vsWhere)) {
        $vsWhere2 = Join-Path $env:ProgramFiles "Microsoft Visual Studio\Installer\vswhere.exe"
        if (Test-Path $vsWhere2) { $vsWhere = $vsWhere2 }
    }

    $vsPath = $null; $vsMajor = $null; $vsYear = $null
    if (Test-Path $vsWhere) {
        $vsPath = & $vsWhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath 2>$null
        if (-not $vsPath) {
            $vsPath = & $vsWhere -latest -products * -property installationPath 2>$null
        }
        if ($vsPath) {
            $vsVer = & $vsWhere -latest -products * -property installationVersion 2>$null
            if ($vsVer -match '^(\d+)') { $vsMajor = $matches[1] }
            $vsName = & $vsWhere -latest -products * -property displayName 2>$null
            if ($vsName -match '(\d{4})\s*$') { $vsYear = $matches[1] }
        }
    }
    if (-not $vsPath) {
        # Fallback: known install locations
        $fallbackRoots = @(
            "D:\worksoft\VS2026", "D:\worksoft\VS2022",
            "C:\Program Files\Microsoft Visual Studio\2026\Professional",
            "C:\Program Files\Microsoft Visual Studio\2026\Enterprise",
            "C:\Program Files\Microsoft Visual Studio\2026\Community",
            "C:\Program Files\Microsoft Visual Studio\2022\Professional",
            "C:\Program Files\Microsoft Visual Studio\2022\Enterprise",
            "C:\Program Files\Microsoft Visual Studio\2022\Community"
        )
        foreach ($p in $fallbackRoots) {
            if (Test-Path (Join-Path $p "VC\Auxiliary\Build\vcvars64.bat")) {
                $vsPath = $p
                if ($p -match 'VS(20\d{2})') {
                    $vsYear = $matches[1]
                    $vsMajor = if ($vsYear -eq '2026') { 18 } elseif ($vsYear -eq '2022') { 17 } else { 17 }
                }
                break
            }
        }
    }

    if ($vsPath) {
        $vsLabel = if ($vsYear) { " ($vsYear)" } else { "" }
        Write-Host "[gen-win] VS    : $vsPath$vsLabel"

        # Find the latest MSVC toolchain
        $msvcDirs = Get-ChildItem "$vsPath\VC\Tools\MSVC" -Directory -ErrorAction SilentlyContinue | Sort-Object Name -Descending
        if (-not $msvcDirs) {
            Write-Error "VS found at $vsPath but VC\Tools\MSVC is empty. Install the 'MSVC v143/v144 - VS 2022/2026 C++ x64/x86 build tools' component."
            exit 1
        }
        $msvc = $msvcDirs | Select-Object -First 1

        # Windows Kits: read from registry (no hardcoded C drive)
        $kitsRoot = $null
        try {
            $reg = Get-ItemProperty -Path "HKLM:\SOFTWARE\Microsoft\Windows Kits\Installed Roots" -Name KitsRoot10 -ErrorAction Stop
            if ($reg.KitsRoot10) { $kitsRoot = $reg.KitsRoot10.TrimEnd('\') }
        } catch { }
        if (-not $kitsRoot) {
            foreach ($cand in @("C:\Program Files (x86)\Windows Kits\10", "D:\Windows Kits\10", "C:\Windows Kits\10")) {
                if (Test-Path $cand) { $kitsRoot = $cand; break }
            }
        }
        if (-not $kitsRoot) {
            Write-Error "Windows Kits not found. Install the Windows 10/11 SDK."
            exit 1
        }
        # SDK version directories live under Include/, not at the root
        $kitsDirs = Get-ChildItem "$kitsRoot\Include" -Directory -ErrorAction SilentlyContinue | Where-Object { $_.Name -match '^\d' } | Sort-Object Name -Descending
        if (-not $kitsDirs) {
            Write-Error "No SDK version directory under Include/ in Windows Kits ($kitsRoot). Install the Windows 10/11 SDK."
            exit 1
        }
        $kitsVer = ($kitsDirs | Select-Object -First 1).Name
        $clDir = "$vsPath\VC\Tools\MSVC\$($msvc.Name)\bin\Hostx64\x64"

        # PATH must include: cl.exe dir, Windows SDK bin (rc.exe/mt.exe for resource embedding —
        # the VS generator locates these internally, but Ninja relies on PATH), and the VS tooling dirs.
        $env:PATH    = "$clDir;$kitsRoot\bin\$kitsVer\x64;$vsPath\Common7\IDE;$vsPath\MSBuild\Current\Bin;$env:PATH"
        $env:INCLUDE = "$($msvc.FullName)\include;$kitsRoot\Include\$kitsVer\ucrt;$kitsRoot\Include\$kitsVer\um;$kitsRoot\Include\$kitsVer\shared"
        $env:LIB     = "$($msvc.FullName)\lib\x64;$kitsRoot\Lib\$kitsVer\ucrt\x64;$kitsRoot\Lib\$kitsVer\um\x64"

        Write-Host "[gen-win] MSVC  : $($msvc.Name)"
        Write-Host "[gen-win] SDK   : $kitsVer ($kitsRoot)"
    } else {
        Write-Warning "Visual Studio not found; CMake may be unable to locate the MSVC compiler."
    }

    # ── Derive the CMake generator: prefer Ninja (faster, parallel by default, sidesteps the MSBuild MSB4166 bug) ──
    # Ninja is a single-config generator; the VS generator is multi-config. The choice affects:
    #   - configure: Ninja needs -DCMAKE_BUILD_TYPE=<cfg> at configure time.
    #   - build:     Ninja parallelizes by default (no /m flag); VS generator used /m:1 to dodge MSB4166.
    #   - exe path:  Ninja -> bin/workbench.exe; VS -> bin/<Config>/workbench.exe.
    $NinjaExe = $null
    if ($env:ARIA_VS_GENERATOR) {
        # Explicit override: user wants the VS multi-config generator.
        $Generator = $env:ARIA_VS_GENERATOR
        $UseNinja = $false
    } else {
        # Locate ninja: try PATH first, then the copy bundled with VS.
        $ninjaCmd = Get-Command ninja -ErrorAction SilentlyContinue
        if ($ninjaCmd) { $NinjaExe = $ninjaCmd.Source }
        if (-not $NinjaExe -and $vsPath) {
            $bundled = Join-Path $vsPath "Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja\ninja.exe"
            if (Test-Path $bundled) { $NinjaExe = $bundled }
        }
        if ($NinjaExe) {
            $Generator = "Ninja"
            $UseNinja = $true
            # Ensure ninja is on PATH for CMake's compiler/generator probes.
            $ninjaDir = Split-Path -Parent $NinjaExe
            if ($env:PATH -notlike "*$ninjaDir*") { $env:PATH = "$ninjaDir;$env:PATH" }
        } else {
            # Fallback: VS multi-config generator.
            $UseNinja = $false
            if ($vsMajor -and $vsYear) {
                $Generator = "Visual Studio $vsMajor $vsYear"
            } else {
                $Generator = "Visual Studio 17 2022"
                Write-Warning "Could not determine VS version and ninja not found; defaulting to '$Generator'. Override via `$env:ARIA_VS_GENERATOR."
            }
        }
    }
    Write-Host "[gen-win] Gen   : $Generator"
    if ($UseNinja) { Write-Host "[gen-win] Ninja : $NinjaExe" }

    # ── Qt6 auto-detection ───────────────────────────────────────────────
    function Find-Qt6 {
        if ($env:ARIA_NO_QT6 -eq "1") { return $null }
        if ($env:QT_DIR) {
            if (Test-Path (Join-Path $env:QT_DIR "lib\cmake\Qt6\Qt6Config.cmake")) {
                return $env:QT_DIR
            }
        }
        $roots = @("D:\worksoft\Qt", "C:\Qt", "D:\Qt")
        $kitOrder = @("msvc2022_64", "msvc2019_64", "mingw_64")
        foreach ($root in $roots) {
            if (-not (Test-Path $root)) { continue }
            $versions = Get-ChildItem $root -Directory -ErrorAction SilentlyContinue |
                        Where-Object { $_.Name -match '^6\.' } | Sort-Object Name -Descending
            foreach ($v in $versions) {
                foreach ($kit in $kitOrder) {
                    $p = Join-Path $v.FullName $kit
                    if (Test-Path (Join-Path $p "lib\cmake\Qt6\Qt6Config.cmake")) {
                        return $p
                    }
                }
            }
        }
        return $null
    }

    $Qt6Dir = Find-Qt6
    if (-not $Qt6Dir) {
        Write-Error "Qt6 (msvc2022_64) not found. Set `$env:QT_DIR or install Qt to D:\worksoft\Qt / C:\Qt."
        exit 1
    }
    Write-Host "[gen-win] Qt6   : $Qt6Dir"

    # ── Build type ───────────────────────────────────────────────────────────
    $BuildConfig = if ($Mode -eq "debug") { "Debug" } else { "Release" }

    # ── CMake configure ──────────────────────────────────────────────────
    $CMakeOpts = @(
        "-DWORKBENCH_TARGET_QT=ON",
        "-DCMAKE_PREFIX_PATH=$Qt6Dir"
    )
    # Ninja is a single-config generator: the build type must be chosen at configure time.
    if ($UseNinja) { $CMakeOpts += "-DCMAKE_BUILD_TYPE=$BuildConfig" }

    Write-Host ""
    Write-Host "[gen-win] Configuring ($BuildConfig)..."
    $cfgArgs = @("-S", $WB_ROOT, "-B", $BUILD_DIR, "-G", $Generator) + $CMakeOpts
    & $cmakePath @cfgArgs
    if ($LASTEXITCODE -ne 0) { Write-Error "CMake configuration failed"; exit $LASTEXITCODE }

    # ── Build ───────────────────────────────────────────────────────────
    Write-Host ""
    Write-Host "[gen-win] Building ($BuildConfig)..."
    if ($UseNinja) {
        # Ninja parallelizes across all logical cores by default; no /m flag needed.
        # Optionally cap concurrency via $env:ARIA_BUILD_JOBS (e.g. "8") if RAM is tight.
        $buildArgs = @("--build", $BUILD_DIR)
        if ($env:ARIA_BUILD_JOBS) { $buildArgs += @("--parallel", $env:ARIA_BUILD_JOBS) }
        & $cmakePath @buildArgs
    } else {
        # MSBuild v18 (VS 2026) has the MSB4166 child-node timeout crash bug; force /m:1 single process to work around it
        $env:MSBUILDDISABLENODEREUSE = "1"
        & $cmakePath --build $BUILD_DIR --config $BuildConfig -- /m:1
    }
    if ($LASTEXITCODE -ne 0) { Write-Error "Build failed"; exit $LASTEXITCODE }

    # ── Locate the executable ───────────────────────────────────────────
    # RUNTIME_OUTPUT_DIRECTORY is set to bin/ so the exe sits beside aria_*.dll.
    #   Ninja (single-config): build/win/bin/workbench.exe
    #   VS    (multi-config):  build/win/bin/<Config>/workbench.exe
    $ExePath = if ($UseNinja) {
        Join-Path $BUILD_DIR "bin/workbench.exe"
    } else {
        Join-Path $BUILD_DIR "bin/$BuildConfig/workbench.exe"
    }
    if (-not (Test-Path $ExePath)) {
        # Fallback: older layout before RUNTIME_OUTPUT_DIRECTORY was set
        $alt = if ($UseNinja) {
            Join-Path $BUILD_DIR "platform/qt/workbench.exe"
        } else {
            Join-Path $BUILD_DIR "platform/qt/$BuildConfig/workbench.exe"
        }
        if (Test-Path $alt) { $ExePath = $alt }
    }

    # ── Run the executable ──────────────────────────────────────────────
    if ($Mode -eq "run" -and (Test-Path $ExePath)) {
        Write-Host ""
        Write-Host "[gen-win] Launching app..."
        # Add Qt bin and VC runtime to PATH, otherwise the exe cannot find Qt6Core.dll etc. at startup
        $runPath = @(
            (Join-Path $Qt6Dir "bin")
        )
        if ($vsPath) {
            $msvcDirs2 = Get-ChildItem "$vsPath\VC\Tools\MSVC" -Directory -ErrorAction SilentlyContinue | Sort-Object Name -Descending
            if ($msvcDirs2) { $runPath += (Join-Path $msvcDirs2[0].FullName "bin\Hostx64\x64") }
        }
        $env:PATH = ($runPath -join ";") + ";" + $env:PATH
        & $ExePath
        exit $LASTEXITCODE
    }

    # ── Module tests ────────────────────────────────────────────────────
    if ($Mode -eq "tests") {
        Write-Host ""
        Write-Host "[gen-win] Running ctest..."
        $testPath = @(
            (Join-Path $BUILD_DIR (if ($UseNinja) { "bin" } else { "bin/$BuildConfig" }))
        )
        $testPath += (Join-Path $Qt6Dir "bin")
        if ($vsPath) {
            $msvcDirs2 = Get-ChildItem "$vsPath\VC\Tools\MSVC" -Directory -ErrorAction SilentlyContinue | Sort-Object Name -Descending
            if ($msvcDirs2) { $testPath += (Join-Path $msvcDirs2[0].FullName "bin\Hostx64\x64") }
        }
        $env:PATH = ($testPath -join ";") + ";" + $env:PATH

        $ctestPath = Join-Path (Split-Path -Parent $cmakePath) "ctest.exe"
        if (-not (Test-Path $ctestPath)) {
            $ctestCmd = Get-Command ctest -ErrorAction SilentlyContinue
            if ($ctestCmd) { $ctestPath = $ctestCmd.Source }
        }
        if (-not (Test-Path $ctestPath)) {
            Write-Error "ctest not found (neither beside cmake nor on PATH)."
            exit 1
        }
        $proc = Start-Process $ctestPath -ArgumentList @("--test-dir", $BUILD_DIR, "-C", $BuildConfig, "--output-on-failure") -NoNewWindow -Wait -PassThru
        if ($proc.ExitCode -ne 0) { Write-Error "Tests failed"; exit $proc.ExitCode }
    }

    Write-Host ""
    Write-Host "[gen-win] ✅ Build complete: $ExePath"

} finally {
    Set-Location $OriginalDir
}
