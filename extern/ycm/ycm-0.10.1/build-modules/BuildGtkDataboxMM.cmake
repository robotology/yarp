#.rst:
# BuildGtkDataboxMM
# -----------------
#
# GtkDataboxMM

#=============================================================================
# Copyright 2013-2015 Istituto Italiano di Tecnologia (IIT)
#   Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
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


include(YCMEPHelper)
include(ExternalProject)

find_or_build_package(GtkDatabox QUIET)
if(COMMAND set_package_properties)
    set_package_properties(GtkDatabox PROPERTIES PURPOSE "Used by GtkDatabox")
endif()

ycm_ep_helper(GtkDataboxMM TYPE GIT
                           STYLE SOURCEFORGE
                           REPOSITORY gtkdataboxmm/code
                           DEPENDS GtkDatabox
                           TAG v0.9.4
                           CONFIGURE_COMMAND <SOURCE_DIR>/configure --enable-maintainer-mode --prefix=<INSTALL_DIR>)

externalproject_add_step(GtkDataboxMM prepare
                         COMMAND NOCONFIGURE=1 <SOURCE_DIR>/autogen.sh
                         WORKING_DIRECTORY <SOURCE_DIR>
                         COMMENT "Performing prepare step (autogen.sh) for 'GtkDataboxMM'"
                         DEPENDEES update
                         DEPENDERS configure)

