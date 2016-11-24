#include "yarpbuilderlib.h"


YarpBuilderLib::YarpBuilderLib()
{
}

BuilderWindow *YarpBuilderLib::getBuilder(Application *app, Manager *lazyManager, SafeManager *manager, bool editingMode)
{
    return new BuilderWindow(app,lazyManager,manager,editingMode);
}

