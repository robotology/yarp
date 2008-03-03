MACRO(YarpProgram name no_console)

FOREACH(X ${NEED_LIBS})
	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/lib${X}/include)
ENDFOREACH(X ${NEED_LIBS})
SET(header_path ${CMAKE_SOURCE_DIR}/src/lib${name}/include)
INCLUDE_DIRECTORIES(${header_path})

FILE(GLOB folder_source *.cpp)
FILE(GLOB folder_header *.h)
SOURCE_GROUP("Source Files" FILES ${folder_source})
SOURCE_GROUP("Header Files" FILES ${folder_header})

IF (${no_console} STREQUAL NO_CONSOLE)
  #WIN32 in windows creates an application without
  #console, this flag is probably ignored in linux
  ADD_EXECUTABLE(${name} WIN32 ${folder_source} ${folder_header}) 
ELSE (${no_console} STREQUAL NO_CONSOLE)
  ADD_EXECUTABLE(${name} ${folder_source} ${folder_header})
ENDIF (${no_console} STREQUAL NO_CONSOLE)

FOREACH(X ${NEED_LIBS})
	#MESSAGE(STATUS "Linking for ${name} : ${${X}_LIB}")
	TARGET_LINK_LIBRARIES(${name} ${X})
ENDFOREACH(X ${NEED_LIBS})
TARGET_LINK_LIBRARIES(${name} ${ACE_LINK_FLAGS})

# disable a set of warnings due to use of deprecated features/libs.
IF(MSVC)
    SET_TARGET_PROPERTIES(${name} PROPERTIES COMPILE_FLAGS /wd4996)
ENDIF(MSVC)

ENDMACRO(YarpProgram)

