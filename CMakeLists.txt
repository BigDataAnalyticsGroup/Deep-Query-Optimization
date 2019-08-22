cmake_minimum_required(VERSION 3.1...3.14)

project(DQO VERSION 1.0
            DESCRIPTION "Vertical Prototype for Deep Query Optimization"
            LANGUAGES C CXX)

# Build flags
SET(EXECUTABLE_OUTPUT_PATH      "${PROJECT_BINARY_DIR}/bin")
SET(LIBRARY_OUTPUT_PATH         "${PROJECT_BINARY_DIR}/lib")

# Compile flags
SET(CMAKE_COMPILE_FLAGS         "-W -Wall -pedantic -march=native -Wno-gnu-anonymous-struct -Wno-nested-anon-types -Wno-c99-extensions")
SET(CMAKE_C_FLAGS               "${CMAKE_C_FLAGS} ${CMAKE_COMPILE_FLAGS}")
SET(CMAKE_CXX_FLAGS             "-std=c++17 ${CMAKE_CXX_FLAGS} ${CMAKE_COMPILE_FLAGS}")

# Directories
add_subdirectory(src/)
