
add_library(ThreadPool STATIC
    source/ThreadPool.cpp
    source/MultiFuture.cpp
    source/TaskPool.cpp
    source/MessagePool.cpp
)

target_include_directories(ThreadPool PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
