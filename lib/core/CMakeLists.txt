set(NAME "${CMAKE_PROJECT_NAME}_core")
add_library(NAME)
target_sources(
  NAME
  PRIVATE handshake/socket_utils.cpp system/semaphore_set.cpp
          system/shared_memory.cpp messenger/message_io
          messenger/message_queue.cpp)
