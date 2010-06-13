# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

option(CREATE_LIB_MATH "Create math library libYARP_math?" OFF)

set(YARP_HAS_MATH_LIB FALSE)
if (CREATE_LIB_MATH)
  find_package(GSL REQUIRED)
  if (NOT GSL_FOUND)
  	message(FATAL_ERROR "GSL not found, won't compile libYARP_math")
  else (NOT GSL_FOUND)
  	set(YARP_HAS_MATH_LIB TRUE)
  endif (NOT GSL_FOUND)
endif(CREATE_LIB_MATH)

if (YARP_HAS_MATH_LIB)

  project(YARP_math)

  file(GLOB folder_source src/*.cpp)
  file(GLOB_RECURSE folder_header include/*.h)
  source_group("Source Files" FILES ${folder_source})
  source_group("Header Files" FILES ${folder_header})

  set_property(GLOBAL APPEND PROPERTY YARP_TREE_INCLUDE_DIRS 
    ${PROJECT_SOURCE_DIR}/include)
  get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
  include_directories(${YARP_TREE_INCLUDE_DIRS})

  set(YARP_USE_ATLAS FALSE CACHE BOOL "Select true to link to Atlas for BLAS")
  set(MATH_LIB_EXTRAS)
  if(YARP_USE_ATLAS)
     find_package(Atlas)
     set(MATH_LIB_EXTRAS ${ATLAS_CBLAS_LIBRARY} ${ATLAS_ATLAS_LIBRARY} ${GSL_LIBRARIES})
  else(YARP_USE_ATLAS)
     set(MATH_LIB_EXTRAS ${GSL_LIBRARIES})
  endif(YARP_USE_ATLAS)

  include_directories(${GSL_INCLUDE_DIRS})

  add_library(YARP_math ${folder_source} ${folder_header})
  target_link_libraries(YARP_math YARP_sig YARP_OS ${MATH_LIB_EXTRAS})
  install(TARGETS YARP_math EXPORT YARP COMPONENT runtime DESTINATION lib)
  set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_math)

  set_property(TARGET YARP_math PROPERTY INCLUDE_DIRS ${YARP_TREE_INCLUDE_DIRS})

endif(YARP_HAS_MATH_LIB)

set_property(GLOBAL PROPERTY YARP_HAS_MATH_LIB ${YARP_HAS_MATH_LIB})
