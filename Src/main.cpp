#include "main.h"
#include "MD5.h"

using std::cerr;
using std::cin;
using std::cout;
using std::endl;

int main(int argc, char **argv) {
    // Check arguments
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
            cout << "Cache cleared. Removed " << deleted << " item" << (deleted == 1 ? "" : "s") << "." << endl;
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
    String scriptName = fs::path(argv[1]).filename().string();
    String scriptHash = CalculateMD5Sum(fs::system_complete(argv[1]).string());

    fs::path currentDir = fs::path(argv[1]).parent_path();
    fs::path workingDir = GetFullPathForSubfolder(scriptHash);
    fs::path buildDir = workingDir / "Build";

    CreateSubfolderIfNotExist(scriptHash);

    // Firstly, check to make sure we've definitely changed the script
    bool isCached = false;
    FileIn inHash((GetFullPathForSubfolder(scriptHash) / "LastHash.txt").string());
    if (inHash.good()) {
        String hash;
        inHash >> hash;

        if (hash == CalculateMD5Sum(fs::path(argv[1]))) {
            isCached = true;
        }
    }

    if (!isCached) {
        // Read in file
        bool insideCmakePart = false;
        OutputStringStream cmake;
        OutputStringStream preprocessor;
        OutputStringStream code;

        for (String line; std::getline(script, line);) {
            if (StringBeginsWith(line, "#!")) {
                // Don't let this into the C++ code!!
            } else if (StringBeginsWith(line, "/**CMake")) {
                insideCmakePart = true;
            } else if (StringBeginsWith(line, "*/") && insideCmakePart) {
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
                        args = packageNameAndArgs.substr(secondSpace + 1);
                    } else {
                        packageName = packageNameAndArgs;
                    }

                    String autoCmake = StringReplacePlaceholders(AUTO_PACKAGE_TEMPLATE, {
                            {"pkg_name", packageName},
                            {"args", args}
                    });
                    cmake << autoCmake << endl;
                } else if (insideCmakePart) {
                    cmake << line << endl;
                } else {
                    StringReplaceInlineBash(line);

                    code << '\t';

                    if (line.empty() || StringEndsWith(line, ";") || StringEndsWith(line, "{") || StringEndsWith(line, ",")) {
                        code << line;
                    } else {
                        size_t commentSlashSlash = line.rfind("//");
                        size_t commentSlashStar = line.rfind("/*");

                        size_t endOfLine = std::min(commentSlashSlash, std::min(commentSlashStar, line.length()));

                        code << line.substr(0, endOfLine) << ";" << line.substr(endOfLine);
                    }

                    code << endl;
                }
            }
        }

        // Write build artifacts
        UnorderedMap<String, String> placeholders = {
            {"script_name", scriptName},
            {"build_dir", buildDir.string()},
            {"current_dir", currentDir.string() }
        };

        // Write out artifacts
        FileOut outCode((workingDir / "Script.cpp").string());
        FileOut outCMake((workingDir / "CMakeLists.txt").string());
        FileOut outBash((buildDir / "build.sh").string());
        FileOut outHash((workingDir / "LastHash.txt").string());

        outCode << "#include <cppsh/cppsh.h>" << "\n"
                << preprocessor.str() << "\n"
                << "int main(int argc, char **argv) {" << "\n"
                    << "Vector<String> Args;" << "\n"
                    << "if (argc > 1) Args = Vector<String>(argv + 1, argv + argc);" << "\n"
                    << code.str() << "\n"
                << "}" << std::flush;

        outCMake    << StringReplacePlaceholders(CMAKE_DEFAULTS, placeholders) << "\n\n"
                    << cmake.str()
                    << std::flush;

        outBash << StringReplacePlaceholders(BASH_SCRIPT, placeholders) << std::flush;
        outHash << CalculateMD5Sum(fs::path(argv[1])) << std::flush;

        // Call CMake!
        fs::path bashFile = (buildDir / "build.sh");
        fs::permissions(bashFile, fs::add_perms | fs::others_exe | fs::group_exe | fs::owner_exe);

        int buildReturn = RunCommand("/usr/bin/env bash " + bashFile.string());
        if (buildReturn != 0) {
            cerr << "CMake / Make failure" << endl;
            return 1;
        }
    }

    // Finally, run it!
    OutputStringStream command;
    command << (buildDir / scriptName).string() << " " << ConcatenateArgs(argv + 2, argv + argc);
    return RunCommand(command.str());
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
                        string.begin() + endPos,
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

void StringReplaceInlineBash(String &str) {
    // This is a bit (/lot) of a mess, but it does a difficult job!
    size_t position = 0;

    bool finished = false;

    // Find each `command`
    while (position < str.length()) {
        size_t beginBacktick = str.find("`", position);

        // No commands!
        if (beginBacktick == String::npos) {
            break;
        }

        // Find end of this command
        size_t endBacktick = str.find("`", beginBacktick + 1);

        // Command not closed! Damnit!
        if (endBacktick == String::npos) {
            break;
        }

        // Write start of call
        OutputStringStream command;
        command << "RunExternalCommand(\"";

        // Get the full command text, including ${{var}} placeholders
        String rawCommand = str.substr(beginBacktick + 1, endBacktick - beginBacktick - 1); //e.g. "ls -alh"

        //Find each ${{var}} in the command (if any!)
        size_t commandPosition = 0;
        while (commandPosition < rawCommand.length()) {
            size_t beginVar = rawCommand.find("${{", commandPosition);

            // No ${{vars}} at all!
            if (beginVar == String::npos) {
                // Write command as-is, all done
                command << rawCommand.substr(commandPosition, rawCommand.length() - commandPosition);
                break;
            } else {
                // Write up to the var
                command << rawCommand.substr(commandPosition, beginVar - commandPosition);
            }

            // Find end of var
            size_t endVar = rawCommand.find("}}", beginVar + 1);

            // Let's hope we don't have unclosed vars, but just in case
            if (endVar == String::npos) {
                break;
            }

            // Get the text to appear outside quotes (i.e. the var)
            String outsideQuotes = rawCommand.substr(beginVar + 3, endVar - beginVar - 3);

            // Insert it in
            commandPosition = endVar + 2;
            command << "\" + " << outsideQuotes;

            // If we're on a var and we've reached the end of the command, don't start a new string literal
            if (commandPosition == rawCommand.length()) {
                command << ")";
                finished = true;
            } else {
                // Start a new string literal so we're back at initial state
                command << " + \"";
            }
        }

        // Unless we finished on a var just now, close out the string literal
        if (!finished) {
            command << "\")";
        }

        // Jam this finished command in :D
        str.replace(beginBacktick, endBacktick + 1 - beginBacktick, command.str());

        // Move cursor!
        position = beginBacktick + command.str().length() + 1;
    }
}

int RunCommand(const String &command) {
    int returnCode = system(command.c_str());
    return WEXITSTATUS(returnCode);
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

String CalculateMD5Sum(const fs::path &in) {
    MD5 md5;
    return md5.digestFile(const_cast<char *>(in.string().c_str()));
}
