# Minimum version of CMake
cmake_minimum_required(VERSION 3.10)

# Set project name
project(CodeBird)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add executable
add_executable(codebird codebird.cpp)

# Optional: Add some compile-time flags if needed (e.g., for debugging)
# target_compile_options(codebird PRIVATE -g)

# Optionally, specify a specific output directory for your executable
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
