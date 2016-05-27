# Copyright (C) 2014  iCub Facility, Istituto Italiano di Tecnologia
# Author: Ali Paikan <ali.paikan@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(CREATE_YARPMANAGER_PP AND YARP_HAS_QT5)

    include(YarpUseQt5)
    include(YarpOSXUtilities)

    get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
    get_property(YARP_manager_INCLUDE_DIRS TARGET YARP_manager PROPERTY INCLUDE_DIRS)
    include_directories(${YARP_OS_INCLUDE_DIRS}
                        ${YARP_manager_INCLUDE_DIRS}
                        ./src-builder
                        ./src-manager)

    #find_package(Qt5Widgets)
    #find_package(Qt5Declarative)
    #list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})
    #include(YarpUseQt5)

    set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

    #manager files
    set(manager_SRCS
        src-manager/aboutdlg.cpp
        src-manager/applicationviewwidget.cpp
        src-manager/customtreewidget.cpp
        src-manager/entitiestreewidget.cpp
        src-manager/genericinfodlg.cpp
        src-manager/genericviewwidget.cpp
        src-manager/logwidget.cpp
        src-manager/main.cpp
        src-manager/mainwindow.cpp
        src-manager/moduleviewwidget.cpp
        src-manager/newapplicationwizard.cpp
        src-manager/resourceviewwidget.cpp
        src-manager/safe_manager.cpp
        src-manager/stdoutwindow.cpp
        src-manager/yscopewindow.cpp)

    set(manager_HDRS
        src-manager/aboutdlg.h
        src-manager/applicationviewwidget.h
        src-manager/customtreewidget.h
        src-manager/entitiestreewidget.h
        src-manager/genericinfodlg.h
        src-manager/genericviewwidget.h
        src-manager/logwidget.h
        src-manager/mainwindow.h
        src-manager/moduleviewwidget.h
        src-manager/newapplicationwizard.h
        src-manager/resourceviewwidget.h
        src-manager/safe_manager.h
        src-manager/stdoutwindow.h
        src-manager/template_res.h
        src-manager/yscopewindow.h)

    set(manager_QRC_FILES src-manager/res_manager.qrc)
    if(WIN32)
	   set(yarpmanager_qt_RC_FILES yarpmanager.rc)
    endif()

    set(manager_UI_FILES
        src-manager/aboutdlg.ui
        src-manager/applicationviewwidget.ui
        src-manager/genericinfodlg.ui
        src-manager/mainwindow.ui
        src-manager/moduleviewwidget.ui
        src-manager/resourceviewwidget.ui
        src-manager/stdoutwindow.ui
        src-manager/yscopewindow.ui)

    # all builder files
    set(builder_HDRS
        src-builder/applicationitem.h
        src-builder/arrow.h
        src-builder/builderitem.h
        src-builder/builderscene.h
        src-builder/builderwindow.h
        src-builder/commons.h
        src-builder/customtree.h
        src-builder/destinationportitem.h
        src-builder/itemsignalhandler.h
        src-builder/moduleitem.h
        src-builder/propertiestable.h
        src-builder/sourceportitem.h
        src-builder/yarpbuilderlib_global.h
        src-builder/yarpbuilderlib.h )

    set(builder_SRCS
        src-builder/applicationitem.cpp
        src-builder/arrow.cpp
        src-builder/builderitem.cpp
        src-builder/builderscene.cpp
        src-builder/builderwindow.cpp
        src-builder/customtree.cpp
        src-builder/destinationportitem.cpp
        src-builder/itemsignalhandler.cpp
        src-builder/moduleitem.cpp
        src-builder/propertiestable.cpp
        src-builder/sourceportitem.cpp
        src-builder/yarpbuilderlib.cpp)

    set(builder_QRC_FILES src-builder/res_builder.qrc)

    qt5_add_resources(yarpmanager++_QRC_GEN_SRCS ${manager_QRC_FILES} ${builder_QRC_FILES})
    qt5_wrap_ui(yarpmanager++_UI_GEN_SRCS ${manager_UI_FILES} ${manager_UI_FILES})

    source_group("Source Files" FILES    ${manager_SRCS} ${builder_SRCS})
    source_group("Header Files" FILES    ${manager_HDRS} ${builder_HDRS})
    source_group("Resources Files" FILES ${manager_QRC_FILES} ${builder_QRC_FILES})
    source_group("UI Files" FILES        ${manager_UI_FILES} ${manager_UI_FILES})
    source_group("Generated Files" FILES ${yarpmanager++_QRC_GEN_SRCS}
                                         ${yarpmanager++_UI_GEN_SRCS})

    add_executable(yarpmanager++ WIN32 ${manager_SRCS} ${builder_SRCS}
                                       ${manager_HDRS} ${builder_HDRS}
                                       ${manager_QRC_FILES} ${builder_QRC_FILES}
                                       ${manager_UI_FILES} ${manager_UI_FILES}
                                       ${yarpmanager++_QRC_GEN_SRCS}
                                       ${yarpmanager++_UI_GEN_SRCS})

    target_link_libraries(yarpmanager++ YARP_OS
                                        YARP_init
                                        YARP_manager)
    qtyarp_use_modules(yarpmanager++ Widgets
                                     Gui)
    qtyarp_deprecate_with_cmake_version(2.8.11)
    if(WIN32 AND CMAKE_VERSION VERSION_LESS 2.8.11)
    target_link_libraries(yarpmanager++ Qt5::WinMain)
    endif()

    install(TARGETS yarpmanager++ COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})

    if(NOT YARP_DEFAULT_GTK)
    set_target_properties(yarpmanager++ PROPERTIES OUTPUT_NAME yarpmanager++)
    endif()

    yarp_osx_duplicate_and_add_bundle(TARGET_ORIG yarpmanager++
                                      TARGET_DEST yarpmanager++.app
                                      APP_ICON src-manager/Resources/AppIcon.icns
                                      INSTALL_COMPONENT utilities
                                      INSTALL_DESTINATION "${CMAKE_INSTALL_BINDIR}")

endif()
