# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_install(FILES xml/applications/eyesviewer-cluster.xml.template DESTINATION ${YARP_APPLICATIONS_TEMPLATES_INSTALL_DIR})

set(app_examples xml/applications/eyesviewer-localhost.xml xml/applications/fakeye-localhost.xml)
yarp_install(FILES ${app_examples} DESTINATION ${YARP_APPLICATIONS_INSTALL_DIR})

yarp_install(FILES xml/modules/fakeFrameGrabber.xml DESTINATION ${YARP_MODULES_INSTALL_DIR})
yarp_install(FILES xml/modules/test_grabber.xml DESTINATION ${YARP_MODULES_INSTALL_DIR})
