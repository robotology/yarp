TEMPLATE = lib
TARGET = QtYARPScopePlugin
QT += qml quick xml widgets printsupport
CONFIG += qt plugin

DESTDIR = imports/robotology/yarp/scope

TARGET = $$qtLibraryTarget($$TARGET)
uri = robotology.yarp.scope

linux-g++{
    INCLUDEPATH += /usr/local/src/yarp/src/libYARP_OS/include
    INCLUDEPATH += /usr/local/src/yarp/src/libYARP_sig/include
    INCLUDEPATH += /usr/local/src/yarp/src/libYARP_math/include
    INCLUDEPATH += /usr/local/src/yarp/src/libYARP_dev/include
    INCLUDEPATH += /usr/local/src/yarp/build/generated_include/

    LIBS += -L/usr/local/src/yarp/build

    LIBS += -lYARP_sig
    LIBS += -lYARP_init
    LIBS += -lyarpcar
    LIBS += -lYARP_wire_rep_utils
    LIBS += -lYARP_dev
    LIBS += -lyarp_tcpros
    LIBS += -lyarp_bayer
    LIBS += -lYARP_OS
    LIBS += -lyarp_human
}

win32-msvc2010{
    INCLUDEPATH += $$(YARP_ROOT)/src/libYARP_OS/include
    INCLUDEPATH += $$(YARP_ROOT)/src/libYARP_sig/include
    INCLUDEPATH += $$(YARP_ROOT)/src/libYARP_math/include
    INCLUDEPATH += $$(YARP_ROOT)/src/libYARP_dev/include
    INCLUDEPATH += $$(YARP_ROOT)/build/generated_include/

    CONFIG(debug, debug|release) {


        LIBS += -L$$(YARP_ROOT)/build/lib/Debug
        LIBS += -L"$$(ACE_ROOT)/lib"

        LIBS += -lYARP_sigd
        LIBS += -lYARP_initd
        LIBS += -lyarpcard
        #LIBS += -lYARP_wire_rep_utilsd
        LIBS += -lYARP_devd
        #LIBS += -lyarp_tcprosd
        LIBS += -lyarp_bayerd
        LIBS += -lYARP_OSd
        #LIBS += -lyarp_humand
        LIBS += -lACEd
        LIBS += -lWinmm
        LIBS += -lAdvapi32
        LIBS += -lShell32



    } else {
        LIBS += -L$$(YARP_ROOT)/build/lib/Release
        LIBS += -L"$$(ACE_ROOT)/lib"

        LIBS += -lYARP_sig
        LIBS += -lYARP_init
        LIBS += -lyarpcar
        LIBS += -lYARP_wire_rep_utils
        LIBS += -lYARP_dev
        LIBS += -lyarp_tcpros
        LIBS += -lyarp_bayer
        LIBS += -lYARP_OS
        LIBS += -lyarp_human
        LIBS += -lACE
        LIBS += -lWinmm
        LIBS += -lAdvapi32
        LIBS += -lShell32
    }
}



# Input
SOURCES += \
    qtyarpscopeplugin_plugin.cpp \
    qtyarpscope.cpp \
    simpleloader.cpp \
    plotmanager.cpp \
    plotter.cpp \
    genericloader.cpp \
    xmlloader.cpp \
    qcustomplot.cpp

HEADERS += \
    qtyarpscopeplugin_plugin.h \
    qtyarpscope.h \
    simpleloader.h \
    plotmanager.h \
    plotter.h \
    genericloader.h \
    xmlloader.h \
    qcustomplot.h

OTHER_FILES = $$DESTDIR/qmldir \
    YarpScopeToolBar.qml \
    YarpScopeMenu.qml \
    YarpScopeAbout.qml

!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copy_qmldir.target = $$OUT_PWD/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

qmldir.files = qmldir
unix {
    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}

RESOURCES += \
    res.qrc

