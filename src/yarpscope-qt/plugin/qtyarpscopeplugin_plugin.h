#ifndef QTYARPSCOPEPLUGIN_PLUGIN_H
#define QTYARPSCOPEPLUGIN_PLUGIN_H

#include <QQmlExtensionPlugin>

class QtYARPScopePluginPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};

#endif // QTYARPSCOPEPLUGIN_PLUGIN_H

