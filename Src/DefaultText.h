#pragma once
#include <string>

// CMake Defaults:
static std::string CMAKE_DEFAULTS = R"(
    project(${script_name})
    cmake_minimum_required(VERSION 2.8)
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} --std=c++11)
    set(SCRIPT ${PROJECT_NAME})

    add_executable(${PROJECT_NAME} Script.cpp)

    # cppsh header
    find_path(CPPSH_HEADER
            NAMES           cppsh/cppsh.h
            HINTS           /usr/local/include
                            /opt/local/include)
    include_directories(${CPPSH_HEADER})
)";

// Bash build script:
static std::string BASH_SCRIPT = R"(#!/bin/bash
OLD_DIR=$PWD
BUILD_DIR=`dirname $0`

# Get into right folder
cd $BUILD_DIR

# Build it...
cmake .. -Wno-dev > /dev/null && make > /dev/null

# Check it...
if [ $? -ne 0 ]; then
    echo Compilation failed!
    exit 1
fi

# Run it!
cd $OLD_DIR
$BUILD_DIR/${script_name} ${args}

exit $?
)";