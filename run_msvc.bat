@echo off

set VCPKG_TARGET_TRIPLET=x64-windows
vcpkg install tbb parallelstl boost botan range-v3 benchmark catch2 hayai nonius celero fmt cryptopp wolfssl libmysql thrift protobuf grpc lz4 openssl curl curlpp

echo. & echo. & echo.
pause

copy /Y CMakeLists.txt.msvc CMakeLists.txt
copy /Y src\CMakeLists.txt.msvc src\CMakeLists.txt

del /F /S /Q msvc
del /F /Q msvc

mkdir msvc
cd msvc

cmake -DCMAKE_TOOLCHAIN_FILE=C:/Code/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows ..

cd ..
echo. & echo. & echo.
pause

msbuild -m msvc/blog.sln -t:Rebuild -p:Configuration=Debug
REM msbuild -m msvc/blog.sln -t:Rebuild -p:Configuration=Release

echo. & echo. & echo.
pause
