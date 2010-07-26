
# compile if needed - used by tcpros_carrier, mjpeg_carrier

if (ENABLE_yarpcar_mjpeg_carrier OR ENABLE_yarpcar_tcpros_carrier)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  find_package(YARP REQUIRED)
  include_directories(${YARP_INCLUDE_DIRS})
  add_library(wire_rep_utils WireImage.cpp WireImage.h WireBottle.cpp 
              WireBottle.h)
  target_link_libraries(wire_rep_utils ${YARP_LIBRARIES})
  set_property(TARGET wire_rep_utils PROPERTY INCLUDE_DIRS 
    ${CMAKE_CURRENT_SOURCE_DIR})
endif ()
