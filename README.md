cppsh
=====

cppsh (pronounced "capiche", ka-PEESH) is C++11 as a scripting language, using CMake as a build system.

Currently VERY, VERY alpha!
* Only tested on Mac OS X, but should work on Linux too (Windows support won't work without Cygwin, or possibly at all!)
* Prerequisites to build: CMake, Boost, C++11 compiler
* Prerequisites to run scripts: CMake, C++11 compiler
* May eat children and/or destroy rainforest if run without seatbelts on

While C++ code is accepted as-is, I've added some nice defines and using statements to make it feel...
*scriptier*. You can use these extensions if you wish, or if not you can just use plain C++11. Semicolons are optional, unless you use `//` comments at the end of the line (this be a bug!).

Sample Script
=============

```
#!/usr/bin/env cppsh
/**CMake
    find_package(Boost COMPONENTS filesystem system REQUIRED)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(${SCRIPT} ${Boost_LIBRARIES}) 
*/

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem

fs::path testDir = "Goose"
fs::path testFile = testDir / "test.txt"

fs::create_directory(testDir)

FileOut out(testFile.string();
out << "Hello, scripty world!"

Output << "Made a goosey file!" << Newl
```

The `/**CMake` section is *optional*, but it injects arbitrary CMake script into the CMakeLists.txt file so that you can link in external libraries. Here, I include Boost's filesystem library.

As an aside, there is an alternative syntax for CMake too - `@package`. For very simple CMake includes you can use this instead, for example, our Boost example above goes from

```
/**CMake
    find_package(Boost COMPONENTS filesystem system REQUIRED)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(${SCRIPT} ${Boost_LIBRARIES}) 
*/
```

to

```
@package Boost COMPONENTS filesystem system REQUIRED
````

As you can see, the line is passed into `find_package` as-is, and the first argument (i.e. `Boost`) is understood to be the library name, and is used for the 2nd two lines. Most packages can be used in this way, but some unfortunately cannot. This `@package` syntax can appear either *inside* the `/**CMake` section, or else anywhere in the script.

The `#include` directive, while specified inside the script itself, is *hoisted* to the top of the file, where the includes should go. In this way, you are actually able to `#include` files from anywhere within the script.

`FileOut` is simply an alias to `std::ofstream`. In `DefaultText.h` you can see all the aliases I create - I prefer working with these class names instead of the standard ones, but the standard ones are still available. 

This script, when run, will create a directory called `Goose` and then inside it place a file named `test.txt`, containing the text `"Hello, scripty world!"`.

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
var isEven = func (Int a) -> Bool {
	return a % 2 == 0
}

for (Int i = 0; i < 10; ++i) {
	Output << std::boolalpha << i << " is even? " << isEven(i) << Newl
}
```

How does this work internally?!
===============================

Magic. And dragons. But seriously, it creates a hidden folder inside your home directory, called .cppsh. In here you'll find MD5 hashes of the full paths of cppsh scripts you've run.

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

The CMakeLists.txt and Script.cpp files are generated from the script, and the run.sh is generated from a template. Then, cppsh runs run.sh which calls CMake to build your script in the parent directory. It then runs the resultant executable with any parameters you passed.

Each time you run the script, if a cached version of the binary is available, it should be used rather than compiling from scratch to save time.



Enjoy! And feel free to submit any bugs/issues here. Or fork it for your own purposes.
