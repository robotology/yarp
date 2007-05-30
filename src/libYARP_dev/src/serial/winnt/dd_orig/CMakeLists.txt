PROJECT(aceserialhandler)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES($ENV{ACE_ROOT})

FILE(GLOB folder_header include/*.h)
FILE(GLOB folder_source src/*.cpp)


ADD_LIBRARY(aceserialhandler ${folder_source} ${folder_header})
INSTALL(TARGETS aceserialhandler DESTINATION "${PROJECT_SOURCE_DIR}/lib")



