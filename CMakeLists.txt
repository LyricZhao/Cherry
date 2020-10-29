cmake_minimum_required(VERSION 3.17)
project(Cherry)

set(CMAKE_CXX_STANDARD 17)

# Install GoogleTest
include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.8.0
)

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()

# Add include directory
include_directories(include)

# Add tests
add_executable(test_all tests/test_all.cpp)
target_link_libraries(test_all gtest gtest_main)