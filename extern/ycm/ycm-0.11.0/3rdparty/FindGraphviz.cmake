# - Try to find Graphviz cgraph library
# Once done this will define
#
#  Graphviz_FOUND - system has Graphviz installed
#  Graphviz_LIBRARIES
#  Graphviz_INCLUDE_DIRS

if(EXISTS "$ENV{Graphviz_ROOT}")
    set(Graphviz_POSSIBLE_INCDIRS
        "$ENV{Graphviz_ROOT}/include"
        "$ENV{Graphviz_ROOT}/include/graphviz")

    set(Graphviz_POSSIBLE_LIBRARY_PATHS
        "$ENV{Graphviz_ROOT}/lib/release/dll"
        "$ENV{Graphviz_ROOT}/lib/release/lib")
endif()

##### check Graphviz_DIR
if(EXISTS "$ENV{Graphviz_DIR}")
    set(Graphviz_POSSIBLE_INCDIRS
        "$ENV{Graphviz_DIR}/include"
        "$ENV{Graphviz_DIR}/include/graphviz")

    set(Graphviz_POSSIBLE_LIBRARY_PATHS
        "$ENV{Graphviz_DIR}/lib/release/dll"
        "$ENV{Graphviz_DIR}/lib/release/lib")
endif()

if(Graphviz_DIR)
    set(Graphviz_POSSIBLE_INCDIRS
        "${Graphviz_DIR}/include"
        "${Graphviz_DIR}/include/graphviz")

    set(Graphviz_POSSIBLE_LIBRARY_PATHS
        "${Graphviz_DIR}/lib/release/dll"
        "${Graphviz_DIR}/lib/release/lib")
endif()

if(Graphviz_CGRAPH_LIBRARY )
   # in cache already
   set(Graphviz_FIND_QUIETLY TRUE)
endif()

# use pkg-config to get the directories and then use these values
# in the FIND_PATH() and FIND_LIBRARY() calls
if(NOT WIN32)
  find_package(PkgConfig)
  pkg_check_modules(Graphviz_GVC_PKG gvc QUIET)
  pkg_check_modules(Graphviz_CGRAPH_PKG cgraph QUIET)
  pkg_check_modules(Graphviz_CDT_PKG cdt QUIET)
endif()

find_library(Graphviz_GVC_LIBRARY
             NAMES gvc
                   libgvc
             PATHS ${Graphviz_POSSIBLE_LIBRARY_PATHS}
                   /usr/lib
                   /usr/local/lib
             HINTS ${Graphviz_GVC_PKG_LIBRARY_DIRS})
mark_as_advanced(Graphviz_GVC_LIBRARY)

find_library(Graphviz_CGRAPH_LIBRARY
             NAMES cgraph
                   libcgraph
             PATHS ${Graphviz_POSSIBLE_LIBRARY_PATHS}
                   /usr/lib
                   /usr/local/lib
             HINTS ${Graphviz_CGRAPH_PKG_LIBRARY_DIRS})
mark_as_advanced(Graphviz_CGRAPH_LIBRARY)

find_library(Graphviz_CDT_LIBRARY
             NAMES cdt
                   libcdt
             PATHS ${Graphviz_POSSIBLE_LIBRARY_PATHS}
                   /usr/lib
                   /usr/local/lib
             HINTS ${Graphviz_CDT_PKG_LIBRARY_DIRS})
mark_as_advanced(Graphviz_CDT_LIBRARY)

set(Graphviz_LIBRARIES ${Graphviz_GVC_LIBRARY}
                       ${Graphviz_CGRAPH_LIBRARY}
                       ${Graphviz_CDT_LIBRARY})

find_path(Graphviz_INCLUDE_DIR
          NAMES cgraph.h
          PATHS ${Graphviz_POSSIBLE_INCDIRS}
                /usr/include
                /usr/include/graphviz
                /usr/local/include
                /usr/local/include/graphviz
          HINTS ${Graphviz_PKG_INCLUDE_DIR})
mark_as_advanced(Graphviz_INCLUDE_DIR)

set(Graphviz_INCLUDE_DIRS ${Graphviz_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Graphviz
                                  FOUND_VAR Graphviz_FOUND
                                  REQUIRED_VARS Graphviz_LIBRARIES
                                                Graphviz_INCLUDE_DIRS)
