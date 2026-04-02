add_executable(
	${PROJECT_NAME} 
	source/main.cpp
	source/program.cpp
	include/program.h
)

target_link_libraries(
	${PROJECT_NAME} PUBLIC
	textAnalyzer
)

target_include_directories(
	${PROJECT_NAME} PUBLIC
	"include/"
)