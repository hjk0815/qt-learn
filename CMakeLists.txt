


cmake_minimum_required(VERSION 3.10)

project(jk)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)


set(SOURCE main.cpp
    src/utils/event.cpp
    src/utils/Timer.cpp
)

add_executable(jk ${SOURCE})

set(PROJECT_PUBLIC_INCLUDES
    ${CMAKE_SOURCE_DIR}/include
)

target_include_directories(jk
    PRIVATE
    ${PROJECT_PUBLIC_INCLUDES}
)
