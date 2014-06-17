project(cppsh)

cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11")
set(EXE_NAME ${PROJECT_NAME})

file(GLOB_RECURSE HEADERS Src/*.h Include/*.h)
file(GLOB_RECURSE SOURCES Src/*.cpp)

add_executable(${EXE_NAME} ${SOURCES} ${HEADERS})

#header hackery
include_directories(${CMAKE_SOURCE_DIR})

#boost filesystem
find_package(Boost COMPONENTS filesystem system REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(${EXE_NAME} ${Boost_LIBRARIES})

#install
install(FILES Include/cppsh.h   DESTINATION include/cppsh)
install(TARGETS ${EXE_NAME}     DESTINATION bin)