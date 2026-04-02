cmake_minimum_required(VERSION 3.0)

project(gtest VERSION 1.0)

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED true )

add_library(
	gtestLib
	src/gtest.cc
	src/gtest-assertion-result.cc
	src/gtest-death-test.cc
	src/gtest-filepath.cc	
	src/gtest-matchers.cc
	src/gtest-port.cc
	src/gtest-printers.cc
	src/gtest-test-part.cc
	src/gtest-typed-test.cc
)

target_include_directories(
	gtestLib PUBLIC
	"include/"
	"./"
)