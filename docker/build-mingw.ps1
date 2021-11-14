$BUILD_DIR = "build-windows-amd64"

cmake -B $BUILD_DIR -DCMAKE_TOOLCHAIN_FILE="mingw-toolchain.cmake" -G "MinGW Makefiles" .
cmake --build $BUILD_DIR --parallel 8
