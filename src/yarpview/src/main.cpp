/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <QtGui/QGuiApplication>
#include "qtquick2applicationviewer.h"
#include "config.h"

#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QtWidgets/QApplication>
#include <QQmlContext>
#include <QVariant>
#include <QDir>

/*! \brief Main method for the YARPView container.
 *
 *  \param argc
 *  \param argv
 */
int main(int argc, char *argv[])
{
    bool minimal=false;
    bool compact=false;
    bool keepabove=false;
    // Pack the argc and argv to a QStringList so we can pass them easily to
    // the plugin.
    // This list must be packed before creating the QApplication, because
    // QApplication will remove the known arguments, and this includes the
    // "--name <name>" argument on linux.
    QStringList params;
    for(int i=1;i<argc;i++){
        params.append(argv[i]);
        if (std::string(argv[i]) == "--compact")
        {
            compact=true;
        }
        if (std::string(argv[i]) == "--minimal")
        {
            minimal=true;
        }
        if (std::string(argv[i]) == "--keep-above")
        {
            keepabove=true;
        }
    }

    QApplication app(argc, argv);
    QVariant retVal;

    // De-comment this to trace all imports
    /*QByteArray data = "1";
    qputenv("QML_IMPORT_TRACE", data);*/

    QQmlApplicationEngine engine;
    engine.addImportPath(QDir::cleanPath(QCoreApplication::applicationDirPath() + QDir::separator() +
                                         PLUGINS_RELATIVE_PATH));
#ifdef CMAKE_INTDIR
    engine.addImportPath(QDir::cleanPath(QCoreApplication::applicationDirPath() + QDir::separator() +
                                         ".." + QDir::separator() +
                                         PLUGINS_RELATIVE_PATH + QDir::separator() +
                                         CMAKE_INTDIR));
#endif
    if (compact || minimal)
    {
        engine.load(QUrl("qrc:/qml/QtYARPView/mainCompact.qml"));
    }
    else
    {
        engine.load(QUrl("qrc:/qml/QtYARPView/main.qml"));
    }

    QObject *topLevel = engine.rootObjects().value(0);

    if(!topLevel){
        return 1;
    }

    QQuickWindow *window = qobject_cast<QQuickWindow *>(topLevel);
    if (minimal)
    {
        window->setFlags(Qt::FramelessWindowHint);
    }
    if (keepabove)
    {
        window->setFlags(Qt::WindowStaysOnTopHint);
    }

    // Call the parseParameters of the qml object called YARPVideoSurface
    QObject *yarpVideoSurface = topLevel->findChild<QObject*>("YARPVideoSurface");
    QMetaObject::invokeMethod(yarpVideoSurface,"parseParameters",
                              Qt::DirectConnection,
                              Q_RETURN_ARG(QVariant, retVal),
                              Q_ARG(QVariant,params));
    if(!retVal.toBool()){
        return 1;
    }

    window->show();
    window->setIcon(QIcon(":/logo.png"));
    // window->setIcon(QIcon(":/logo.svg"));

    return (app.exec()!=0?1:0);
}

