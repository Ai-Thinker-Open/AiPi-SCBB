#!/usr/bin/env python3
"""AiPi-SCBB menuconfig — runs kconfiglib and generates scbb_config.h.

Usage:
    python menuconfig.py
"""

import os
import subprocess
import sys
from pathlib import Path

# Set console window title
sys.stdout.write("\033]0;AiPi-SCBB\007")
sys.stdout.flush()

ROOT = Path(__file__).resolve().parent
CONFIG_FILE = ".config"
CONFIG_HEADER_NAME = "scbb_config.h"


def find_menuconfig():
    """Find menuconfig executable: try Scripts/menuconfig.exe, then fallback to -m kconfiglib."""
    scripts_dir = Path(sys.executable).parent / "Scripts"
    menuconfig_exe = scripts_dir / "menuconfig.exe"
    if menuconfig_exe.exists():
        return [str(menuconfig_exe), "Kconfig"]
    return [sys.executable, "-m", "kconfiglib", "menuconfig", "Kconfig"]


def find_and_remove_old_config_headers():
    """Find and remove all existing scbb_config.h files to ensure uniqueness."""
    removed = []
    for f in ROOT.rglob(CONFIG_HEADER_NAME):
        # Skip .git and .mimocode directories
        parts = f.relative_to(ROOT).parts
        if any(p.startswith(".") for p in parts):
            continue
        f.unlink()
        removed.append(str(f.relative_to(ROOT)))
    return removed


cmd = find_menuconfig()
env = os.environ.copy()
env["KCONFIG_CONFIG"] = str(ROOT / CONFIG_FILE)
env["MENUCONFIG_STYLE"] = "aquatic"

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

# Read output path from .config
output_path = CONFIG_HEADER_NAME
config_path = ROOT / CONFIG_FILE
with open(config_path, "r") as f:
    for line in f:
        line = line.strip()
        if line.startswith("CONFIG_SCBB_CONFIG_HEADER_PATH="):
            val = line.split("=", 1)[1]
            if len(val) >= 2 and val[0] == '"' and val[-1] == '"':
                val = val[1:-1]
            output_path = val
            break

# Ensure uniqueness: remove old scbb_config.h files
removed = find_and_remove_old_config_headers()
if removed:
    print(f"Removed old config headers: {', '.join(removed)}")

# Ensure output directory exists
output_full = ROOT / output_path
output_full.parent.mkdir(parents=True, exist_ok=True)

try:
    subprocess.check_call(
        [sys.executable, "scripts/gen_config_header.py", CONFIG_FILE, output_path],
        cwd=str(ROOT),
    )
    print(f"{output_path} generated successfully.")
except subprocess.CalledProcessError as e:
    print(f"\nHeader generation failed (exit {e.returncode})", file=sys.stderr)
    input("Press Enter to exit...")
    sys.exit(e.returncode)
