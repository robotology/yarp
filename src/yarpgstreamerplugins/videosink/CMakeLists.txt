cmake_minimum_required(VERSION 3.10)

project(gstyarpvideosink LANGUAGES CXX)

# Include the directories for the required packages
include_directories(${GSTREAMER_INCLUDE_DIRS} ${YARP_INCLUDE_DIRS})

# Set the source files
set(SOURCES yarpVideoSink.cpp yarpVideoSink.h)

# Set the output directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Add the library target
add_library(gstyarpvideosink SHARED ${SOURCES})

# Link the required libraries
target_link_libraries(gstyarpvideosink PRIVATE ${GOBJECT_LIBRARIES})
target_include_directories(gstyarpvideosink SYSTEM PRIVATE ${GOBJECT_INCLUDE_DIR})

target_link_libraries(gstyarpvideosink PRIVATE ${GLIB2_LIBRARIES})
target_include_directories(gstyarpvideosink SYSTEM PRIVATE ${GLIB2_INCLUDE_DIR})

target_link_libraries(gstyarpvideosink PRIVATE ${GSTREAMER_LIBRARY})
target_include_directories(gstyarpvideosink SYSTEM PRIVATE ${GSTREAMER_INCLUDE_DIRS})

target_link_libraries(gstyarpvideosink PRIVATE ${GSTREAMER_APP_LIBRARY})
target_include_directories(gstyarpvideosink SYSTEM PRIVATE ${GSTREAMER_app_INCLUDE_DIR})

target_link_libraries(gstyarpvideosink PRIVATE
    YARP::YARP_os
    YARP::YARP_sig
    YARP::YARP_dev
    YARP::YARP_init
    gstvideo-1.0
)

# Set the properties for the shared library
set_target_properties(gstyarpvideosink PROPERTIES
#   VERSION 1.0
#   SOVERSION 1
    LIBRARY_OUTPUT_NAME "gstyarpvideosink"
)

# Install the plugin to the specified directory
install(TARGETS gstyarpvideosink
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/gstreamer-1.0
)
