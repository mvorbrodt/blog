@echo off
copy /Y CMakeLists.win.txt CMakeLists.txt
copy /Y src\CMakeLists.win.txt src\CMakeLists.txt
cmake -DCMAKE_TOOLCHAIN_FILE=C:\Code\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows .
msbuild -m blog.sln -t:Clean
msbuild -m blog.sln -t:Rebuild -p:Configuration=Release
git checkout CMakeLists.txt
git checkout src\CMakeLists.txt
