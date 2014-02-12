#include "qtyarpscopeplugin_plugin.h"
#include "qtyarpscope.h"
#include <qqml.h>

void QtYARPScopePluginPlugin::registerTypes(const char *uri)
{
    // @uri robotology.yarp.scope
    qmlRegisterType<QtYARPScope>(uri, 1, 0, "QtYARPScopePlugin");
}


