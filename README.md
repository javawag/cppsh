cppsh
=====

C++11 as a scripting language, using CMake as a build system

Currently VERY, VERY alpha!

* Only tested on Mac OS X, but should work on Linux too (Windows support won't work without Cygwin!)
* Prerequisites to build: CMake, Boost, C++11 compiler
* Prerequisites to run scripts: CMake, C++11 compiler
* May eat children and/or destroy rainforest if run without seatbelts on

Apologies for my absurd coding style, I've come from a mixed background and I like what I like :)

Sample Script
=============

```
#!/usr/bin/env cppsh
/**CMake
	#boost program_options
	find_package(Boost COMPONENTS program_options filesystem system REQUIRED)
	include_directories(${Boost_INCLUDE_DIRS})
	target_link_libraries(${SCRIPT} ${Boost_LIBRARIES})	
*/

cout << "Wahoo!" << endl;

FileOut f("/Users/joe/Desktop/goose.txt");
f << "Magic!" << endl;

return 0;
```

The `/**CMake` section is *optional*, but it injects arbitrary CMake script into the CMakeLists.txt file so that you can link in external libraries. Here, I include Boost's program_options and filesystem libraries, and then proceed to not use them.

`FileOut` is simply an alias to `std::ofstream`. In `DefaultText.h` you can see all the aliases I create - I prefer working with these class names instead of the standard ones, but the standard ones are still available. 

You'll also see in `DefaultText.h` that I controversially do a `using namespace std;` - this is horrible in real code, but for scripts it saves time. But feel free to tweak away at that file if you need to ;)
