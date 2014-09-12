#pragma once

#include "Include/cppsh.h"
#include "DefaultText.h"
#include <boost/filesystem.hpp>
    namespace fs = boost::filesystem;

using namespace std;

// TEMPLATES
template <typename Iterator>
string ConcatenateArgs(Iterator begin, Iterator end) {
    ostringstream ss;

    for (Iterator iter = begin; iter != end; ++iter) {
        if (iter != begin) {
            ss << " ";
        }

        ss << *iter;
    }

    return ss.str();
}

// FUNCTIONS
bool StringBeginsWith(const string &str, const string &pattern);
bool StringEndsWith(const string &str, const string &pattern);
string StringReplacePlaceholders(string str, const unordered_map<string, string> &placeholderValues);
void StringReplaceInlineBash(string &str, const fs::path &cdTo);

int RunCommand(const string &command);

fs::path GetCppshFolder();
fs::path GetFullPathForSubfolder(const string &folderName);

void CreateSubfolderIfNotExist(const string &folderName);
string CalculateMD5Sum(const string &in);
string CalculateMD5Sum(const fs::path &in);