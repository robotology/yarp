# Copyright: (C) 2012 IITRBCS
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

include(GNUInstallDirs)

file(GLOB examples RELATIVE ${CMAKE_SOURCE_DIR}/bindings bindings/example*.*)
foreach(f CMakeLists.txt yarp.i README swig-allegro-and-chicken-fix.patch compat.h ${examples})
    install(FILES ${CMAKE_SOURCE_DIR}/bindings/${f}
            COMPONENT development
            DESTINATION ${CMAKE_INSTALL_DATADIR}/yarp/bindings)
endforeach(f)

file(GLOB java_srcs RELATIVE ${CMAKE_SOURCE_DIR}/bindings/src bindings/src/*.java)
foreach(f ${java_srcs})
    install(FILES ${CMAKE_SOURCE_DIR}/bindings/src/${f}
            COMPONENT development
            DESTINATION ${CMAKE_INSTALL_DATADIR}/yarp/bindings/src)
endforeach(f)
