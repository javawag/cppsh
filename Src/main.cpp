#include "main.h"
#include "MD5.h"
#include <unistd.h>

int main(int argc, char **argv) {
	if (argc < 2) {
		cerr << "No script defined." << endl;
		cerr << "Usage: " << argv[0] << " <script-name>" << endl;
        cerr << "   or: " << argv[0] << " --clear-cache # to remove cached executables" << endl;

        return 1;
	}

    // First check the user doesn't mean to clear their cache!
    if (String(argv[1]) == "--clear-cache") {
        uintmax_t deleted = fs::remove_all(GetCppshFolder());

        if (deleted) {
            cout << "Cached cleared. Removed " << deleted << " item" << (deleted == 1 ? "" : "s") << "." << endl;
        } else {
            cerr << "Nothing to clear." << endl;
        }
        return 0;
    }

    FileIn script(argv[1]);

    if (!script.good()) {
        cerr << "Error reading script " << argv[1] << "! Bailing..." << endl;
        return 1;
    }

    // Set up environment
    String scriptHash = CalculateMD5Sum(fs::system_complete(argv[1]).string());
    CreateSubfolderIfNotExist(scriptHash);

    // Read in file
    bool insideCmakePart = false;
    OutputStringStream cmake;
    OutputStringStream preprocessor;
    OutputStringStream code;

    for (String line; std::getline(script, line); ) {
        if (StringBeginsWith(line, "#!")) {
            // Don't let this into the C++ code!!
        } else if (StringBeginsWith(line, "/**CMake")) {
            insideCmakePart = true;
        } else if (StringBeginsWith(line, "*/")) {
            insideCmakePart = false;
        } else if (StringBeginsWith(line, "#")) {
            preprocessor << line << endl;
        } else {
            if (insideCmakePart) {
                cmake << line << endl;
            } else {
                code << line;

                if (!StringEndsWith(line, ";") && !StringEndsWith(line, "{")) {
                    code << ";";
                }

                code << endl;
            }
        }
    }

    // Write build artifacts
    fs::path workingDir = GetFullPathForSubfolder(scriptHash);
    fs::path buildDir = workingDir / "Build";

    String scriptName = fs::path(argv[1]).filename().string();

    UnorderedMap<String, String> placeholders = {
        { "script_name",    scriptName },
        { "build_dir",      buildDir.string() },
        { "args",           ConcatenateArgs(argv+2, argv+argc) }
    };

    {
        FileOut outCode((workingDir / "Script.cpp").string());
        FileOut outCMake((workingDir / "CMakeLists.txt").string());
        FileOut outBash((buildDir / "run.sh").string());

        outCode << CPP_DEFAULTS << "\n\n"
                << preprocessor.str() << "\n\n"
                << "int main(int argc, char **argv) {" << "\n"
                    << code.str() << "\n\n"
                << "}";


        outCMake << StringReplacePlaceholders(CMAKE_DEFAULTS, placeholders) << "\n\n"
                << cmake.str();

        outBash << StringReplacePlaceholders(BASH_SCRIPT, placeholders);
    }

    // Call CMake!
    fs::path bashFile = (buildDir / "run.sh");
    fs::permissions(bashFile, fs::add_perms | fs::others_exe);

    OutputStringStream command;
    command << "/usr/bin/env bash \"" << bashFile.string() << "\"";
    system(command.str().c_str());

    return 0;
}

bool StringBeginsWith(const String &string, const String &pattern) {
    return std::equal(  string.begin(),
                        string.begin() + pattern.length(),
                        pattern.begin()
    );
}
bool StringEndsWith(const String &string, const String &pattern) {
    return std::equal(  string.begin() + string.length() - pattern.length(),
                        string.end(),
                        pattern.begin()
    );
}

String StringReplacePlaceholders(const String &string, const UnorderedMap<String, String> &placeholderValues) {
    String sCopy = string;

    for (auto &pair : placeholderValues) {
        size_t pos = 0;

        String token = "${" + pair.first + "}";

        while ((pos = sCopy.find(token, pos)) != String::npos) {
            sCopy.replace(pos, token.length(), pair.second);
            pos += pair.second.length();
        }
    }

    return sCopy;
}

fs::path GetCppshFolder() {
    return fs::path(getenv("HOME")) / ".cppsh";
}


fs::path GetFullPathForSubfolder(const String &folderName) {
    return GetCppshFolder() / folderName;
}

void CreateSubfolderIfNotExist(const String &folderName) {
    fs::path sub = GetFullPathForSubfolder(folderName) / "Build";

    if (!fs::exists(sub)) {
        fs::create_directories(sub);
    }
}

String CalculateMD5Sum(const String &in) {
    MD5 md5;
    return md5.digestString(const_cast<char *>(in.c_str()));
}