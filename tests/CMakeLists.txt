function(messenger_add_test name)
  cmake_parse_arguments(arg "" "" "SRCS" ${ARGN})

  set(test_name "test_${name}")
  add_executable("${test_name}")
  target_sources("${test_name}" PRIVATE "${arg_SRCS}")
  target_link_libraries("${test_name}" PRIVATE "${CMAKE_PROJECT_NAME}_core")
  add_test(NAME "${name}" COMMAND "${test_name}")
endfunction()

messenger_add_test(handshake SRCS test_handshake.cpp)
messenger_add_test(messenger SRCS test_messenger.cpp)
