# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Arjan Gijsberts, Lorenzo Natale, Fabien Benureau, Stephane Lallee, Ali Paikan, Francesco Romano
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# this is set in the parent directory, but for some reason it does not get inherited
set_source_files_properties(${SWIG_BINDINGS_SOURCE_FILE} PROPERTIES CPLUSPLUS ON)


set(CMAKE_SWIG_OUTDIR "${CMAKE_CURRENT_BINARY_DIR}")
set(ALLEGRO_INSTALL_DIR "lib/allegro/site" CACHE PATH "Install path of allegro bindings.")
set(ALLEGRO_ASDF_SYSTEMS_DIR "lib/allegro/asdf-systems" CACHE PATH "Path to register asdf file.")

set(allegro_PROJ_FILE allegro/yarp.asd)
set(allegro_SRC_FILE yarp.cl)
set(allegro_LIBS yarp-allegro.so)

set(CMAKE_SWIG_FLAGS "-module;yarp;-identifier-converter;lispify;${SWIG_COMMON_FLAGS}")
swig_add_library(yarp_allegro
                 LANGUAGE allegrocl
                 SOURCES ${SWIG_BINDINGS_SOURCE_FILE})

swig_link_libraries(yarp_allegro ${SWIG_YARP_LIBRARIES})

string(REGEX REPLACE "\\.cl" ".lisp" allegro_DEST_FILE ${allegro_SRC_FILE})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${allegro_PROJ_FILE}
        DESTINATION ${ALLEGRO_INSTALL_DIR}/yarp)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${allegro_SRC_FILE}
        DESTINATION ${ALLEGRO_INSTALL_DIR}/yarp/src
        RENAME ${allegro_DEST_FILE})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${allegro_LIBS}
        DESTINATION ${ALLEGRO_INSTALL_DIR}/yarp/lib)

if(UNIX)
  install(CODE "execute_process(COMMAND \"\${CMAKE_COMMAND}\" \"-E\" \"make_directory\" \"${ALLEGRO_ASDF_SYSTEMS}\"
                                COMMAND \"ln\" \"-s\" \"${CMAKE_INSTALL_PREFIX}/${ALLEGRO_INSTALL_DIR}/yarp/yarp.asd\" \"${CMAKE_INSTALL_PREFIX}/${ALLEGRO_ASDF_SYSTEMS}\")")
endif()
