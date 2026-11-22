add_library(
  "${CMAKE_PROJECT_NAME}_core"
  handshake/socket_utils.cpp system/semaphore_set.cpp system/shared_memory.cpp
  messenger/message_queue_processor.cpp)
