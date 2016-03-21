#  Sharedlib++
#  Copyright: (C) 2015 iCub Facility, Istituto Italiano di Tecnologia
#  Authors: Ali Paikan <ali.paikan@gmail.com>
#
#  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#

cmake_minimum_required(VERSION 2.8.9)

project(RTF_yarp_utility)

find_package(YARP REQUIRED)

get_property(RTF_TREE_INCLUDE_DIRS GLOBAL PROPERTY RTF_TREE_INCLUDE_DIRS)

get_property(RTF_LIBS GLOBAL PROPERTY RTF_LIBS)
get_property(RTF_INTERNAL_INCLUDE_DIRS GLOBAL PROPERTY RTF_INTERNAL_INCLUDE_DIRS)
get_property(RTF_INTERNAL_LIBS GLOBAL PROPERTY RTF_INTERNAL_LIBS)

# Create the library
file(GLOB folder_source *.cpp)
file(GLOB_RECURSE folder_header ../include/rtf/yarp/*.h)
source_group("Source Files" FILES ${folder_source})
source_group("Header Files" FILES ${folder_header})

include_directories(${RTF_TREE_INCLUDE_DIRS}
                    ${RTF_INTERNAL_INCLUDE_DIRS}
                    ../include/rtf/yarp
                    ${YARP_INCLUDE_DIRS})

if(WIN32)
    add_library(RTF_yarp_utility STATIC ${folder_source} ${folder_header} )
else()
    add_library(RTF_yarp_utility SHARED ${folder_source} ${folder_header} )
endif()

target_link_libraries(RTF_yarp_utility ${RTF_LIBS} ${YARP_LIBRARIES})


# We already added the header files within the parent CMakeLists
# choose which header files should be installed
#set(RTF_YARP_HDRS ../include/rtf/yarp/YarpTestAsserter.h)
#set_property(TARGET RTF_yarp_utility PROPERTY PUBLIC_HEADER ${RTF_YARP_HDRS})

install(TARGETS RTF_yarp_utility
        EXPORT RTF
        COMPONENT runtime
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include/rtf/yarp)

set_property(GLOBAL APPEND PROPERTY RTF_LIBS RTF_yarp_utility)
set_property(TARGET RTF_yarp_utility PROPERTY INCLUDE_DIRS ${RTF_TREE_INCLUDE_DIRS})

