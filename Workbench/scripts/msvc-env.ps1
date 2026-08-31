# ============================================================================
#  msvc-env.ps1 — Dot-source helper that bootstraps the MSVC toolchain for
#  Ninja builds (shared by gen-win.ps1 / gen-web.ps1 style scripts).
#
#  The "Visual Studio" CMake generator locates cl.exe/rc.exe internally, but a
#  Ninja build needs the toolchain on PATH. This helper:
#    - finds VS via vswhere (supports 2022/2026, no hardcoded install)
#    - picks the latest MSVC toolchain under VC\Tools\MSVC
#    - reads the Windows Kits root from the registry (no hardcoded C drive)
#    - prepends the right dirs to PATH / INCLUDE / LIB
#
#  Usage:
#    . "$PSScriptRoot\msvc-env.ps1"
#    if (Initialize-MsvcToolchain) { <configure + build with Ninja> }
#
#  On success also sets:
#    $script:MSVC_VS_PATH          VS install root
#    $script:MSVC_TOOLCHAIN_DIR    dir containing cl.exe
# ============================================================================

function Initialize-MsvcToolchain {
    # -- Locate Visual Studio (vswhere, supports 2022/2026) -----------------
    $vsWhere = "C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe"
    if (-not (Test-Path $vsWhere)) {
        $alt = Join-Path $env:ProgramFiles "Microsoft Visual Studio\Installer\vswhere.exe"
        if (Test-Path $alt) { $vsWhere = $alt }
    }

    $vsPath = $null
    if (Test-Path $vsWhere) {
        $vsPath = & $vsWhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath 2>$null
        if (-not $vsPath) {
            $vsPath = & $vsWhere -latest -products * -property installationPath 2>$null
        }
    }
    if (-not $vsPath) {
        foreach ($p in @(
            "D:\worksoft\VS2026", "D:\worksoft\VS2022",
            "C:\Program Files\Microsoft Visual Studio\2026\Professional",
            "C:\Program Files\Microsoft Visual Studio\2026\Enterprise",
            "C:\Program Files\Microsoft Visual Studio\2026\Community",
            "C:\Program Files\Microsoft Visual Studio\2022\Professional",
            "C:\Program Files\Microsoft Visual Studio\2022\Enterprise",
            "C:\Program Files\Microsoft Visual Studio\2022\Community")) {
            if (Test-Path (Join-Path $p "VC\Auxiliary\Build\vcvars64.bat")) { $vsPath = $p; break }
        }
    }
    if (-not $vsPath) { return $false }

    $msvcDirs = Get-ChildItem "$vsPath\VC\Tools\MSVC" -Directory -ErrorAction SilentlyContinue |
                Sort-Object Name -Descending
    if (-not $msvcDirs) { return $false }
    $msvc = $msvcDirs | Select-Object -First 1

    # -- Windows Kits: read from registry (no hardcoded C drive) ------------
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
    if (-not $kitsRoot) { return $false }

    # SDK version dirs live under Include/, not at the root
    $kitsDirs = Get-ChildItem "$kitsRoot\Include" -Directory -ErrorAction SilentlyContinue |
                Where-Object { $_.Name -match '^\d' } | Sort-Object Name -Descending
    if (-not $kitsDirs) { return $false }
    $kitsVer = ($kitsDirs | Select-Object -First 1).Name

    # -- Put the toolchain on PATH/INCLUDE/LIB ------------------------------
    $script:MSVC_VS_PATH = $vsPath
    $script:MSVC_TOOLCHAIN_DIR = "$vsPath\VC\Tools\MSVC\$($msvc.Name)\bin\Hostx64\x64"

    $env:PATH    = "$($script:MSVC_TOOLCHAIN_DIR);$kitsRoot\bin\$kitsVer\x64;$vsPath\Common7\IDE;$vsPath\MSBuild\Current\Bin;$env:PATH"
    $env:INCLUDE = "$($msvc.FullName)\include;$kitsRoot\Include\$kitsVer\ucrt;$kitsRoot\Include\$kitsVer\um;$kitsRoot\Include\$kitsVer\shared"
    $env:LIB     = "$($msvc.FullName)\lib\x64;$kitsRoot\Lib\$kitsVer\ucrt\x64;$kitsRoot\Lib\$kitsVer\um\x64"

    Write-Host "[msvc-env] VS    : $vsPath"
    Write-Host "[msvc-env] MSVC  : $($msvc.Name)"
    Write-Host "[msvc-env] SDK   : $kitsVer ($kitsRoot)"
    return $true
}
