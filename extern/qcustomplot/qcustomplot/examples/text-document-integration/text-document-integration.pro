#-------------------------------------------------
#
# Project created by QtCreator 2013-07-06T18:51:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = text-document-integration
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcpdocumentobject.cpp \
    ../../qcustomplot.cpp

HEADERS  += mainwindow.h \
    qcpdocumentobject.h \
    ../../qcustomplot.h

FORMS    += mainwindow.ui
