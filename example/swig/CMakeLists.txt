CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# replace "..." with the path to your YARP binaries if needed
SET(YARP_DIR "${CMAKE_SOURCE_DIR}/../.." CACHE LOCATION "where is yarp?")

SET(CREATE_JAVA FALSE CACHE BOOL "Do you want to create a Java interface")
SET(CREATE_PYTHON FALSE CACHE BOOL "Do you want to create the Python interface")
SET(CREATE_PERL FALSE CACHE BOOL "Do you want to create the Perl interface")
SET(CREATE_CHICKEN FALSE CACHE BOOL "Do you want to create the Chicken Scheme interface")
SET(CREATE_CSHARP FALSE CACHE BOOL "Do you want to create the CSharp interface")
SET(CREATE_ALLEGRO FALSE CACHE BOOL "Do you want to create the Allegro Common Lisp interface")

SET(CREATE_TCL FALSE CACHE BOOL "Do you want to create the TCL interface")
SET(CREATE_RUBY FALSE CACHE BOOL "Do you want to create the Ruby interface")




# small update to FindJNI
SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})
MESSAGE(STATUS "${SWIG_USE_FILE}")
SET (MIN_SWIG_VERSION "1.3.30")
IF ("${SWIG_VERSION}" VERSION_LESS "${MIN_SWIG_VERSION}")
    MESSAGE( FATAL_ERROR "Please update your SWIG. Current version: ${SWIG_VERSION}. The minimum version needed: ${MIN_SWIG_VERSION}")
ENDIF ("${SWIG_VERSION}" VERSION_LESS "${MIN_SWIG_VERSION}")

FIND_PACKAGE(YARP REQUIRED)

SET_SOURCE_FILES_PROPERTIES(yarp.i PROPERTIES CPLUSPLUS ON)

# This causes a problem on Linux.
# If it is needed in some other system, please wrap in an IF().
# Thanks,
# -- paulfitz
# SET_SOURCE_FILES_PROPERTIES(yarp.i PROPERTIES SWIG_FLAGS "-includeall")

IF (CREATE_JAVA)
   MAKE_DIRECTORY("${CMAKE_CURRENT_BINARY_DIR}/generated_src")
   SET(CMAKE_SWIG_OUTDIR "${CMAKE_CURRENT_BINARY_DIR}/generated_src")
   SET(CMAKE_SWIG_FLAGS "-package;yarp;-Wall;-module;yarp")
   FIND_PACKAGE(JNI REQUIRED)
   INCLUDE_DIRECTORIES(${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2} )
   IF (WIN32 AND NOT CYGWIN)
      SET(target_name jyarp)
   ELSE(WIN32 AND NOT CYGWIN)
      SET(target_name libjyarp)
   ENDIF (WIN32 AND NOT CYGWIN)
   SWIG_ADD_MODULE(${target_name} java yarp.i)

   # issue on MINGW where C++ name-mangling prevents java finding methods
   IF (MINGW)
      MESSAGE(STATUS "untested MINGW patch - see CMakeLists.txt")
      SET_TARGET_PROPERTIES(${target_name} PROPERTIES 
                            LINK_FLAGS "--add-stdcall-alias")
   ENDIF (MINGW)
   ## the following is based on:
   ## http://www.cmake.org/pipermail/cmake/2007-September/016343.html
   IF(APPLE)
     SET_TARGET_PROPERTIES(${target_name} PROPERTIES SUFFIX ".jnilib")
   ENDIF(APPLE)
ENDIF (CREATE_JAVA)

IF (CREATE_PYTHON)
   SET(CMAKE_SWIG_FLAGS "-Wall;-module;yarp;-threads")
   FIND_PACKAGE(PythonLibs)
   INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
   SWIG_ADD_MODULE(yarp python yarp.i)
   # uncomment if we want a debug version of the library
   #ADD_DEFINITIONS(-DPy_DEBUG)

   # installation path is determined reliably on most platforms using distutils
   FIND_PACKAGE(PythonInterp)
   EXECUTE_PROCESS(COMMAND ${PYTHON_EXECUTABLE} 
                   -c "from distutils import sysconfig; print sysconfig.get_python_lib(1,0,prefix='${CMAKE_INSTALL_PREFIX}')" 
                   OUTPUT_VARIABLE PYTHON_INSTDIR OUTPUT_STRIP_TRAILING_WHITESPACE )
   INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/yarp.py ${CMAKE_CURRENT_SOURCE_DIR}/_yarp.so 
           DESTINATION ${PYTHON_INSTDIR} )
