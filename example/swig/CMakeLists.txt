
# replace "..." with the path to your YARP binaries if needed
SET(YARP_DIR "${CMAKE_SOURCE_DIR}/../.." CACHE LOCATION "where is yarp?")

SET(CREATE_JAVA FALSE CACHE BOOL "Do you want to create a Java interface")
SET(CREATE_PYTHON FALSE CACHE BOOL "Do you want to create the Python interface")
SET(CREATE_PERL FALSE CACHE BOOL "Do you want to create the Perl interface")
SET(CREATE_CHICKEN FALSE CACHE BOOL "Do you want to create the Chicken Scheme interface")


# small update to FindJNI
SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})
FIND_PACKAGE(YARP REQUIRED)

SET_SOURCE_FILES_PROPERTIES(yarp.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(yarp.i PROPERTIES SWIG_FLAGS "-includeall")

IF (CREATE_JAVA)
   MAKE_DIRECTORY("generated_src")
   SET(CMAKE_SWIG_FLAGS "-outdir;generated_src;-Wall;-module;yarp")
   FIND_PACKAGE(JNI REQUIRED)
   INCLUDE_DIRECTORIES(${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2} )
   
   IF (WIN32 AND NOT CYGWIN)
      SWIG_ADD_MODULE(jyarp java yarp.i)
   ELSE(WIN32 AND NOT CYGWIN)
      SWIG_ADD_MODULE(libjyarp java yarp.i)
   ENDIF (WIN32 AND NOT CYGWIN)

ENDIF (CREATE_JAVA)

IF (CREATE_PYTHON)
   SET(CMAKE_SWIG_FLAGS "-Wall;-module;yarp")
   FIND_PACKAGE(PythonLibs)
   INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
   SWIG_ADD_MODULE(yarp python yarp.i)
ENDIF (CREATE_PYTHON)

IF (CREATE_PERL)
   SET(CMAKE_SWIG_FLAGS "-Wall;-module;yarp")
   FIND_PACKAGE(PerlLibs)
   INCLUDE_DIRECTORIES(${PERL_INCLUDE_PATH})
   SWIG_ADD_MODULE(libyarp perl5 yarp.i)
ENDIF (CREATE_PERL)

IF (CREATE_CHICKEN)
   SET(CMAKE_SWIG_FLAGS "-Wall;-module;cyarp")
   SWIG_ADD_MODULE(cyarp chicken yarp.i)
ENDIF (CREATE_CHICKEN)


