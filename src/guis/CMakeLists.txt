# SPDX-FileCopyrightText: 2025 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(YARP_COMPILE_EXECUTABLES)
    # Qt5 GUIs
    add_subdirectory(yarpview)
    add_subdirectory(yarpscope)
    add_subdirectory(yarpmanager)
    add_subdirectory(yarplogger)
    add_subdirectory(yarpdataplayer)
    add_subdirectory(yarpmotorgui)
    add_subdirectory(yarpbatterygui)
    add_subdirectory(yarpmobilebasegui)
    add_subdirectory(yarpviz)
    add_subdirectory(yarpaudiocontrolgui)
    add_subdirectory(yarpllmgui)

    # Other GUIs
    add_subdirectory(yarplaserscannergui)
    add_subdirectory(yarpopencvdisplay)
endif()

