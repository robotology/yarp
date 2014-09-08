#-------------------------------------------------
#
# Project created by QtCreator 2014-02-03T09:21:15
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtYARPManager
TEMPLATE = app

linux-g++{
    INCLUDEPATH += /usr/local/src/yarp/src/libYARP_OS/include
    INCLUDEPATH += /usr/local/src/yarp/src/libYARP_dev/include
    INCLUDEPATH += /usr/local/build/generated_include/
    INCLUDEPATH += /usr/local/src/yarpmanager/libymanager/include
}

win32-msvc2010{
    INCLUDEPATH += $$(YARP_ROOT)/src/libYARP_OS/include
    INCLUDEPATH += $$(YARP_ROOT)/src/libYARP_dev/include
    INCLUDEPATH += $$(YARP_ROOT)/build/generated_include/
    INCLUDEPATH += $$(YARP_ROOT)/src/yarpmanager/libymanager/include

    CONFIG(debug, debug|release) {
        LIBS += -L$$(YARP_ROOT)/build/lib/Debug
        LIBS += -L"$$(ACE_ROOT)/lib"
        LIBS += -L"$$(GSL_DIR)/lib"

        LIBS += -lYARP_mathd
        LIBS += -lYARP_managerd
        LIBS += -lYARP_tinyxmld
        LIBS += -lYARP_OSd
        LIBS += -lYARP_sigd
        LIBS += -lYARP_initd
        LIBS += -lgsl
        LIBS += -lgslcblas
        LIBS += -lyarpcard
        LIBS += -lYARP_wire_rep_utilsd
        LIBS += -lyarp_tcprosd
        LIBS += -lyarp_bayerd
        LIBS += -lyarp_humand
        LIBS += -lACEd
        LIBS += -lWinmm
        LIBS += -lAdvapi32
        LIBS += -lShell32
    }else{
        LIBS += -L$$(YARP_ROOT)/build/lib/Release
        LIBS += -L"$$(ACE_ROOT)/lib"
        LIBS += -L"$$(GSL_DIR)/lib"

        LIBS += -lYARP_math
        LIBS += -lYARP_manager
        LIBS += -lYARP_tinyxml
        LIBS += -lYARP_OS
        LIBS += -lYARP_sig
        LIBS += -lYARP_init
        LIBS += -lgsl
        LIBS += -lgslcblas
        LIBS += -lyarpcar
        LIBS += -lYARP_wire_rep_utils
        LIBS += -lyarp_tcpros
        LIBS += -lyarp_bayer
        LIBS += -lyarp_human
        LIBS += -lACE
        LIBS += -lWinmm
        LIBS += -lAdvapi32
        LIBS += -lShell32
    }
}

SOURCES += main.cpp\
        mainwindow.cpp \
    entitiestreewidget.cpp \
    moduleviewwidget.cpp \
    applicationviewwidget.cpp \
    safe_manager.cpp \
    genericviewwidget.cpp \
    customtreewidget.cpp \
    resourceviewwidget.cpp \
    yscopewindow.cpp \
    logwidget.cpp \
    stdoutwindow.cpp

HEADERS  += mainwindow.h \
    entitiestreewidget.h \
    moduleviewwidget.h \
    applicationviewwidget.h \
    safe_manager.h \
    genericviewwidget.h \
    customtreewidget.h \
    template_res.h \
    resourceviewwidget.h \
    yscopewindow.h \
    logwidget.h \
    stdoutwindow.h

FORMS    += mainwindow.ui \
    moduleviewwidget.ui \
    applicationviewwidget.ui \
    resourceviewwidget.ui \
    yscopewindow.ui \
    stdoutwindow.ui

RESOURCES += \
    res.qrc
