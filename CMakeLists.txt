cmake_minimum_required(VERSION 4.0)

project(semaphore-messenger LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)

include_directories(".")

add_subdirectory(cli)
add_subdirectory(lib)

add_subdirectory(tests)
