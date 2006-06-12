# This is a small project to check if ace is working
PROJECT(ace_check)

# There is a script for finding the ACE library in the current directory
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

# We need ACE
FIND_PACKAGE(Ace REQUIRED)

# Find source code files
FILE(GLOB HEADERS *.h)
FILE(GLOB CODE *.cpp)

# Lay the files out in folders in environments where that makes sense
# This is completely optional
SOURCE_GROUP("Source Files" FILES ${HEADERS})
SOURCE_GROUP("Header Files" FILES ${CODE})

# Create our executable
ADD_EXECUTABLE(ace_check ${HEADERS} ${CODE})

# This is just for packaging up this example for distribution
IF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
   SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "An example for testing ACE")
   SET(CPACK_PACKAGE_VENDOR "VVV")
   SET(CPACK_PACKAGE_VERSION_MAJOR "1")
   SET(CPACK_PACKAGE_VERSION_MINOR "0")
   SET(CPACK_PACKAGE_VERSION_PATCH "0")
   INSTALL_FILES(/ FILES ${HEADERS} ${CODE} CMakeLists.txt)
   INCLUDE(CPack)
ENDIF(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
