PROJECT(libYARP_sig)

INCLUDE_DIRECTORIES(include ../libYARP_OS/include ${ACE_INCLUDE_DIR})

# We just intend to use cmake to create visual studio project files -
# so ok not to manually include list of source code
AUX_SOURCE_DIRECTORY(src libcode)
ADD_LIBRARY(YARP_sig ${libcode})

LINK_LIBRARIES(YARP_sig YARP_OS ACE)

AUX_SOURCE_DIRECTORY(harness harnesscode)
ADD_EXECUTABLE(harness_sig ${harnesscode})

# add in our tests
FOREACH(X ${harnesscode})
	  GET_FILENAME_COMPONENT(XN ${X} NAME_WE)
	  ADD_TEST(YARP_sig::${XN} harness_sig regression ${XN})
ENDFOREACH(X $(harnesscode))

