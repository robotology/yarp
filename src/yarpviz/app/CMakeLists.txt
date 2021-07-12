# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

set(PROJECTNAME yarpviz-plugins)

file(GLOB plugins ${CMAKE_CURRENT_SOURCE_DIR}/plugins/*.lua)
yarp_install(FILES ${plugins} DESTINATION ${YARP_CONTEXTS_INSTALL_DIR}/yarpviz)
