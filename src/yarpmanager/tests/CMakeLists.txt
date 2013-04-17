# Copyright: (C) 2013 IIT
# Authors: Elena Ceseracciu
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

get_property(YARP_TREE_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)

configure_file(templates/application.xml.template ${CMAKE_BINARY_DIR}/templates/application.xml.template COPYONLY)
configure_file(templates/module.xml.template ${CMAKE_BINARY_DIR}/templates/module.xml.template COPYONLY)
configure_file(templates/resource.xml.template ${CMAKE_BINARY_DIR}/templates/resource.xml.template COPYONLY)

if(YARP_TREE_INCLUDE_DIRS)
    install(FILES ${CMAKE_BINARY_DIR}/templates/application.xml.template ${CMAKE_BINARY_DIR}/templates/module.xml.template ${CMAKE_BINARY_DIR}/templates/resource.xml.template
            DESTINATION share/yarp/templates)
endif(YARP_TREE_INCLUDE_DIRS)

set(app_examples eyesviewer-cluster.xml eyesviewer-localhost.xml fakeye-localhost.xml)

foreach(appname ${app_examples})
    configure_file(xml/applications/${appname} ${CMAKE_BINARY_DIR}/templates/app/${appname}.template COPYONLY)
if(YARP_TREE_INCLUDE_DIRS)
    install(FILES ${CMAKE_BINARY_DIR}/templates/app/${appname}.template 
            DESTINATION share/yarp/templates/app)
endif(YARP_TREE_INCLUDE_DIRS)

endforeach()
