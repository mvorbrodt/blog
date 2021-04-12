@echo off
copy /Y CMakeLists.win.txt CMakeLists.txt
copy /Y src\CMakeLists.win.txt src\CMakeLists.txt
mkdir msvc
cd msvc
cmake -DCMAKE_TOOLCHAIN_FILE=C:\Code\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows ..
REM msbuild -m blog.sln -t:Clean -p:Configuration=Release
REM msbuild -m blog.sln -t:Rebuild -p:Configuration=Release
REM git checkout CMakeLists.txt
REM git checkout src\CMakeLists.txt
