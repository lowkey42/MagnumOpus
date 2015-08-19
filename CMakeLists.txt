cmake_minimum_required(VERSION 2.6)

if(WIN32 OR EMSCRIPTEN)
	SET(PHYSFS_INTERNAL_ZLIB TRUE)
	SET(PHYSFS_ARCHIVE_7Z FALSE)
endif(WIN32 OR EMSCRIPTEN)

set(ROOT_DIR ${CMAKE_SOURCE_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${ROOT_DIR}/modules")
include_directories(AFTER ${ROOT_DIR}/dependencies)
add_definitions(-DGLM_FORCE_RADIANS)

add_subdirectory(dependencies/physfs)
add_subdirectory(dependencies/soil)
add_subdirectory(dependencies/glm)
add_subdirectory(dependencies/sf2)
add_subdirectory(dependencies/happyhttp)
add_subdirectory(src)


file(GLOB_RECURSE ALL_FILES
		 RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
		 ${CMAKE_CURRENT_SOURCE_DIR}/*.*)

add_custom_target(all_project_files SOURCES ${ALL_FILES})
