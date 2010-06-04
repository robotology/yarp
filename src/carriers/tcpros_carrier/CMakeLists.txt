if (COMPILE_PLUGIN_LIBRARY)
  prepare_carrier(tcpros_carrier TYPE TcpRosCarrier INCLUDE TcpRosCarrier.h)
endif (COMPILE_PLUGIN_LIBRARY)

if (NOT SKIP_tcpros_carrier)
  find_package(YARP REQUIRED)
  include_directories(${YARP_INCLUDE_DIRS})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  add_library(rostcp_carrier 
    TcpRosCarrier.h TcpRosCarrier.cpp
    TcpRosStream.h TcpRosStream.cpp
    RosHeader.h RosHeader.cpp
    BlobHeader.h)
  target_link_libraries(rostcp_carrier ${YARP_LIBRARIES})

  add_executable(yarpros yarpros.cpp)
  target_link_libraries(yarpros YARP_init ${YARP_LIBRARIES})
endif (NOT SKIP_tcpros_carrier)
