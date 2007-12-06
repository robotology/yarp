
##################################################
#ask user to specify build type (linux only)
IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "Release" CACHE STRING 
	"Choose the type of build, recommanded options are: Debug or Release")
ENDIF(NOT CMAKE_BUILD_TYPE)
# hide variable to WINDOWS users (CMAKE_BUILD_TYPE is not used on win)
IF (WIN32 AND NOT CYGWIN)
  MARK_AS_ADVANCED(CMAKE_BUILD_TYPE)
ENDIF(WIN32 AND NOT CYGWIN)
#################################################


IF (BUILTIN_STDC)
IF(CMAKE_COMPILER_IS_GNUCXX)
  IF(EXISTS libstdc)
    MESSAGE(STATUS "libstc directory present, compiling libstdc++ in statically")
    LINK_DIRECTORIES("${PROJECT_SOURCE_DIR}/libstdc")
    ADD_DEFINITIONS(-static-libgcc)
    SET(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -static-libgcc")
  ENDIF(EXISTS libstdc)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)
ENDIF (BUILTIN_STDC)


# compile device library by default - safe, since individual devices
# are added on request
SET(CREATE_DEVICE_LIBRARY TRUE CACHE BOOL "Do you want to compile the device library")
SET(CREATE_DEVICE_LIBRARY_BUILTINS FALSE CACHE BOOL "Do you want to link specific hardware into the device library")
SET(CREATE_GUIS FALSE CACHE BOOL "Do you want to compile GUIs")

SET(CREATE_LIB_MATH FALSE CACHE BOOL "Create lib libYARP_math (experimental)?")

SET(CREATE_SHARED_LIBRARY FALSE CACHE BOOL "Compile shared libraries rather than linking statically")
IF (WIN32)
	MARK_AS_ADVANCED(CREATE_SHARED_LIBRARY)
ENDIF (WIN32)


# Flag for device testing and documentation - not really for end-user,
# but instead the library developers
SET(CREATE_BUILTIN_DEVICE_TESTS FALSE CACHE BOOL "Do you want to create tests for builtin devices")
MARK_AS_ADVANCED(CREATE_BUILTIN_DEVICE_TESTS CREATE_DEVICE_LIBRARY)
