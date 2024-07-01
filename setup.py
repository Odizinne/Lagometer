from cx_Freeze import setup, Executable
import sys

build_exe_options = {
    "include_files": ["design.ui"],
}

base = None
if sys.platform == "win32":
    base = "Win32GUI"

executables = [
    Executable("lagometer.py", base=base, icon=None)
]

setup(
    name="Lagometer",
    version="0.1",
    description="Lagometer",
    options={"build_exe": build_exe_options},
    executables=executables
)
