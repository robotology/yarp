# Copyright: (C) 2013 IIT
# Authors: Elena Ceseracciu
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

yarp_install(FILES xml/applications/eyesviewer-cluster.xml.template DESTINATION ${YARP_APPLICATIONS_TEMPLATES_INSTALL_DIR})

set(app_examples xml/applications/eyesviewer-localhost.xml xml/applications/fakeye-localhost.xml)
yarp_install(FILES ${app_examples} DESTINATION ${YARP_APPLICATIONS_INSTALL_DIR})

yarp_install(FILES xml/modules/test_grabber.xml DESTINATION ${YARP_MODULES_INSTALL_DIR})

