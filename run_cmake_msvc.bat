@echo off

copy /Y CMakeLists.txt.msvc CMakeLists.txt
copy /Y src\CMakeLists.txt.msvc src\CMakeLists.txt

del /F /S /Q msvc
del /F /Q msvc
mkdir msvc
cd msvc

cmake -DCMAKE_TOOLCHAIN_FILE=C:\Code\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows ..
cd ..

REM msbuild -m blog.sln -t:Clean -p:Configuration=Release
REM msbuild -m blog.sln -t:Rebuild -p:Configuration=Release
REM git checkout CMakeLists.txt
REM git checkout src\CMakeLists.txt
