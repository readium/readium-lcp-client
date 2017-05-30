#!/usr/bin/python
import os
import shutil
import platform
import utils
import urllib
import zipfile
import tarfile
import tempfile
import subprocess

# Other variables
SYSTEM = platform.system().lower()

if SYSTEM == "darwin":
    SYSTEM = "mac"

# Install gyp
def install_gyp():
    if os.path.exists(os.path.join("vendor", "gyp")):
        return

    print "Clone gyp"
    utils.execute_command(["git", "clone", "https://chromium.googlesource.com/external/gyp.git", "vendor/gyp"])

# Install ninja
def install_ninja():
    if not os.path.exists(os.path.join("vendor", "ninja")):
        print "Clone ninja"
        utils.execute_command(["git", "clone", "https://github.com/ninja-build/ninja.git", "vendor/ninja"])

    # Configure and build ninja
    if SYSTEM == "windows":
        cmd_path = (os.path.join("vendor", "ninja", "ninja.exe"))
    else:
        cmd_path = (os.path.join("vendor", "ninja", "ninja"))

    if not os.path.exists(cmd_path):
        print "Build and install ninja"

        # Initialize visual studio environment variables
        cmd = ["python", "configure.py", "--bootstrap"]

        if SYSTEM == "windows":
            cmd = ["vcvarsall.bat", "&&"] + cmd

        utils.execute_command(cmd, os.path.join("vendor", "ninja"))

# Apply patches
def apply_patches():
    patch_file_path = os.path.abspath(os.path.join("patches", "%s.diff" % SYSTEM))

    if not os.path.exists(patch_file_path):
        return
        
    print "Apply patches"
    cmd = ["git", "apply"]

    # Add options for windows to manage spaces
    if SYSTEM == "windows":
        cmd += ["--ignore-space-change", "--ignore-whitespace"]

    cmd += [patch_file_path]

    utils.execute_command(cmd, os.path.join("..", ".."))

# Download and install vendors
if not os.path.exists("vendor"):
    os.mkdir("vendor")

install_gyp()
install_ninja()

# Apply patches
apply_patches()

