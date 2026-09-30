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
#    $script:MSVC_VS_MAJOR/YEAR    detected CMake Visual Studio generator version
# ============================================================================

function Initialize-MsvcToolchain {
    $script:MSVC_VS_PATH = $null
    $script:MSVC_VS_MAJOR = $null
    $script:MSVC_VS_YEAR = $null
    $script:MSVC_TOOLCHAIN_DIR = $null

    # Prefer an explicit Developer PowerShell installation or vswhere.
    $programRoots = @(${env:ProgramFiles(x86)}, $env:ProgramFiles) |
                    Where-Object { $_ } | Select-Object -Unique
    $vsWhereCmd = Get-Command vswhere.exe -ErrorAction SilentlyContinue
    $vsWhere = if ($vsWhereCmd) { $vsWhereCmd.Source } else { $null }
    if (-not $vsWhere) {
        foreach ($root in $programRoots) {
            $candidate = Join-Path $root "Microsoft Visual Studio\Installer\vswhere.exe"
            if (Test-Path $candidate) { $vsWhere = $candidate; break }
        }
    }

    $vsPath = $env:VSINSTALLDIR
    if ($vsPath -and -not (Test-Path $vsPath)) { $vsPath = $null }
    if (-not $vsPath -and $vsWhere) {
        $query = @("-latest", "-products", "*", "-requires", "Microsoft.VisualStudio.Component.VC.Tools.x86.x64")
        $vsPath = & $vsWhere @query -property installationPath 2>$null
        if ($vsPath) {
            $vsVersion = & $vsWhere @query -property installationVersion 2>$null
            if ($vsVersion -match '^(\d+)') {
                $script:MSVC_VS_MAJOR = [int]$matches[1]
                $script:MSVC_VS_YEAR = switch ($script:MSVC_VS_MAJOR) {
                    18 { "2026" }
                    17 { "2022" }
                }
            }
        }
    }
    if (-not $vsPath) {
        foreach ($root in $programRoots) {
            foreach ($year in @("2026", "2022")) {
                foreach ($edition in @("Professional", "Enterprise", "Community", "BuildTools")) {
                    $candidate = Join-Path $root "Microsoft Visual Studio\$year\$edition"
                    if (Test-Path (Join-Path $candidate "VC\Auxiliary\Build\vcvars64.bat")) {
                        $vsPath = $candidate
                        break
                    }
                }
                if ($vsPath) { break }
            }
            if ($vsPath) { break }
        }
    }
    if (-not $vsPath) { return $false }
    $script:MSVC_VS_PATH = $vsPath
    if (-not $script:MSVC_VS_YEAR -and $vsPath -match '(2026|2022)') {
        $script:MSVC_VS_YEAR = $matches[1]
        $script:MSVC_VS_MAJOR = if ($script:MSVC_VS_YEAR -eq "2026") { 18 } else { 17 }
    }

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
        foreach ($root in $programRoots) {
            $cand = Join-Path $root "Windows Kits\10"
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
