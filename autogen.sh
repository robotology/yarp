#!/bin/sh

echo "Checking for CMake..."
which cmake || {
    echo "CMake is required to configure this project."
    echo "It is available at http://www.cmake.org"
    echo "or a package \"cmake\" on most distributions."
    exit 1
}

echo "Creating and entering build directory"
mkdir -p build || {
    echo "Failed to create a build directory"
    exit 1
}
cd build
cmake .. && {
    echo " "
    echo "Successful configuration! Compile with:"
    echo "  cd build"
    echo "  make"
    echo "Report problems to robotcub-hackers@lists.sourceforge.net"
}