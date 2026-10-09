#!/usr/bin/env python3
"""Unified build pipeline for AriaTools (Windows/macOS/Linux).

Complete pipeline: deps -> build -> test -> bench -> package.
Powered by aria_deps.build_kit (pip install aria-deps).

Supports native, Qt, web, iOS and Android targets.

Usage:
    python tools/build.py                           # Native release build
    python tools/build.py --platform qt             # Qt build
    python tools/build.py --platform android        # Android build
    python tools/build.py deps                      # Only dependencies
    python tools/build.py build --platform ios      # Only iOS build
"""
from __future__ import annotations

import argparse
import platform as plat
import sys
from pathlib import Path

try:
    from aria_deps.build_kit import Pipeline
    _HAS_BUILD_KIT = True
except ImportError:
    _HAS_BUILD_KIT = False
    Pipeline = None

ROOT = Path(__file__).resolve().parents[1]
WORKBENCH = ROOT / "Workbench"


def extra_args(parser: argparse.ArgumentParser):
    parser.add_argument("--platform", choices=("native", "qt", "web", "ios", "android"),
                        default="native", help="Target platform")
    parser.add_argument("--toolchain", choices=("auto", "msvc", "mingw"),
                        default="auto", help="Windows toolchain")
    parser.add_argument("--arch", help="Android ABI or Apple architecture")
    parser.add_argument("--ios-sdk", choices=("iphonesimulator", "iphoneos"),
                        default="iphonesimulator")
    parser.add_argument("--test", action="store_true", help="Run CTest after build")
    parser.add_argument("--offline", action="store_true", help="Offline dependency mode")
    parser.add_argument("--aria-root", type=Path, help="Explicit local Aria source")
    parser.add_argument("--generator-platform", help="VS target platform (x64/ARM64)")
    parser.add_argument("--no-pch", action="store_true",
                        help="Disable precompiled headers (slower but uses less disk)")


def validate(args):
    host = plat.system()
    target = args.platform
    if target == "ios" and host != "Darwin":
        raise ValueError("iOS requires macOS and Xcode")
    if args.test and target in ("ios", "android"):
        raise ValueError("Mobile execution requires simulator/device runner")
    if args.arch and target != "android" and host != "Darwin":
        raise ValueError("--arch only for Apple archs or Android ABIs")
    if target == "android" and args.arch and args.arch not in \
            ("armeabi-v7a", "arm64-v8a", "x86", "x86_64"):
        raise ValueError("Unsupported Android ABI")
    toolchain = ("msvc" if host == "Windows" else "native") \
        if args.toolchain == "auto" else args.toolchain
    if host != "Windows" and toolchain != "native":
        raise ValueError("--toolchain msvc/mingw only valid on Windows")


def build_dir_fn(args) -> Path:
    host = plat.system()
    target = args.platform
    toolchain = ("msvc" if host == "Windows" else "native") \
        if args.toolchain == "auto" else args.toolchain
    arch = args.arch or ("arm64-v8a" if target == "android"
                         else "arm64" if target == "ios" else plat.machine())
    suffix = f"{target}-{toolchain}-{args.config.lower()}-{arch}"
    if target == "ios":
        suffix += f"-{args.ios_sdk}"
    if args.generator_platform:
        suffix += f"-{args.generator_platform}"
    return (ROOT / "build" / "unified" / suffix).resolve()


def deps_list(args) -> list:
    cmds = []
    if not args.aria_root:
        fetch = [sys.executable, str(ROOT / "tools/ci/fetch_aria.py")]
        if args.offline:
            fetch.append("--offline")
        cmds.append(("aria", fetch))
    return cmds


def cmake_flags(args) -> dict:
    target = args.platform
    aria = args.aria_root.resolve() if args.aria_root else ROOT / "build/deps/aria"
    selected = "qt" if target == "native" else target
    flags = {"ARIA_DEPENDENCIES_OFFLINE": "ON" if args.offline else "OFF"}
    for name in ("qt", "web", "ios", "android"):
        flags[f"WORKBENCH_TARGET_{name.upper()}"] = "ON" if name == selected else "OFF"
    flags["ARIA_DIR"] = str(aria)
    flags["WORKBENCH_ENABLE_PCH"] = "OFF" if args.no_pch else "ON"
    return flags


def main(argv=None) -> int:
    if not _HAS_BUILD_KIT:
        print("Error: aria-deps is required. Install it with:", file=sys.stderr)
        print("    pip install aria-deps", file=sys.stderr)
        return 1
    pipeline = Pipeline(
        name="aria-tools",
        root=WORKBENCH,
        deps=deps_list,
        cmake_flags=cmake_flags,
        qt_required=True,
        extra_args=extra_args,
        build_dir_fn=build_dir_fn,
        validate_fn=validate,
    )
    return pipeline.run(argv)


if __name__ == "__main__":
    sys.exit(main())
