project(cppsh)

cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} --std=c++11)
set(EXE_NAME ${PROJECT_NAME})

file(GLOB_RECURSE HEADERS Src/*.h Include/*.h)
file(GLOB_RECURSE SOURCES Src/*.cpp)

add_executable(${EXE_NAME} ${SOURCES} ${HEADERS})

# cppsh header
find_path(CPPSH_HEADER
            NAMES           cppsh/cppsh.h
            HINTS           /usr/local/include
                            /opt/local/include)
include_directories(${CPPSH_HEADER})

#boost filesystem
find_package(Boost COMPONENTS filesystem system REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(${EXE_NAME} ${Boost_LIBRARIES})

#install
install(FILES Include/cppsh.h   DESTINATION include/cppsh)
install(TARGETS ${EXE_NAME}     DESTINATION bin)