set(NAME "${CMAKE_PROJECT_NAME}_core")
add_library(${NAME})
target_sources(
  ${NAME}
  PUBLIC handshake/handshake.cpp
         handshake/socket_utils.cpp
         system/abstract_socket.cpp
         system/semaphore_set.cpp
         system/shared_memory.cpp
         messenger/message_io.cpp
         messenger/message_queue.cpp
         util/async_threads.cpp)
target_compile_definitions(${NAME} PUBLIC MSG_LOG_LEVEL "${MSG_LOG_LEVEL}")
