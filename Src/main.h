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

using std::cin;
using std::cout;
using std::cerr;
using std::endl;

namespace joe {
    using FileIn = std::ifstream;
    using FileOut = std::ofstream;
    using File = std::fstream;
    using String = std::string;
    using OutputStringStream = std::ostringstream;
    using InputStringStream = std::istringstream;
    using StringStream = std::stringstream;

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

using namespace joe;

#include "DefaultText.h"

#include <boost/filesystem.hpp>
    namespace fs = boost::filesystem;

// TEMPLATES
template <typename _Iter>
String ConcatenateArgs(_Iter begin, _Iter end) {
    OutputStringStream ss;

    for (_Iter iter = begin; iter != end; ++iter) {
        if (iter != begin) {
            ss << " ";
        }

        ss << *iter;
    }

    return ss.str();
}

// FUNCTIONS
bool StringBeginsWith(const String &string, const String &pattern);
String StringReplacePlaceholders(const String &string, const UnorderedMap<String, String> &placeholderValues);

fs::path GetFullPathForSubfolder(const String &folderName);
void CreateSubfolderIfNotExist(const String &folderName);
String CalculateMD5Sum(const String &in);