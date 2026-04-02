cmake_minimum_required(VERSION 3.0)

set(PROJECT_NAME textAnalyzerLib)

project(${PROJECT_NAME} VERSION 1.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/compilerSetup.txt)

add_subdirectory(dependencies/googletest)
add_subdirectory(dependencies/threadPool)
add_subdirectory(lib/textAnalyzer)
add_subdirectory(lib/main)
add_subdirectory(lib/gui)
add_subdirectory(lib/ai)
add_subdirectory(UnitTest)