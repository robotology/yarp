#-------------------------------------------------
#
# Project created by QtCreator 2014-08-06T23:57:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtYARPLogger
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    logtab.cpp \
    advanced_dialog.cpp \
    messagewidget.cpp

HEADERS  += mainwindow.h \
    logtab.h \
    advanced_dialog.h \
    messagewidget.h

FORMS    += mainwindow.ui \
    logtab.ui \
    advanced_dialog.ui

RESOURCES += \
    res.qrc
