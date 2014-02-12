#include "qtyarpview_plugin.h"
#include "qtyarpview.h"

#include <qqml.h>


void QtYARPViewPlugin::registerTypes(const char *uri)
{
    // @uri robotology.yarp.view
    qmlRegisterType<QtYarpView>(uri, 1, 0, "QtYarpViewPlugin");
}


