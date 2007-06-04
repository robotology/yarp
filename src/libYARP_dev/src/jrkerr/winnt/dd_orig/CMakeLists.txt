PROJECT(jrkerrlib)


INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)

FILE(GLOB folder_header include/iCub/*.h)
FILE(GLOB folder_source src/*.cpp)



SET(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}")
ADD_LIBRARY(jrkerrlib ${folder_source} ${folder_header})
INSTALL(TARGETS jrkerrlib ARCHIVE DESTINATION lib)



