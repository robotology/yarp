if (COMPILE_PLUGIN_LIBRARY)
  prepare_carrier(mjpeg_carrier TYPE MjpegCarrier INCLUDE MjpegCarrier.h)
endif (COMPILE_PLUGIN_LIBRARY)

if (NOT SKIP_mjpeg_carrier)
  find_package(YARP REQUIRED)
  find_package(JPEG REQUIRED)
  include_directories(${JPEG_INCLUDE_DIR})
  include_directories(${YARP_INCLUDE_DIRS})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  add_library(mjpeg_carrier MjpegCarrier.h MjpegCarrier.cpp
			    MjpegStream.h MjpegStream.cpp ${JPEG_SOURCES})
  target_link_libraries(mjpeg_carrier ${YARP_LIBRARIES} ${JPEG_LIBRARY})
endif (NOT SKIP_mjpeg_carrier)
