cmake_minimum_required(VERSION 2.6)

project(happyhttp)

set(HHTTP_SRCS happyhttp.cpp)

ADD_LIBRARY(happyhttp STATIC ${HHTTP_SRCS})
SET_TARGET_PROPERTIES(happyhttp PROPERTIES OUTPUT_NAME "happyhttp")
SET(HHTTP_LIB_TARGET happyhttp)
SET(HHTTP_INSTALL_TARGETS ${HHTTP_INSTALL_TARGETS} ";happyhttp")