ENDIF (CREATE_PYTHON)

IF (CREATE_PERL)
   SET(CMAKE_SWIG_FLAGS "-Wall;-module;yarp")
   FIND_PACKAGE(PerlLibs)
   INCLUDE_DIRECTORIES(${PERL_INCLUDE_PATH})
   SWIG_ADD_MODULE(libperlyarp perl5 yarp.i)
ENDIF (CREATE_PERL)

IF (CREATE_CHICKEN)
   SET(CMAKE_SWIG_FLAGS "-Wall;-module;cyarp")
   SWIG_ADD_MODULE(libcyarp chicken yarp.i)
ENDIF (CREATE_CHICKEN)

IF (CREATE_ALLEGRO)
    set( CMAKE_SWIG_OUTDIR "${CMAKE_CURRENT_BINARY_DIR}" )
    set( ALLEGRO_INSTALL_PREFIX "/usr/local/lib/allegro/site" CACHE PATH "Install path prefix of allegro bindings." )
    set( ALLEGRO_ASDF_SYSTEMS "${ALLEGRO_INSTALL_PREFIX}/../asdf-systems" CACHE PATH "Path to register asdf file." )

    set( allegro_PROJ_FILE
        allegro/yarp.asd )

    set( allegro_SRC_FILE
        yarp.cl )

    set( allegro_LIBS
        yarp-allegro.so )

    set(CMAKE_SWIG_FLAGS "-Wall;-module;yarp;-identifier-converter;lispify")
    swig_add_module(yarp-allegro allegrocl yarp.i)

    string( REGEX REPLACE "\\.cl" ".lisp" allegro_DEST_FILE ${allegro_SRC_FILE})
    install( FILES ${CMAKE_CURRENT_SOURCE_DIR}/${allegro_PROJ_FILE} DESTINATION ${ALLEGRO_INSTALL_PREFIX}/yarp )
    install( FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${allegro_SRC_FILE}
        DESTINATION ${ALLEGRO_INSTALL_PREFIX}/yarp/src
        RENAME ${allegro_DEST_FILE} )
    install( FILES ${CMAKE_CURRENT_BINARY_DIR}/${allegro_LIBS} DESTINATION ${ALLEGRO_INSTALL_PREFIX}/yarp/lib/ )

    if( UNIX )
        install( CODE
            "execute_process( COMMAND \"mkdir\" \"-p\" \"${ALLEGRO_ASDF_SYSTEMS}\"
                              COMMAND \"ln\" \"-s\" \"${ALLEGRO_INSTALL_PREFIX}/yarp/yarp.asd\" \"${ALLEGRO_ASDF_SYSTEMS}\" )" )
    endif( UNIX )
ENDIF (CREATE_ALLEGRO)

IF (CREATE_TCL)
   SET(CMAKE_SWIG_FLAGS "-Wall;-module;yarp")
   FIND_PACKAGE(TCL REQUIRED)
   INCLUDE_DIRECTORIES(${TCL_INCLUDE_PATH})
   LINK_LIBRARIES(${TCL_LIBRARY})
   SWIG_ADD_MODULE(libtclyarp tcl yarp.i)
ENDIF (CREATE_TCL)


IF (CREATE_CSHARP)
   # outdir does not seem to work?
   SET(CMAKE_SWIG_FLAGS "-Wall;-module;yarp")
   IF (WIN32 AND NOT CYGWIN)
      SWIG_ADD_MODULE(yarp csharp yarp.i)
   ELSE(WIN32 AND NOT CYGWIN)
      SWIG_ADD_MODULE(yarp csharp yarp.i)
   ENDIF (WIN32 AND NOT CYGWIN)
ENDIF (CREATE_CSHARP)

IF (CREATE_RUBY)
   FIND_PACKAGE(Ruby)
   SET(CMAKE_SWIG_FLAGS "-Wall;-module;yarp")
   INCLUDE_DIRECTORIES(${RUBY_INCLUDE_PATH})
   SWIG_ADD_MODULE(yarp ruby yarp.i)
   SWIG_LINK_LIBRARIES(yarp ${RUBY_LIBRARY})
ENDIF (CREATE_RUBY)

