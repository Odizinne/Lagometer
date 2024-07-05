from cx_Freeze import setup, Executable
import sys

build_dir = "build/Lagometer"
base = None

if sys.platform == "win32":
    base = "Win32GUI"

system_icon = "icons/icon.ico" if sys.platform == "win32" else "icons/icon.png"

build_exe_options = {
    "include_files": ["icons/"],
    "build_exe": build_dir,
    "zip_include_packages": ["PyQt6", "ping3"]
}

executables = [
    Executable("lagometer.py", base=base, icon=system_icon)
]

setup(
    name="Lagometer",
    version="0.1",
    description="Lagometer",
    options={"build_exe": build_exe_options},
    executables=executables
)
