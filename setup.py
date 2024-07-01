from cx_Freeze import setup, Executable
import sys

build_exe_options = {
    "include_files": ["design.ui", "icons/"],
}

base = None
if sys.platform == "win32":
    base = "Win32GUI"

system_icon = None
if sys.platform == "win32":
    system_icon = "icons/icon.ico"
else:
    system_icon = "icons/icon.png"

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
