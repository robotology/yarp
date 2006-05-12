
MACRO(YarpLibrary name postfix)

PROJECT(lib${name})

FOREACH(X ${NEED_LIBS})
	INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/lib${X}/include)
ENDFOREACH(X ${NEED_LIBS})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/lib${name}/include)

AUX_SOURCE_DIRECTORY(src libcode)

INCLUDE(YarpReqLib)

ADD_LIBRARY(${name} ${libcode})

AUX_SOURCE_DIRECTORY(harness harnesscode)
ADD_EXECUTABLE(harness_${postfix} ${harnesscode})

TARGET_LINK_LIBRARIES(harness_${postfix} ${name} ${NEED_LIBS})

GET_TARGET_PROPERTY(EXEC harness_${postfix} LOCATION)

# add in our tests
FOREACH(X ${harnesscode})
	  GET_FILENAME_COMPONENT(XN ${X} NAME_WE)
	  ADD_TEST(${name}::${XN} ${EXEC} regression ${XN})
ENDFOREACH(X $(harnesscode))

INSTALL_TARGETS(/lib ${name})

ENDMACRO(YarpLibrary)

