cmake_minimum_required(VERSION 2.6)

# include core
add_subdirectory(core)

project(MagnumOpus)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -Wextra -Wall -pedantic -Werror -Wno-unused-parameter")

include_directories(${ROOT_DIR}/src) # for core


if(EMSCRIPTEN)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_SDL=2 -Oz -s DEMANGLE_SUPPORT=1")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -s USE_SDL=2 -s DEMANGLE_SUPPORT=1")
	set(ZLIB_LIBRARY "")
	set(GLEW_LIBRARIES "")
	set(OPENGL_LIBRARIES "")
	set(SDL2_LIBRARY "")
	
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O1")
	
	add_definitions(-DSTACKTRACE)
	find_package(GLEW REQUIRED)

	find_package(OpenGL REQUIRED)
	include_directories(${OpenGL_INCLUDE_DIRS})
	link_directories(${OpenGL_LIBRARY_DIRS})
	add_definitions(${OpenGL_DEFINITIONS})
	find_package(SDL2 REQUIRED)
	include_directories(${SDL2_INCLUDE_DIR})
	
	
	if(WIN32)
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lmingw32 -limagehlp -lsdl2main -lglew32") # why???
		add_definitions(-DWIN)

	else()
		FIND_PACKAGE(ZLIB)
		INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIR})
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-pie")
	endif()
endif(EMSCRIPTEN)




if(EMSCRIPTEN)
	file(COPY ${ROOT_DIR}/assets/archives.lst DESTINATION "./")
	file(COPY ${ROOT_DIR}/assets/assets DESTINATION "./")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file archives.lst --preload-file assets -o MagnumOpus.html")
else()
#	file(COPY ${ROOT_DIR}/assets/archives.lst DESTINATION ${BIN_DIR})
#	file(COPY ${ROOT_DIR}/assets/assets DESTINATION ${BIN_DIR})
endif(EMSCRIPTEN)


file(GLOB_RECURSE MAGNUM_SRCS
         RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
		 game/*.cpp
		 game/*.h*)

add_executable(MagnumOpus main.cpp ${MAGNUM_SRCS})
target_link_libraries(MagnumOpus ${SDL2_LIBRARY} core)


option(BUILD_TESTS "Build tests" OFF)

if(BUILD_TESTS AND NOT EMSCRIPTEN)
# Tests
add_executable(TestLevelGen test_level_gen.cpp ${MAGNUM_SRCS})
target_link_libraries(TestLevelGen ${SDL2_LIBRARY} core)
endif()

