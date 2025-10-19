cmake_minimum_required(VERSION 3.14)
project(PieceTree)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

# PieceTree library
add_library(PieceTree STATIC
    src/PieceTree.cpp
)
target_include_directories(PieceTree PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Tests
add_executable(MyTests tests/InsertionTest.cpp
        tests/RemovingTest.cpp
        tests/Util.cpp
        tests/Util.h
        tests/GetLinePiecesTest.cpp
)
target_link_libraries(MyTests
    PRIVATE PieceTree 
    PRIVATE gtest_main
)

# CTest integration
include(GoogleTest)
gtest_discover_tests(MyTests)
