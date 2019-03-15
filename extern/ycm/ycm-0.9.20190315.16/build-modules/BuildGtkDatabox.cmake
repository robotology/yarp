#.rst:
# BuildGtkDatabox
# ---------------
#
# GtkDatabox

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

ycm_ep_helper(GtkDatabox TYPE GIT
                        STYLE SOURCEFORGE
                        REPOSITORY gtkdatabox/git
                        TAG GTK2
                        CONFIGURE_COMMAND <SOURCE_DIR>/configure --enable-maintainer-mode --prefix=<INSTALL_DIR>)

externalproject_add_step(GtkDatabox prepare
                         COMMAND autoreconf --force --install --verbose <SOURCE_DIR>
                         WORKING_DIRECTORY <SOURCE_DIR>
                         COMMENT "Performing prepare step (autogen.sh) for 'GtkDatabox'"
                         DEPENDEES update
                         DEPENDERS configure)

