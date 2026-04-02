find_package(Qt6 REQUIRED COMPONENTS Core Network)

add_library(AIApi STATIC
    source/ChatGPTClient.cpp
    include/ChatGPTClient.h
)

set_target_properties(AIApi PROPERTIES
    AUTOMOC ON
)

target_include_directories(AIApi PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(AIApi PUBLIC
    Qt6::Core
    Qt6::Network
)
