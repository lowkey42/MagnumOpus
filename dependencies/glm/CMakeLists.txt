cmake_minimum_required(VERSION 2.6)

project(glm)

file(GLOB_RECURSE GLM_SRCS
         RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
		 ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
		 ${CMAKE_CURRENT_SOURCE_DIR}/*.hxx)

add_custom_target(glm SOURCES ${GLM_SRCS})

