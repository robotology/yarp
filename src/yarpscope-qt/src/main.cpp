#include <QtGui/QGuiApplication>
#include "qtquick2applicationviewer.h"

#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QtWidgets/QApplication>
#include <QQmlContext>
#include <QVariant>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QVariant retVal;

    // De-comment this to trace all imports
    /*QByteArray data = "1";
    qputenv("QML_IMPORT_TRACE", data);*/


    QQmlApplicationEngine engine;
    engine.load(QUrl("qrc:/qml/QtYARPScope/main.qml"));
    QObject *topLevel = engine.rootObjects().value(0);
    QQuickWindow *window = qobject_cast<QQuickWindow *>(topLevel);


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
