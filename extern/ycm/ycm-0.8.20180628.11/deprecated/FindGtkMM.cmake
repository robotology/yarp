#.rst:
# FindGtkMM (Replaced by :cmake:module:`FindGTK2`)
# ------------------------------------------------
#
# .. warning:: This module is deprecated. You should use :cmake:module:`FindGTK2` instead.

#=============================================================================
# Copyright 2009 RobotCub Consortium
#   Authors: Giorgio Metta <giorgio.metta@iit.it>
#            Lorenzo Natale <lorenzo.natale@iit.it>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of YCM, substitute the full
#  License text for the above reference.)


include(${CMAKE_CURRENT_LIST_DIR}/YCMDeprecatedWarning.cmake)
ycm_deprecated_warning("FindGtkMM.cmake is deprecated. Use FindGTK2 from CMake instead.")

if(NOT YCM_NO_DEPRECATED)


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


endif(NOT YCM_NO_DEPRECATED)
