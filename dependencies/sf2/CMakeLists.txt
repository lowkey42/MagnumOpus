cmake_minimum_required(VERSION 2.6)

project(sf2)

file(GLOB_RECURSE SF2_SRCS
         RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
		 ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
		 ${CMAKE_CURRENT_SOURCE_DIR}/*.hxx)

add_custom_target(sf2 SOURCES ${SF2_SRCS})

