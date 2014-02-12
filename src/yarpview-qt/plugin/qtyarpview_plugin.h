#ifndef QTYARPVIEW_PLUGIN_H
#define QTYARPVIEW_PLUGIN_H

#include <QQmlExtensionPlugin>

class QtYARPViewPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};

#endif // QTYARPVIEW_PLUGIN_H

