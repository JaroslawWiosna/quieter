set -xe

CXXFLAGS="-std=c++17 -ggdb3 -O0 -Wall -Wextra -pedantic -I./3rd_party/"
LIBS="-lstdc++fs"

$CXX $CXXFLAGS -o quieter main.cpp $LIBS

