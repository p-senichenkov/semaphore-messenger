cmake_minimum_required(VERSION 4.0)

project(semaphore-messenger LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)

set(CACHE{MSG_LOG_LEVEL} TYPE STRING HELP "Possible values: Debug, Info" VALUE
                         "Info")

include_directories(".")

add_subdirectory(cli)
add_subdirectory(lib)

enable_testing()
add_subdirectory(tests)
