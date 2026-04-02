add_executable(
	unitTest
	source/test.cpp
	source/textAnalyzerTest.cpp)

target_link_libraries(
	unitTest PUBLIC
	gtestLib
	textAnalyzer
)