/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include <QtGui/QGuiApplication>
#include "qtquick2applicationviewer.h"
#include "config.h"

#include <csignal>

#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QtWidgets/QApplication>
#include <QQmlContext>
#include <QVariant>
#include <QDir>
#include <QtGlobal>

void catchSignals(int sig) {
    // blocking and not aysnc-signal-safe func are valid
    printf("\nYarpview killed by signal(%d).\n", sig);
    QCoreApplication::quit();
}

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

#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#else
    qputenv("QT_DEVICE_PIXEL_RATIO", QByteArray("auto"));
#endif
    QApplication app(argc, argv);
    // add SIGINT and SIGTERM handler
    std::signal(SIGINT, catchSignals);
    std::signal(SIGTERM, catchSignals);

#if !defined(_WIN32)
    std::signal(SIGQUIT, catchSignals);
    std::signal(SIGHUP, catchSignals);
#endif

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

    auto* window = qobject_cast<QQuickWindow *>(topLevel);
    if (minimal)
    {
        window->setFlags(Qt::FramelessWindowHint);
    }
    if (keepabove)
    {
        window->setFlags(Qt::WindowStaysOnTopHint);
    }

    // Call the parseParameters of the qml object called YARPVideoSurface
    auto* yarpVideoSurface = topLevel->findChild<QObject*>("YARPVideoSurface");
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
