cmake_minimum_required(VERSION 3.10)
project(pfTest)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)

set(GRAMINE_DIR "/home/user/projects/gramine/")

# Add the include directory
include_directories(
    "include"
    "${GRAMINE_DIR}tools/sgx"
    "${GRAMINE_DIR}common/src/protected_files"
)

link_directories(
    "${GRAMINE_DIR}dist/lib/x86_64-linux-gnu"
)

# Add the source files
file(GLOB SOURCES "src/*.cpp")

# Add the executable
add_executable(pfTest ${SOURCES})

target_link_libraries(pfTest ${GRAMINE_DIR}/dist/lib/x86_64-linux-gnu/libsgx_util.a mbedcrypto_gramine mbedcrypto_gramine)
