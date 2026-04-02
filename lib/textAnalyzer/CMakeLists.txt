add_library(
	textAnalyzer
	include/IStatisticModule.h
	include/Modules.h
	include/TextMetrics.h
	include/MetricCollector.h
	include/TextUtils.h
	include/TextAnalyzer.h
	include/TextManager.h
	source/TextAnalyzer.cpp
	source/TextManager.cpp)

set_target_properties(textAnalyzer PROPERTIES LINKER_LANGUAGE CXX)

set_target_properties(
	textAnalyzer PROPERTIES 
    VERSION 1
    SOVERSION 0
)

target_include_directories(
	textAnalyzer PUBLIC
	"include/"
	"${CMAKE_SOURCE_DIR}/dependencies/nlohmann"
	"${CMAKE_SOURCE_DIR}/dependencies/threadPool/include"
)

target_link_libraries(textAnalyzer PUBLIC ThreadPool)