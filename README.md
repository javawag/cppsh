cppsh
=====

cppsh (pronounced "capiche", ka-PEESH) is C++11 as a scripting language, using CMake as a build system.

Currently VERY, VERY alpha!
* Only tested on Mac OS X, but should work on Linux too (Windows support won't work without Cygwin, or possibly at all!)
* Prerequisites to build: CMake, Boost, C++11 compiler
* Prerequisites to run scripts: CMake, C++11 compiler
* May eat children and/or destroy rainforest if run without seatbelts on

While C++ code is accepted as-is, I've added some nice defines and using statements to make it feel...
*scriptier*. You can use these extensions if you wish, or if not you can just use plain C++11. 


Sample Script (C++11 style)
===========================

```
#!/usr/bin/env cppsh
/**CMake
    find_package(Boost COMPONENTS filesystem system REQUIRED)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(${SCRIPT} ${Boost_LIBRARIES}) 
*/

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

fs::path testDir = "Goose";
fs::path testFile = testDir / "test.txt";

fs::create_directory(testDir);

std::ofstream out(testFile.string());
out << "Hello, scripty world!";

std::cout << "Made a goosey file!" << std::endl;
```

This script, when run, will create a directory called `Goose` and then inside it place a file named `test.txt`, containing the text `"Hello, scripty world!"`.

The `/**CMake` section is *optional*, but it injects arbitrary CMake script into the CMakeLists.txt file so that you can link in external libraries. Here, I include Boost's filesystem library.

The `#include` directive, while specified inside the script itself, is *hoisted* to the top of the file, where the includes should go. In this way, you are actually able to `#include` files from anywhere within the script.


Sample Script (cppsh style)
===========================

Here is that same script, made *scriptier* using the cppsh syntax - capiche??

```
#!/usr/bin/env cppsh

@package Boost COMPONENTS filesystem system REQUIRED
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem

fs::path testDir = Args.empty() ? "Goose" : Args[0] // Args is a Vector<String>
fs::path testFile = testDir / "test.txt"

fs::create_directory(testDir)

FileOut out(testFile.string())
out << "Hello, scripty world!"

Output << "Made a goosey file!" << Newl
```

Here, we make a few changes. Firstly, semi-colons are out! We also swap out the `std::` classes for nicer-named ones. They are, however, equivalent and you may continue using the C++11 ones if you so prefer.

We're using the alternative syntax for CMake here - `@package`. For very simple CMake includes you can use this instead - for example, our `Boost` package is included in this way.

As you can see, the `@package` line is passed into `find_package` as-is, and the first argument (i.e. `Boost`) is understood to be the library name, and is used for the 2nd two lines from the first example. Most packages can be used in this way (i.e. `Boost`, `CURL`, ...), but some unfortunately cannot (i.e. `wxWidgets`). This `@package` syntax can appear either *inside* the `/**CMake` section, or else anywhere in the script, by the way.

I also sneaked in an optional argument which allows you to rename the folder it creates. `Args` is a `Vector<String>` of the arguments passed in, excluding the script name itself. You can, of course, use `argc` and `argv` instead if you prefer.

`FileOut` is simply an alias to `std::ofstream`. In `DefaultText.h` you can see all the aliases I create - I prefer working with these class names instead of the standard ones, but the standard ones are still available. 

Another quick example: functions
================================

Because these scripts are compiled into a single function, calling other functions is a bit tricky. You can't simply write a function in a function! However, as this is C++11, you can use lambdas like this:

```
auto isEven = [](int a) {
	return a % 2 == 0;
};

for (int i = 0; i < 10; ++i) {
	std::cout << std::boolalpha << i << " is even? " << isEven(i) << std::endl;
}
```

Or, using the optional cppsh syntax:

```
auto isEven = func (Int a) -> Bool {
	return a % 2 == 0
}

for (Int i = 0; i < 10; ++i) {
	Output << std::boolalpha << i << " is even? " << isEven(i) << Newl
}
```

Then, you call it as if it were just another function, i.e. `isEven(5)`.

Inline Bash
===========

You can also run bash commands directly from your C++ code (woah!). Using this you can get String output from Unix commands easily, and you are able to pass in actual C++ objects to scripts too!

```
String message = "hello"
String banner = `banner ${{message}}` //C++ expressions can be passed in with ${{ }} syntax

Output << banner
```

Noice!

How does this work internally?!
===============================

Magic. And dragons. But seriously, it creates a hidden folder inside your home directory, called `.cppsh`. In here you'll find MD5 hashes of the full paths of cppsh scripts you've run.

Directory configuration:
```
~/.cppsh
  |_ 615b3257271b98f658a5ec33cc5f20b0   <-- hashed script path
         |_ Build/                      <-- build folder
         |    |_ run.sh                 <-- script which bootstraps and runs the compiled code
         |    |_ (cmake generated files)<-- compiled code from cmake
         |_ CMakeLists.txt              <-- generated cmake build script
         |_ Script.cpp                  <-- generated full C++ source
```

The `CMakeLists.txt` and `Script.cpp` files are generated from the script (adding semi-colons and such where needed!), and the `run.sh` is generated from a template. Then, cppsh runs `run.sh` which calls CMake to build your script in the parent directory. It then runs the resultant executable with any parameters you passed.

Each time you run the script, if a cached version of the binary is available, it should be used rather than compiling from scratch to save time.

Enjoy! And feel free to submit any bugs/issues here. Or fork it for your own purposes.
