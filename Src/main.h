#pragma once

#include "Include/cppsh.h"
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
bool StringEndsWith(const String &string, const String &pattern);
String StringReplacePlaceholders(const String &string, const UnorderedMap<String, String> &placeholderValues);
String StringReplace(const String &string, const String &find, const String &replace);
void StringReplaceInlineBash(String &str);

int RunCommand(const String &command);

fs::path GetCppshFolder();
fs::path GetFullPathForSubfolder(const String &folderName);

void CreateSubfolderIfNotExist(const String &folderName);
String CalculateMD5Sum(const String &in);
String CalculateMD5Sum(const fs::path &in);