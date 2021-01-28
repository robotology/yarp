/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#else
    qputenv("QT_DEVICE_PIXEL_RATIO", QByteArray("auto"));
#endif
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
    engine.load(QUrl("qrc:/qml/QtYARPScope/main.qml"));
    QObject *topLevel = engine.rootObjects().value(0);
    auto* window = qobject_cast<QQuickWindow *>(topLevel);

    // Pack the argc and argv to a QStringList so we can pass them easily to the plugin
    QStringList params;
    for(int i=0;i<argc;i++){
        params.append(argv[i]);
    }
    // Call the parseParameters of the toplevel object
    QMetaObject::invokeMethod(topLevel,"parseParameters",
                              Qt::DirectConnection,
                              Q_RETURN_ARG(QVariant, retVal),
                              Q_ARG(QVariant,params));
    if(!retVal.toBool()){
        return 1;
    }

    window->show();

    return (app.exec()!=0?1:0);
}
