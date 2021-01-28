# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

set(appname yarpdataplayer)

file(GLOB conf ${CMAKE_CURRENT_SOURCE_DIR}/conf/*.ini)
file(GLOB templates ${CMAKE_CURRENT_SOURCE_DIR}/scripts/*.template)
file(GLOB scripts ${CMAKE_CURRENT_SOURCE_DIR}/scripts/*.xml)

yarp_install(FILES ${conf} DESTINATION ${YARP_CONTEXTS_INSTALL_DIR}/${appname})
yarp_install(FILES ${templates} DESTINATION ${YARP_APPLICATIONS_TEMPLATES_INSTALL_DIR})
yarp_install(FILES ${scripts} DESTINATION ${YARP_APPLICATIONS_INSTALL_DIR})
