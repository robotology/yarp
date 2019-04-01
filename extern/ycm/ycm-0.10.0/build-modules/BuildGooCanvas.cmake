#.rst:
# BuildGooCanvas
# --------------
#
# GooCanvas

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

ycm_ep_helper(GooCanvas TYPE GIT
                        STYLE GNOME
                        REPOSITORY goocanvas.git
                        TAG goocanvas-1.0
                        CONFIGURE_COMMAND <SOURCE_DIR>/configure --enable-maintainer-mode --prefix=<INSTALL_DIR>)

externalproject_add_step(GooCanvas prepare
                         COMMAND NOCONFIGURE=1 <SOURCE_DIR>/autogen.sh
                         WORKING_DIRECTORY <SOURCE_DIR>
                         COMMENT "Performing prepare step (autogen.sh) for 'GooCanvas'"
                         DEPENDEES update
                         DEPENDERS configure)
