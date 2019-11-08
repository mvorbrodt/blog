@echo off
cmake -DCMAKE_TOOLCHAIN_FILE=C:\Code\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows .
