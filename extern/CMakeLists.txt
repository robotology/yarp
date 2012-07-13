# TinyXML
option(YARP_USE_SYSTEM_TINYXML "Use system TinyXML library" OFF)

if(YARP_USE_SYSTEM_TINYXML)
    find_package(TinyXML REQUIRED)
else(YARP_USE_SYSTEM_TINYXML)
    add_subdirectory(tinyxml)
endif(YARP_USE_SYSTEM_TINYXML)
