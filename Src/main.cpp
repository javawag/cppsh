#include "main.h"
#include "MD5.h"

int main(int argc, char **argv) {
	if (argc < 2) {
		cerr << "No script defined." << endl;
		cerr << "Usage: " << argv[0] << " <script-name>" << endl;
        return 1;
	}

    FileInputStream script(argv[1]);

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
                code << line << endl;
            }
        }
    }

    // Write build artifacts
    fs::path workingDir = GetFullPathForSubfolder(scriptHash);
    fs::path buildDir = workingDir / "Build";

    String scriptName = fs::path(argv[1]).filename().string();

    UnorderedMap<String, String> cmakePlaceholders = {
            { "script_name", scriptName },
            { "build_dir", buildDir.string() }
    };

    FileOutputStream outCode((workingDir / "Script.cpp").string());
    FileOutputStream outCMake((workingDir / "CMakeLists.txt").string());

    outCode << CPP_DEFAULTS << "\n\n"
            << preprocessor.str() << "\n\n"
            << "int main(int argc, char **argv) {" << "\n"
            << code.str() << "\n\n"
            << "}";


    outCMake    << StringReplacePlaceholders(CMAKE_DEFAULTS, cmakePlaceholders) << "\n\n"
                << cmake.str();

    // Call CMake!
    OutputStringStream command;
    command << "cd \"" << buildDir.string() << "\""
            << "&& cmake .. "
            << "&& make"
            << "&& ./" << scriptName << " " << ConcatenateArgs(argv+1, argv+argc);

    system(command.str().c_str());

    return 0;
}

bool StringBeginsWith(const String &string, const String &pattern) {
    return string.find(pattern) == 0;
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


fs::path GetFullPathForSubfolder(const String &folderName) {
    return fs::path(getenv("HOME")) / ".cppsh" / folderName;
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