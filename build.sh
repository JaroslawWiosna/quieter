set -xe

CXXFLAGS="-std=c++17 -ggdb3 -O0 -Wall -Wextra -pedantic"
LIBS="-lstdc++fs"

$CXX $CXXFLAGS -o quieter src/scu.cpp $LIBS

