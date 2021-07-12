# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_install(FILES xml/applications/eyesviewer-cluster.xml.template DESTINATION ${YARP_APPLICATIONS_TEMPLATES_INSTALL_DIR})

set(app_examples xml/applications/eyesviewer-localhost.xml xml/applications/fakeye-localhost.xml)
yarp_install(FILES ${app_examples} DESTINATION ${YARP_APPLICATIONS_INSTALL_DIR})

yarp_install(FILES xml/modules/fakeFrameGrabber.xml DESTINATION ${YARP_MODULES_INSTALL_DIR})
yarp_install(FILES xml/modules/test_grabber.xml DESTINATION ${YARP_MODULES_INSTALL_DIR})
