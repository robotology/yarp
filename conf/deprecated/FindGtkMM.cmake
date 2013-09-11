# Copyright: (C) 2009 RobotCub Consortium
# Authors: Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


if(NOT YARP_NO_DEPRECATED)
include(${YARP_MODULE_DIR}/YarpDeprecatedWarning.cmake)
yarp_deprecated_warning("FindGtkMM.cmake is deprecated. Use FindGTK2 from CMake instead.")


if(UNIX)
    include(FindGtkMMUnix)
elseif(WIN32 AND NOT CYGWIN)
    include(FindGtkMMWin32)
endif(UNIX)

# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
    set_package_properties(GtkMM PROPERTIES DESCRIPTION "C++ interface for the GTK+ GUI library"
                                            URL "http://www.gtkmm.org/")
endif()


endif(NOT YARP_NO_DEPRECATED)
