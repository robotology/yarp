# CMakeList.txt for SerialPortTestApp
PROJECT(SerialPortTestApp)

# Make sure the compiler can find include files from our Hello library.
INCLUDE_DIRECTORIES(${SerialPortTestApp_SOURCE_DIR})
INCLUDE_DIRECTORIES(${SerialPortTestApp_SOURCE_DIR}/../../DGSCommon)
INCLUDE_DIRECTORIES(${SerialPortTestApp_SOURCE_DIR}/../../DGS_MCU_Driver)
INCLUDE_DIRECTORIES(${SerialPortTestApp_SOURCE_DIR}/../../mailman)
INCLUDE_DIRECTORIES(${SerialPortTestApp_SOURCE_DIR}/../cppunit-1.12.0/include)
INCLUDE_DIRECTORIES($ENV{RESCUER_ROOT}/include)
INCLUDE_DIRECTORIES($ENV{ACE_ROOT})

SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
SET(EXECUTABLE_OUTPUT_PATH .)
SET(COMMONCODE ${SerialPortTestApp_SOURCE_DIR}/../../DGSCommon) 
SET(IESCODE ${SerialPortTestApp_SOURCE_DIR}/../../mailman)
SET(CPPUNITLIBNAMES cppunitd libcppunit.a cppunit )

# Make sure the linker can find the Hello library once it is built.
LINK_DIRECTORIES(${SerialPortTestApp_SOURCE_DIR})

# Add executable 
ADD_EXECUTABLE(SerialPortTestApp 
    SerialPortTestApp.cpp
    SerialPortTest.cpp
    ${COMMONCODE}/SerialHandler.cpp
    ${COMMONCODE}/SerialFeedbackData.cpp
    ${COMMONCODE}/DGSLogManager.cpp)

# look for the CppUnit Library
IF(WIN32)
FIND_LIBRARY(CPPUNIT_LIBRARY NAMES ${CPPUNITLIBNAMES}
    PATHS  ${SerialPortTestApp_SOURCE_DIR}/../../../lib/winnt ${SerialPortTestApp_SOURCE_DIR}/../../../lib/linux
    /usr/local/lib 
    [NO_SYSTEM_ENVIRONMENT_PATH]
    [NO_DEFAULT_PATH]) 
##IF(UNIX)
##      SET(CPPUNIT_LIBRARY -Wl,-Bstatic ${CPPUNIT_LIBRARY} -Wl,-Bdynamic)
##ENDIF(UNIX)
IF (CPPUNIT_LIBRARY)
TARGET_LINK_LIBRARIES(SerialPortTestApp ${CPPUNIT_LIBRARY})
ENDIF (CPPUNIT_LIBRARY)
ENDIF(WIN32)

IF(UNIX)
TARGET_LINK_LIBRARIES(SerialPortTestApp /usr/local/lib/libcppunit.a)
ENDIF(UNIX)

# look for Barrett Hand library
FIND_LIBRARY(BHAND_LIBRARY NAMES bhand libbhand.a
  PATHS  $ENV{RESCUER_ROOT}/lib/winnt $ENV{RESCUER_ROOT}/lib/linux
  $ENV{RESCUER_ROOT}/lib)
IF (BHAND_LIBRARY)
  TARGET_LINK_LIBRARIES(SerialPortTestApp ${BHAND_LIBRARY})
ENDIF (BHAND_LIBRARY)
##TARGET_LINK_LIBRARIES(SerialPortTestApp $ENV{RESCUER_ROOT}/lib/libbhand.a)

# look for the ACE Library
FIND_LIBRARY(ACE_LIBRARY NAMES aced ace 
  PATHS  $ENV{RESCUER_ROOT}/lib/winnt $ENV{RESCUER_ROOT}/lib/linux)
IF (ACE_LIBRARY)
  TARGET_LINK_LIBRARIES(SerialPortTestApp ${ACE_LIBRARY})
ENDIF (ACE_LIBRARY)
