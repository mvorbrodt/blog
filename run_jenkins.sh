#!/bin/zsh
set -e

# pull in environment variables like $PATH etc..
#
source /Users/martin/.zshrc
#
# ------------------------------

# update brew packages used by the code-base...
#
brew update
brew upgrade
brew cleanup
#
# ------------------------------

# update vcpkg packages used by the code-base...
#
vcpkg update
vcpkg upgrade
vcpkg remove --outdated
#
# ------------------------------

# make latest installed llvm/clang the compiler of choice...
#
export PATH="/usr/local/bin:$PATH"
export CC=/usr/local/bin/clang
export CXX=/usr/local/bin/clang++
export LD=/usr/local/bin/ld.lld
export AR=/usr/local/bin/llvm-ar
export RANLIB=/usr/local/bin/llvm-ranlib
export LDFLAGS="-L/usr/local/lib -Wl,-rpath,/usr/local/lib"
export CPPFLAGS="-I/usr/local/include -I/usr/local/include/c++/v1/"
export CXXFLAGS="$CPPFLAGS"
alias cc=$CC
alias c++=$CXX
alias ld=$LD
alias ar=$AR
alias ranlib=$RANLIB
#
# ------------------------------

# use llvm/clang cmake configuration...
#
ln -sf CMakeLists.txt.llvm CMakeLists.txt
#
# ------------------------------

# use llvm/clang compatible list of cmake targets...
#
cd src
ln -sf CMakeLists.txt.llvm CMakeLists.txt
cd ..
#
# ------------------------------

# generate makefiles from llvm/clang cmake configuration...
#
rm -rf jenkins
mkdir jenkins
cd jenkins
cmake ..
#
# ------------------------------

# build it~!!!!!111oneone
#
make
cd ..
#
# ------------------------------

# shit be gone!
#
exit 0
#
# ------------------------------
