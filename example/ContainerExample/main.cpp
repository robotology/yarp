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

/*
 * In this example we will create a single application that contains two plugins
 */

#include <QtGui/QGuiApplication>
#include "qtquick2applicationviewer.h"

#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QtWidgets/QApplication>
#include <QQmlContext>
#include <QVariant>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QVariant retVal;

    QQmlApplicationEngine engine;
    engine.load(QUrl("qrc:/qml/ContainerExample/main.qml"));

    QObject *topLevel = engine.rootObjects().value(0);
    QQuickWindow *window = qobject_cast<QQuickWindow *>(topLevel);

    // We call the parseParameters function of the toplevel qml object that will call
    // the parseParameters functions of the two plugins.
    // In this example we can also use in command line the commands for scope and for view
    // for example:
    // -- xml path/to/the/file.xml --name /view --synch
    // will load an xml file containing the configuration of the scope and give the name /view
    // to the view and will synch the view with the data acquisition.
    QStringList params;
    for(int i=1;i<argc;i++){
        params.append(argv[i]);
    }
    QMetaObject::invokeMethod(topLevel,"parseParameters",
                              Qt::DirectConnection,
                              Q_RETURN_ARG(QVariant, retVal),
                              Q_ARG(QVariant,params));
    if(!retVal.toBool()){
        return 0;
    }

    window->show();

    return app.exec();
}
