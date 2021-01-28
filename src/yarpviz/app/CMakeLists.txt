# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

set(PROJECTNAME yarpviz-plugins)

file(GLOB plugins ${CMAKE_CURRENT_SOURCE_DIR}/plugins/*.lua)
yarp_install(FILES ${plugins} DESTINATION ${YARP_CONTEXTS_INSTALL_DIR}/yarpviz)
