# TinyXML
option(YARP_USE_SYSTEM_TINYXML "Use system TinyXML library" OFF)

if(YARP_USE_SYSTEM_TINYXML)
    find_package(TinyXML REQUIRED)
else(YARP_USE_SYSTEM_TINYXML)
    add_subdirectory(tinyxml)
endif(YARP_USE_SYSTEM_TINYXML)


#GtkDatabox
option(YARP_USE_SYSTEM_GTKDATABOX "Use system GtkDatabox library" OFF)

if(YARP_USE_SYSTEM_GTKDATABOX)
    find_package(GtkDatabox REQUIRED)
else(YARP_USE_SYSTEM_GTKDATABOX)
    add_subdirectory(gtkdatabox)
endif(YARP_USE_SYSTEM_GTKDATABOX)
