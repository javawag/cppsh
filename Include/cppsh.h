#pragma once

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
    using OutputStream = std::ostream;
    using InputStream = std::istream;
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

    inline String RunExternalCommand(const String &command) {
        FILE *in = popen(command.c_str(), "r");
        char buff[512];
        OutputStringStream output;

        if (!in) {
            return "";
        }

        while(fgets(buff, sizeof(buff), in) != NULL){
            output << buff;
        }

        pclose(in);

        return output.str();
    }
}

#define Output std::cout
#define Input std::cin
#define Error std::cerr
#define Newl std::endl

#define func []

using namespace cppsh;
