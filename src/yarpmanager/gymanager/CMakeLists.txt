#  Yarp Modules Manager
#  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
#  Authors: Ali Paikan <ali.paikan@iit.it>
# 
#  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#
 

get_property(YARP_TREE_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)

#if(APPLE)
#    message(STATUS "gyarpmanager is currently available only for Windows and Linux distributions.")
#else(APPLE)

if(YARP_USE_GTK2)
    include_directories(${GTK2_INCLUDE_DIRS})
else(YARP_USE_GTK2)
    add_definitions(${GtkMM_C_FLAGS})
    include_directories(${GtkMM_INCLUDE_DIRS})
endif(YARP_USE_GTK2)

include_directories(${YARP_TREE_INCLUDE_DIRS})
include_directories(${ACE_INCLUDE_DIRS})
include_directories(${TinyXML_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libymanager/include)

file(GLOB folder_source *.cpp)
file(GLOB folder_header *.h)
source_group("Source Files" FILES ${folder_source})
source_group("Header Files" FILES ${folder_header})

#pkg_check_modules(GOOCANVASMM goocanvasmm-1.0)
# GTKMM_INCLUDE_DIRS, GTKMM_LIBRARY_DIRS and GTKMM_LIBRARIES 

if(MSVC)
    add_definitions(/wd4099)
endif(MSVC)

## FIXME 2.4 Move the check in conf/YarpSystemCheck.cmake later
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-Wno-mismatched-tags" CXX_HAS_WNO_MISMATCHED_TAGS)
if(CXX_HAS_WNO_MISMATCHED_TAGS)
    set_source_files_properties(${folder_source}
                                PROPERTIES COMPILE_FLAGS "-Wno-mismatched-tags")
endif(CXX_HAS_WNO_MISMATCHED_TAGS)

if(UNIX)
    add_definitions(-Wno-trigraphs)
endif(UNIX)

add_executable(gyarpmanager ${folder_source} ${folder_header})
target_link_libraries(gyarpmanager YARP_init YARP_OS YARP_manager ${ACE_LIBRARIES})

if(YARP_USE_GTK2)
    target_link_libraries(gyarpmanager ${GTK2_GTK_LIBRARY}
                                       ${GTK2_GDK_LIBRARY}
                                       ${GTK2_GLIB_LIBRARY}
                                       ${GTK2_GTKMM_LIBRARY}
                                       ${GTK2_GLIBMM_LIBRARY}
                                       ${GTK2_GDKMM_LIBRARY}
                                       ${GTK2_ATKMM_LIBRARY}
                                       ${GTK2_SIGC++_LIBRARY})
    target_link_libraries(gyarpmanager ${Gthread_LIBRARIES})
else(YARP_USE_GTK2)
    target_link_libraries(gyarpmanager ${GtkMM_LIBRARIES})
endif(YARP_USE_GTK2)

	
install(TARGETS gyarpmanager COMPONENT utilities DESTINATION bin)

#endif(APPLE)
