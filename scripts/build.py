#!/usr/bin/python3

import os
import subprocess
import shutil

BUILD_DIR = '../build'
BUILD_TYPE = "Debug"
Test = "On"
Target = "Linux"

os.chdir('..')
if os.path.isdir('./build'):
    shutil.rmtree('./build')
os.mkdir('./build')
os.chdir('./build')

subprocess.call(['conan', 'install', '../', '-if', '.', '--build', 'missing', '-s','build_type=' + BUILD_TYPE])

build_cmd = ["cmake"]
if (Target == 'Android'):
    build_cmd.append("-DANDROID=true")
    build_cmd.append("-DCMAKE_SYSTEM_NAME=Android")
    ARCH_ABI = "arm64-v8a"
    build_cmd.append("-DANDROID_ABI=" + ARCH_ABI)
    build_cmd.append("-DCMAKE_ANDROID_ARCH_ABI=" + ARCH_ABI)
    if os.environ.get("ANDROID_NDK") is None:
        build_cmd.append('-DCMAKE_ANDROID_NDK=/mnt/c/Users/newst/AppData/Local/Android/Sdk/ndk/linux-r20b')
    else:
        build_cmd.append('-DCMAKE_ANDROID_NDK='+os.environ.get('ANDROID_NDK'))
elif Target == 'Linux':
    build_cmd.append("-DCMAKE_SYSTEM_NAME=Linux")

if (Test == "On"):
    build_cmd.append("-DBUILD_TESTING=" + "ON")

build_cmd.append("-S../")
build_cmd.append("-B.")

subprocess.call(build_cmd)
print("======start build!!!======")
subprocess.call(['cmake', '--build', '.', '--config', BUILD_TYPE])

if (Test == "On"):
    print("======start test!!!======")
    os.chdir('./../bin')
    subprocess.call(['ctest', '--output-on-failure', '--stop-on-failure', BUILD_TYPE])