PROJECT(libYARP_OS_compat)

INCLUDE_DIRECTORIES(include ../libYARP_OS/include ${ACE_INCLUDE_DIR})

# We just intend to use cmake to create visual studio project files -
# so ok not to manually include list of source code
AUX_SOURCE_DIRECTORY(src libcode)
ADD_LIBRARY(YARP_OS_compat ${libcode})

LINK_LIBRARIES(YARP_OS_compat YARP_OS ACE)

AUX_SOURCE_DIRECTORY(harness harnesscode)
ADD_EXECUTABLE(harness_os_compat ${harnesscode})

# add in our tests
FOREACH(X ${harnesscode})
	  GET_FILENAME_COMPONENT(XN ${X} NAME_WE)
	  ADD_TEST(YARP_OS_compat::${XN} harness_os_compat regression ${XN})
ENDFOREACH(X $(harnesscode))

