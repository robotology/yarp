/*
 * Copyright (C) 2009 RobotCub Consortium, European Commission FP6 Project IST-004370
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

/*! \brief Main method for the YarpView container.
 *
 *  \param argc
 *  \param argv
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QVariant retVal;

    // De-comment this to trace all imports
    /*QByteArray data = "1";
    qputenv("QML_IMPORT_TRACE", data);*/


    QQmlApplicationEngine engine;
#ifdef WIN32
    engine.addImportPath(QCoreApplication::applicationDirPath() + "\\" + PLUGINS_RELATIVE_PATH);
#else
    engine.addImportPath(QCoreApplication::applicationDirPath() + "/" + PLUGINS_RELATIVE_PATH);
#endif
    engine.load(QUrl("qrc:/qml/QtYARPView/main.qml"));
    QObject *topLevel = engine.rootObjects().value(0);

    if(!topLevel){
        return 0;
    }

    QQuickWindow *window = qobject_cast<QQuickWindow *>(topLevel);

    // Pack the argc and argv to a QStrinList so we can pass them easily to the plugin
    QStringList params;
    for(int i=1;i<argc;i++){
        params.append(argv[i]);
    }
    // Call the parseParameters of the qml object called YarpVideoSurface
    QObject *yarpVideoSurface = topLevel->findChild<QObject*>("YarpVideoSurface");
    QMetaObject::invokeMethod(yarpVideoSurface,"parseParameters",
                              Qt::DirectConnection,
                              Q_RETURN_ARG(QVariant, retVal),
                              Q_ARG(QVariant,params));
    if(!retVal.toBool()){
        return 0;
    }


    window->show();

    return app.exec();

}

