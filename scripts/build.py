#!/usr/bin/python3

import os
import subprocess
import shutil

BUILD_DIR = '../build'
BUILD_TYPE = "Release"

build_cmd = ["cmake"]
build_cmd.append("-DANDROID=true")
build_cmd.append("-DCMAKE_SYSTEM_NAME=Android")
ARCH_ABI = "arm64-v8a"
build_cmd.append("-DANDROID_ABI=" + ARCH_ABI)
build_cmd.append("-DCMAKE_ANDROID_ARCH_ABI=" + ARCH_ABI)

if os.environ.get("ANDROID_NDK") is None:
    build_cmd.append('-DCMAKE_ANDROID_NDK=/mnt/c/Users/newst/AppData/Local/Android/Sdk/ndk/linux-r20b')
else:
    build_cmd.append('-DCMAKE_ANDROID_NDK='+os.environ.get('ANDROID_NDK'))
build_cmd.append("../")

if os.path.isdir(BUILD_DIR):
    shutil.rmtree(BUILD_DIR)
os.mkdir(BUILD_DIR)
os.chdir(BUILD_DIR)

subprocess.call(build_cmd)

print("======start build!!!======")

subprocess.call(['cmake', '--build', '.', '--config', BUILD_TYPE])
