#!/usr/bin/env python3
"""AiPi-SCBB menuconfig — runs kconfiglib and generates scbb_config.h.

Usage:
    python menuconfig.py
"""

import os
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent
CONFIG_FILE = ".config"


def find_menuconfig():
    """Find menuconfig executable: try Scripts/menuconfig.exe, then fallback to -m kconfiglib."""
    scripts_dir = Path(sys.executable).parent / "Scripts"
    menuconfig_exe = scripts_dir / "menuconfig.exe"
    if menuconfig_exe.exists():
        return [str(menuconfig_exe), "Kconfig"]
    return [sys.executable, "-m", "kconfiglib", "menuconfig", "Kconfig"]


cmd = find_menuconfig()
env = os.environ.copy()
env["KCONFIG_CONFIG"] = str(ROOT / CONFIG_FILE)

try:
    subprocess.check_call(cmd, cwd=str(ROOT), env=env)
except subprocess.CalledProcessError as e:
    print(f"\nmenuconfig failed (exit {e.returncode})", file=sys.stderr)
    input("Press Enter to exit...")
    sys.exit(e.returncode)
except FileNotFoundError:
    print("Error: kconfiglib not installed.\n  pip install kconfiglib windows-curses", file=sys.stderr)
    input("Press Enter to exit...")
    sys.exit(1)

if not (ROOT / CONFIG_FILE).exists():
    print("Error: .config not generated. Did you save the configuration (press S then Q)?", file=sys.stderr)
    input("Press Enter to exit...")
    sys.exit(1)

try:
    subprocess.check_call(
        [sys.executable, "scripts/gen_config_header.py", CONFIG_FILE, "scbb_config.h"],
        cwd=str(ROOT),
    )
    print("scbb_config.h generated successfully.")
except subprocess.CalledProcessError as e:
    print(f"\nHeader generation failed (exit {e.returncode})", file=sys.stderr)
    input("Press Enter to exit...")
    sys.exit(e.returncode)
