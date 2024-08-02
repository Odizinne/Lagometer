import os
from cx_Freeze import setup, Executable


src_dir = os.path.dirname(os.path.abspath(__file__))
build_dir = "build/Lagometer"

include_files = [os.path.join(src_dir, "dependencies/"), os.path.join(src_dir, "icons")]

zip_include_packages = ["PyQt6"]

build_exe_options = {
    "build_exe": build_dir,
    "include_files": include_files,
    "zip_include_packages": zip_include_packages,
    "excludes": ["tkinter"],
}

executables = [
    Executable(
        script=os.path.join(src_dir, "lagometer.py"),
        base="Win32GUI",
        icon=os.path.join(src_dir, "icons/icon.ico"),
        target_name="Lagometer",
    )
]

setup(
    name="Lagometer",
    version="1.0",
    options={"build_exe": build_exe_options},
    executables=executables,
)
