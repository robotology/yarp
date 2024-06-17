cmake_minimum_required(VERSION 3.10)

project(gstyarpvideosource LANGUAGES CXX)

# Include the directories for the required packages
include_directories(${GSTREAMER_INCLUDE_DIRS} ${YARP_INCLUDE_DIRS})

# Set the source files
set(SOURCES yarpVideoSource.cpp yarpVideoSource.h)

# Set the output directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Add the library target
add_library(gstyarpvideosource SHARED ${SOURCES})

# Link the required libraries
target_link_libraries(gstyarpvideosource PRIVATE ${GOBJECT_LIBRARIES})
target_include_directories(gstyarpvideosource SYSTEM PRIVATE ${GOBJECT_INCLUDE_DIR})

target_link_libraries(gstyarpvideosource PRIVATE ${GLIB2_LIBRARIES})
target_include_directories(gstyarpvideosource SYSTEM PRIVATE ${GLIB2_INCLUDE_DIR})

target_link_libraries(gstyarpvideosource PRIVATE ${GSTREAMER_LIBRARY})
target_include_directories(gstyarpvideosource SYSTEM PRIVATE ${GSTREAMER_INCLUDE_DIRS})

target_link_libraries(gstyarpvideosource PRIVATE ${GSTREAMER_APP_LIBRARY})
target_include_directories(gstyarpvideosource SYSTEM PRIVATE ${GSTREAMER_app_INCLUDE_DIR})

target_link_libraries(gstyarpvideosource PRIVATE
    YARP::YARP_os
    YARP::YARP_sig
    YARP::YARP_dev
    YARP::YARP_init
    gstvideo-1.0
)

# Set the properties for the shared library
set_target_properties(gstyarpvideosource PROPERTIES
#   VERSION 1.0
#   SOVERSION 1
    LIBRARY_OUTPUT_NAME "gstyarpvideosource"
)

# Install the plugin to the specified directory
install(TARGETS gstyarpvideosource
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/gstreamer-1.0
)
