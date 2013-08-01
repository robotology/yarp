#  Yarp Application Builder
#  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
#  Authors: Ali Paikan <ali.paikan@iit.it>
#
#  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#


get_property(YARP_TREE_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)

file(GLOB folder_source src/*.cpp)
file(GLOB folder_header include/*.h)
source_group("Source Files" FILES ${folder_source})
source_group("Header Files" FILES ${folder_header})


if(UNIX)
    if (GtkMM_VERSION_MAJOR GREATER 2 OR GtkMM_VERSION_MAJOR EQUAL 2)
        if (GtkMM_VERSION_MINOR LESS 22)
            add_definitions(-DGTKMM_LESS_2_22)
         endif()
    endif()

    add_definitions(-Wno-trigraphs)

endif(UNIX)

add_definitions(${GTK2_DEFINITIONS})

include_directories(${YARP_TREE_INCLUDE_DIRS})
include_directories(${ACE_INCLUDE_DIRS})
include_directories(${TinyXML_INCLUDE_DIRS})
include_directories(${GTK2_INCLUDE_DIRS})
include_directories(${GooCanvasMM_INCLUDE_DIRS})
include_directories(include)
include_directories(../libymanager/include)

# GTKMM_INCLUDE_DIRS, GTKMM_LIBRARY_DIRS and GTKMM_LIBRARIES

## FIXME 2.4 Move the check in conf/YarpSystemCheck.cmake later
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-Wno-mismatched-tags" CXX_HAS_WNO_MISMATCHED_TAGS)
if(CXX_HAS_WNO_MISMATCHED_TAGS)
   set_source_files_properties(${folder_source}
                               PROPERTIES COMPILE_FLAGS "-Wno-mismatched-tags")
endif(CXX_HAS_WNO_MISMATCHED_TAGS)

add_executable(gyarpbuilder ${folder_source} ${folder_header})

target_link_libraries(gyarpbuilder YARP_init
                                   YARP_OS
                                   YARP_manager
				   ${ACE_LIBRARIES}
                                   ${GTK2_GTK_LIBRARY}
                                   ${GTK2_GDK_LIBRARY}
                                   ${GTK2_GLIB_LIBRARY}
                                   ${GTK2_PANGOCAIRO_LIBRARY}
                                   ${GTK2_GTKMM_LIBRARY}
                                   ${GTK2_GLIBMM_LIBRARY}
                                   ${GTK2_GDKMM_LIBRARY}
                                   ${GTK2_ATKMM_LIBRARY}
                                   ${GTK2_SIGC++_LIBRARY}
                                   ${GTK2_GTHREAD_LIBRARY}
                                   ${GooCanvasMM_LIBRARIES})

install(TARGETS gyarpbuilder COMPONENT utilities DESTINATION bin)
