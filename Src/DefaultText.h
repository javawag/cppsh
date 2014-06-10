// CMake Defaults:
static String CMAKE_DEFAULTS = R"(
    project(${script_name})
    cmake_minimum_required(VERSION 2.8)
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} --std=c++11)

    set(SCRIPT ${PROJECT_NAME})

    add_executable(${PROJECT_NAME} Script.cpp)
)";

// C++ Defaults:
static String CPP_DEFAULTS = R"(
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <set>
#include <queue>
#include <stack>
#include <deque>
#include <functional>
#include <tuple>
#include <future>
#include <thread>

namespace cppsh {
    using FileIn = std::ifstream;
    using FileOut = std::ofstream;
    using File = std::fstream;
    using String = std::string;
    using OutputStringStream = std::ostringstream;
    using InputStringStream = std::istringstream;
    using StringStream = std::stringstream;
    using Thread = std::thread;

    using Int = int;
    using Float = float;
    using SizeType = size_t;
    using Double = double;
    using Bool = bool;
    using Char = char;
    using Void = void;

    template <typename _T1, typename _T2>               using Pair = std::pair<_T1, _T2>;
    template <typename ..._Tp>                          using Tuple = std::tuple<_Tp...>;

    template <typename _KeyType, typename _ValueType>   using Map = std::map<_KeyType, _ValueType>;
    template <typename _KeyType, typename _ValueType>   using UnorderedMap = std::unordered_map<_KeyType, _ValueType>;
    template <typename _KeyType, typename _ValueType>   using Set = std::set<_KeyType, _ValueType>;
    template <typename _KeyType, typename _ValueType>   using UnorderedSet = std::unordered_set<_KeyType, _ValueType>;

    template <typename _ElementType>                    using Vector = std::vector<_ElementType>;
    template <typename _ElementType>                    using Queue = std::queue<_ElementType>;
    template <typename _ElementType>                    using Stack = std::stack<_ElementType>;
    template <typename _ElementType>                    using Deque = std::deque<_ElementType>;

    template <typename _Fp>                             using Function = std::function<_Fp>;
    template <typename _ElementType>                    using Promise = std::promise<_ElementType>;
    template <typename _ElementType>                    using Future = std::future<_ElementType>;
}

#define Output std::cout
#define Input std::cin
#define Error std::cerr
#define Newl std::endl

#define var auto
#define func []

using namespace cppsh;

)";


// Bash build script:
static String BASH_SCRIPT = R"(#!/bin/bash
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