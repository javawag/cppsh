#include "main.h"
#include "MD5.h"

using std::cerr;
using std::cin;
using std::cout;
using std::endl;

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
            if (StringBeginsWith(line, "@package")) {
                size_t firstSpace = line.find_first_of(" ");
                String packageNameAndArgs = line.substr(firstSpace + 1);

                size_t secondSpace = packageNameAndArgs.find_first_of(" ");

                String packageName, args;
                if (firstSpace != String::npos) {
                    packageName = packageNameAndArgs.substr(0, secondSpace);
                    args = packageNameAndArgs.substr(secondSpace+1);
                } else {
                    packageName = packageNameAndArgs;
                }

                String autoCmake = StringReplacePlaceholders(AUTO_PACKAGE_TEMPLATE, {
                        { "pkg_name", packageName },
                        { "args", args}
                });
                cmake << autoCmake << endl;
            } else if (insideCmakePart) {
                cmake << line << endl;
            } else {
                if (!StringEndsWith(line, ";") && !StringEndsWith(line, "{")) {

                    size_t commentSlashSlash = line.rfind("//");
                    size_t commentSlashStar = line.rfind("/*");

                    size_t endOfLine = std::min(commentSlashSlash, std::min(commentSlashStar, line.length()));

                    code << line.substr(0, endOfLine) << ";" << line.substr(endOfLine);
                } else {
                    code << line;
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

        outCode << "#include <cppsh/cppsh.h>" << "\n\n"
                << preprocessor.str() << "\n\n"
                << "int main(int argc, char **argv) {" << "\n"
                    << "Vector<String> Args;" << "\n"
                    << "if (argc > 1) Args = Vector<String>(argv + 1, argv + argc);" << "\n"
                    << code.str() << "\n\n"
                << "}";


        outCMake << StringReplacePlaceholders(CMAKE_DEFAULTS, placeholders) << "\n\n"
                << cmake.str();

        outBash << StringReplacePlaceholders(BASH_SCRIPT, placeholders);
    }

    // Call CMake!
    fs::path bashFile = (buildDir / "run.sh");
    fs::permissions(bashFile, fs::add_perms | fs::others_exe | fs::group_exe | fs::owner_exe);

    int exitCode = system(bashFile.string().c_str());

    return WEXITSTATUS(exitCode);
}

bool StringBeginsWith(const String &string, const String &pattern) {
    size_t startPos = string.find_first_not_of("\t ");

    if (string.length() - startPos < pattern.length()) {
        return false;
    }

    return std::equal(  string.begin() + startPos,
                        string.begin() + startPos + pattern.length(),
                        pattern.begin()
    );
}
bool StringEndsWith(const String &string, const String &pattern) {
    size_t endPos = string.find_last_not_of("\t ") + 1;

    if (endPos < pattern.length()) {
        return false;
    }

    return std::equal(  string.begin() + endPos - pattern.length(),
                        string.end() - endPos,
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