cmake_minimum_required(VERSION 3.16)

project(GuiApp)

find_package(Qt6 REQUIRED COMPONENTS Widgets Charts Network)

qt6_add_resources(GUI_RESOURCES_COMPILED resources/resources.qrc)

set(GUI_SOURCES
    source/main.cpp
    source/MainWindow.cpp
    source/AnalysisResultDialog.cpp
    source/PerformanceChartDialog.cpp
)


set(GUI_HEADERS
    include/MainWindow.h
    include/AnalysisResultDialog.h
    include/PerformanceChartDialog.h
)

set(GUI_UI
    source/MainWindow.ui
    source/AnalysisResultDialog.ui
    source/PerformanceChartDialog.ui
)

add_executable(GuiApp
    ${GUI_SOURCES}
    ${GUI_HEADERS}
    ${GUI_UI}
    ${GUI_RESOURCES_COMPILED}
)

set_target_properties(GuiApp PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)

target_sources(GuiApp PRIVATE ${GUI_RESOURCES_COMPILED})

target_include_directories(GuiApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/../textAnalyzer/include
    ${PROJECT_SOURCE_DIR}/../ai/include
)

target_link_libraries(GuiApp PRIVATE
    Qt6::Widgets
    Qt6::Charts
    Qt6::Network
    textAnalyzer
    AIApi 
)
