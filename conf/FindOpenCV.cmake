set(USE_NEW_FIND_OPENCV FALSE CACHE BOOL "Experimental: enable usage of FindOpenCV.cmake in cmake")
mark_as_advanced(USE_NEW_FIND_OPENCV)

if (USE_NEW_FIND_OPENCV)

  # Using FindOpenCV.cmake in cmake
  # save current CMAKE_MODULE_PATH, disable it 
  # to avoid recursive calls to FindOpenCV
  set(_CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})
  set(CMAKE_MODULE_PATH "")

  find_package(OpenCV)

  # push back original CMAKE_MODULE_PATH
  set(CMAKE_MODULE_PATH ${_CMAKE_MODULE_PATH})

  if (OpenCV_FOUND)
    find_path(OpenCV_INCLUDE_DIRS "cv.h" PATHS "${OpenCV_DIR}" PATH_SUFFIXES "include" "include/opencv" DOC "Include directory") 
    find_path(OpenCV_INCLUDE_DIR "cv.h" PATHS "${OpenCV_DIR}" PATH_SUFFIXES "include" "include/opencv" DOC "Include directory")
    set(OpenCV_LIBRARIES "${OpenCV_LIBS}" CACHE STRING "" FORCE)
  endif()

else()
  message("--> Using old FindOpenCV.cmake")
  set(CMAKE_MODULE_PATH ${_CMAKE_MODULE_PATH})
  include(FindOpenCV-yarp)
endif()
