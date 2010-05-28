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
    RosHeader.h RosHeader.cpp)
endif (NOT SKIP_tcpros_carrier)
