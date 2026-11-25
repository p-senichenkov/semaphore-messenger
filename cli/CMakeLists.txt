set(NAME "${CMAKE_PROJECT_NAME}_cli")
add_executable(${NAME})
target_sources(${NAME} PRIVATE main.cpp cli.cpp)
target_link_libraries(${NAME} "${CMAKE_PROJECT_NAME}_core")
